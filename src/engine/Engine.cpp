#include "Engine.hpp"

Engine::Engine(const std::string title, const Vec2i screenSize, const Vec2i pixelSize, const int frameRate)
    : title_(title), screenSize_(screenSize), pixelSize_(pixelSize), quit_(false), frameRateLimiter_(frameRate)
{
	// Initialize SDL2 related components
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		fprintf(stderr, "could not init SDL2: %s\n", SDL_GetError());
	if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))
		fprintf(stderr, "could not init SDL2_image: %s\n", IMG_GetError());
	if (TTF_Init() != 0)
		fprintf(stderr, "SDL_ttf initialization failed: %s\n", TTF_GetError());
	// Mix_Init(); // automatically called when opening a music file
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
		fprintf(stderr, "Audio initialization failed: %s\n", Mix_GetError());

	font_ = std::unique_ptr<TTF_Font, SDL_Deleter>(TTF_OpenFontDPI(FONT_ARIAL, 8, 100, 200));
}

Engine::~Engine()
{
	Mix_CloseAudio();
	Mix_Quit();      // if we need Mix_Init(), we also need to call this
	font_.release(); // font needs to be released before TTF_Quit(), otherwise throws
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void Engine::start()
{
	windowSize_ = screenSize_ * pixelSize_;

	// Initialize renderer and open window
	window_ = std::unique_ptr<SDL_Window, SDL_Deleter>(
	    SDL_CreateWindow(title_.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowSize_.x, windowSize_.y,
	                     SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE));
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	renderer_ =
	    std::unique_ptr<SDL_Renderer, SDL_Deleter>(SDL_CreateRenderer(window_.get(), -1, SDL_RENDERER_ACCELERATED));

	// Set the scaling factor
	SDL_RenderSetScale(renderer_.get(), pixelSize_.x, pixelSize_.y);

	// Call the user's start method
	onStart();

	FrameTimer frameTimer;

	while (!quit_) {
		frameRateLimiter_.startFrame();
		frameTimer.update();
		keyboard_.reset();
		mouse_.reset();

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				// Handle close window event
				quit_ = true;
			}
			keyboard_.update(event);
			mouse_.update(event);
		}

		clearWindow();

		// Run the user's update function
		onUpdate(frameTimer.getDeltaTime());

		// Flip buffers and display what was drawn to the renderer
		SDL_RenderPresent(renderer_.get());

		fpsCounter_.update();
		frameRateLimiter_.endFrame();
	}

	// Run user's destroy function
	onDestroy();
}

Vec2i Engine::getWindowSize() const
{
	return windowSize_;
}
void Engine::setWindowSize(Vec2i windowSize)
{
	windowSize_ = windowSize;
}
Vec2i Engine::getScreenSize() const
{
	return screenSize_;
}
void Engine::setScreenSize(Vec2i screenSize)
{
	screenSize_ = screenSize;
}
void Engine::setRenderScale(Vec2i pixelSize)
{
	pixelSize_ = pixelSize;
	SDL_RenderSetScale(renderer_.get(), pixelSize_.x, pixelSize_.y);
}
Vec2i Engine::getRenderScale() const
{
	return pixelSize_;
}
void Engine::resizeWindow(Vec2i pos, Vec2i size)
{
	// TODO: implement
}

void Engine::clearWindow() const
{
	SDL_SetRenderDrawColor(renderer_.get(), 0, 0, 0, 0);
	SDL_RenderClear(renderer_.get());
}

void Engine::drawPoint(const Vec2i &pos, const ColorRGBA &color) const
{
	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
	SDL_RenderDrawPoint(renderer_.get(), pos.x, pos.y);
}

void Engine::drawPoint(const Vec2f &pos, const ColorRGBA &color) const
{
	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
	SDL_RenderDrawPointF(renderer_.get(), pos.x, pos.y);
}

void Engine::drawLine(const Vec2i &start, const Vec2i &end, const ColorRGBA &color) const
{
	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
	SDL_RenderDrawLine(renderer_.get(), start.x, start.y, end.x, end.y);
}

void Engine::drawLine(const Vec2f &start, const Vec2f &end, const ColorRGBA &color) const
{
	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
	SDL_RenderDrawLineF(renderer_.get(), start.x, start.y, end.x, end.y);
}

void Engine::fillRectangle(const Vec2i &pos, const int &width, const int &height, const ColorRGBA &color) const
{
	SDL_Rect r;
	r.x = pos.x;
	r.y = pos.y;
	r.w = width;
	r.h = height;

	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer_.get(), &r);
}

void Engine::fillRectangle(const Vec2f &pos, const int &width, const int &height, const ColorRGBA &color) const
{
	SDL_FRect r;
	r.x = pos.x;
	r.y = pos.y;
	r.w = width;
	r.h = height;

	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
	SDL_RenderFillRectF(renderer_.get(), &r);
}

void Engine::drawRectangle(const Vec2i &pos, const int &width, const int &height, const ColorRGBA &color) const
{
	SDL_Rect r;
	r.x = pos.x;
	r.y = pos.y;
	r.w = width;
	r.h = height;

	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
	SDL_RenderDrawRect(renderer_.get(), &r);
}

void Engine::drawRectangle(const Vec2f &pos, const int &width, const int &height, const ColorRGBA &color) const
{
	SDL_FRect r;
	r.x = pos.x;
	r.y = pos.y;
	r.w = width;
	r.h = height;

	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
	SDL_RenderDrawRectF(renderer_.get(), &r);
}

void Engine::fillCircle(const Vec2i &pos, const int &radius, const ColorRGBA &color) const
{
	std::vector<SDL_Point> points;
	for (int i = -radius; i <= radius; i++) {
		for (int j = -radius; j <= radius; j++) {
			if (i * i + j * j <= radius * radius) {
				points.push_back({pos.x + i, pos.y + j});
			}
		}
	}

	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
	SDL_RenderDrawPoints(renderer_.get(), points.data(), static_cast<int>(points.size()));
}

void Engine::fillCircle(const Vec2f &pos, const int &radius, const ColorRGBA &color) const
{
	std::vector<SDL_FPoint> points;
	for (int i = -radius; i <= radius; i++) {
		for (int j = -radius; j <= radius; j++) {
			if (i * i + j * j <= radius * radius) {
				points.push_back({pos.x + i, pos.y + j});
			}
		}
	}

	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
	SDL_RenderDrawPointsF(renderer_.get(), points.data(), static_cast<int>(points.size()));
}

void Engine::drawCircle(const Vec2i &pos, const int &radius, const ColorRGBA &color) const
{
	// This method utilizes the Midpoint Circle Drawing Algorithm
	std::vector<SDL_Point> points;
	int xRadius = radius;
	int yRadius = 0;
	int error = 0;

	while (xRadius >= yRadius) {
		points.push_back({pos.x + xRadius, pos.y + yRadius});
		points.push_back({pos.x - xRadius, pos.y + yRadius});
		points.push_back({pos.x + xRadius, pos.y - yRadius});
		points.push_back({pos.x - xRadius, pos.y - yRadius});
		points.push_back({pos.x + yRadius, pos.y + xRadius});
		points.push_back({pos.x - yRadius, pos.y + xRadius});
		points.push_back({pos.x + yRadius, pos.y - xRadius});
		points.push_back({pos.x - yRadius, pos.y - xRadius});

		yRadius++;
		error += 1 + 2 * yRadius;
		if (2 * (error - xRadius) + 1 > 0) {
			xRadius--;
			error += 1 - 2 * xRadius;
		}
	}

	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
	SDL_RenderDrawPoints(renderer_.get(), points.data(), static_cast<int>(points.size()));
}

void Engine::drawCircle(const Vec2f &pos, const int &radius, const ColorRGBA &color) const
{
	// This method utilizes the Midpoint Circle Drawing Algorithm
	std::vector<SDL_FPoint> points;
	int xRadius = radius;
	int yRadius = 0;
	int error = 0;

	while (xRadius >= yRadius) {
		points.push_back({pos.x + xRadius, pos.y + yRadius});
		points.push_back({pos.x - xRadius, pos.y + yRadius});
		points.push_back({pos.x + xRadius, pos.y - yRadius});
		points.push_back({pos.x - xRadius, pos.y - yRadius});
		points.push_back({pos.x + yRadius, pos.y + xRadius});
		points.push_back({pos.x - yRadius, pos.y + xRadius});
		points.push_back({pos.x + yRadius, pos.y - xRadius});
		points.push_back({pos.x - yRadius, pos.y - xRadius});

		yRadius++;
		error += 1 + 2 * yRadius;
		if (2 * (error - xRadius) + 1 > 0) {
			xRadius--;
			error += 1 - 2 * xRadius;
		}
	}

	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
	SDL_RenderDrawPointsF(renderer_.get(), points.data(), static_cast<int>(points.size()));
}

void Engine::drawTexture(const std::shared_ptr<SDL_Texture> texture) const
{
	SDL_RenderCopy(renderer_.get(), texture.get(), NULL, NULL);
}

void Engine::drawTexture(const std::shared_ptr<SDL_Texture> texture, const SDL_Rect dst) const
{
	SDL_RenderCopy(renderer_.get(), texture.get(), NULL, &dst);
}

void Engine::drawTexture(const std::shared_ptr<SDL_Texture> texture, const SDL_Rect src, const SDL_Rect dst) const
{
	SDL_RenderCopy(renderer_.get(), texture.get(), &src, &dst);
}

void Engine::drawSpriteFromSheet(const Recti &src, const Recti &dst, SDL_Texture *spritesheet) const
{
	SDL_Rect sdlSrc = src.toSDLRect();
	SDL_Rect sdlDst = dst.toSDLRect();
	SDL_RenderCopy(renderer_.get(), spritesheet, &sdlSrc, &sdlDst);
}

void Engine::drawSpriteFromSheet(const Recti &src, const Rectf &dst, SDL_Texture *spritesheet) const
{
	SDL_Rect sdlSrc = src.toSDLRect();
	SDL_FRect sdlDst = dst.toSDLRect();
	SDL_RenderCopyF(renderer_.get(), spritesheet, &sdlSrc, &sdlDst);
}

void Engine::drawSpriteFromSheet(const Recti &src, const Recti &dst, SDL_Texture *spritesheet, double angle,
                                 SDL_Point *center, SDL_RendererFlip flip) const
{
	SDL_Rect sdlSrc = src.toSDLRect();
	SDL_Rect sdlDst = dst.toSDLRect();
	SDL_RenderCopyEx(renderer_.get(), spritesheet, &sdlSrc, &sdlDst, angle, center, flip);
}

// everything from here to EOF is Joshua's work
SDL_Texture *Engine::loadTexture(const std::string &path) const
{
	// Using own implementation because IMG_LoadTexture always returns NULL
	// https://www.reddit.com/r/cpp_questions/comments/u43q2d/sdl2_img_loadtexture_problem/
	// SDL_Texture *texture = IMG_LoadTexture(renderer_.get(), path.c_str());

	SDL_Texture *texture = NULL;
	SDL_Surface *surface = IMG_Load(path.c_str());
	if (surface) {
		texture = SDL_CreateTextureFromSurface(renderer_.get(), surface);
		// SDL_FreeSurface(surface);
	}
	if (!texture) {
		throw std::runtime_error("Error. Unable to open " + path);
	}

	return texture;
}

void Engine::drawText(const Recti &dst, const std::string &text) const
{
	SDL_Surface *textSurface = TTF_RenderUTF8_Solid_Wrapped(font_.get(), text.c_str(), {0, 0, 0, 255}, dst.w);

	if (!textSurface) {
		fprintf(stderr, "Error creating SDL textsurface %s\n", SDL_GetError());
	}
	SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer_.get(), textSurface);

	if (!textTexture) {
		fprintf(stderr, "Error creating SDL textTexture %s\n", SDL_GetError());
	}

	// Use the width of the textSurface here to avoid the text getting stretched horizontally
	// and multiply for better letter spacing
	SDL_Rect dstRect = {dst.x, dst.y, textSurface->w * 2, dst.h};

	// Test weather the text is too short to be wrapped onto two lines
	// If so, reduce the height of dstRect to avoid that the text is stretched vertically
	// Note: This is not used anymore but it might be useful to provide a method to check the size of a string
	int textW = 0;
	int textH = 0;
	TTF_SizeText(font_.get(), text.c_str(), &textW, &textH);
	if (textW < dst.w) {
		// dstRect.h = dstRect.h / 2;
	}

	SDL_RenderCopy(renderer_.get(), textTexture, nullptr, &dstRect);

	SDL_DestroyTexture(textTexture);

	// Using SDL_FreeSurface here causes a crash, but not freeing the surface creates a memory leak.
	// The crash however actually happens when TTF_RenderUTF8_Solid_Wrapped() is called, but only after
	// SDL_FreeSurface was called once.
	// So The crash happens on the very next iteration after the text was successfully drawn.
	// SDL_FreeSurface(textSurface);
}
