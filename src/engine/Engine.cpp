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
	SDL_RenderSetScale(renderer_.get(), static_cast<float>(pixelSize_.x), static_cast<float>(pixelSize_.y));

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
// TODO:
// should this even exist? (since window size is derived from screen size). Should setScreenSize exist?.
// We definitely need to adjust all functions here to set all related values when setting a single value.
// A resizeWindow implementation is also necessary and might just be the best first step to figuring this out\
// and it might just be enough. No need for those setters.
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
	windowSize_ = screenSize * pixelSize_;
}
void Engine::setRenderScale(Vec2i pixelSize)
{
	pixelSize_ = pixelSize;
	SDL_RenderSetScale(renderer_.get(), static_cast<float>(pixelSize_.x), static_cast<float>(pixelSize_.y));
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

void Engine::fillRectangle(const Recti &rect, const ColorRGBA &color) const
{
	SDL_Rect r = rect.toSDLRect();
	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer_.get(), &r);
}

void Engine::fillRectangle(const Rectf &rect, const ColorRGBA &color) const
{
	SDL_FRect r = rect.toSDLRect();
	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
	SDL_RenderFillRectF(renderer_.get(), &r);
}

void Engine::drawRectangle(const Recti &rect, const ColorRGBA &color) const
{
	SDL_Rect r = rect.toSDLRect();
	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
	SDL_RenderDrawRect(renderer_.get(), &r);
}

void Engine::drawRectangle(const Rectf &rect, const ColorRGBA &color) const
{
	SDL_FRect r = rect.toSDLRect();
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

void Engine::drawTexture(const Texture &texture) const
{
	SDL_RenderCopy(renderer_.get(), texture.getSDLTexture(), nullptr, nullptr);
}

void Engine::drawTexture(const Texture &texture, const Recti &dst) const
{
	SDL_Rect sdlDst = dst.toSDLRect();
	SDL_RenderCopy(renderer_.get(), texture.getSDLTexture(), nullptr, &sdlDst);
}

void Engine::drawTexture(const Texture &texture, const Rectf &dst) const
{
	SDL_FRect sdlDst = dst.toSDLRect();
	SDL_RenderCopyF(renderer_.get(), texture.getSDLTexture(), nullptr, &sdlDst);
}

void Engine::drawTexture(const Texture &texture, const Recti &src, const Recti &dst) const
{
	SDL_Rect sdlSrc = src.toSDLRect();
	SDL_Rect sdlDst = dst.toSDLRect();
	SDL_RenderCopy(renderer_.get(), texture.getSDLTexture(), &sdlSrc, &sdlDst);
}

void Engine::drawTexture(const Texture &texture, const Recti &src, const Rectf &dst) const
{
	SDL_Rect sdlSrc = src.toSDLRect();
	SDL_FRect sdlDst = dst.toSDLRect();
	SDL_RenderCopyF(renderer_.get(), texture.getSDLTexture(), &sdlSrc, &sdlDst);
}

void Engine::drawTexture(const Texture &texture, const Recti &src, const Recti &dst, const double &angle,
                         const Vec2i &center, const TextureFlip &flip) const
{
	SDL_Rect sdlSrc = src.toSDLRect();
	SDL_Rect sdlDst = dst.toSDLRect();
	SDL_Point sdlCenter = {center.x, center.y};
	SDL_RenderCopyEx(renderer_.get(), texture.getSDLTexture(), &sdlSrc, &sdlDst, angle, &sdlCenter,
	                 static_cast<SDL_RendererFlip>(flip));
}

void Engine::drawTexture(const Texture &texture, const Recti &src, const Rectf &dst, const double &angle,
                         const Vec2f &center, const TextureFlip &flip) const
{
	SDL_Rect sdlSrc = src.toSDLRect();
	SDL_FRect sdlDst = dst.toSDLRect();
	SDL_FPoint sdlCenter = {center.x, center.y};
	SDL_RenderCopyExF(renderer_.get(), texture.getSDLTexture(), &sdlSrc, &sdlDst, angle, &sdlCenter,
	                  static_cast<SDL_RendererFlip>(flip));
}

SDL_Texture *Engine::loadSDLTexture(const std::string &path) const
{
	SDL_Surface *surface = IMG_Load(path.c_str());
	if (!surface) {
		throw std::runtime_error(std::string("Error loading image at '") + path + "': " + IMG_GetError());
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer_.get(), surface);
	SDL_FreeSurface(surface);

	if (!texture) {
		throw std::runtime_error(std::string("Error creating texture from surface for '") + path
		                         + "': " + SDL_GetError());
	}

	return texture;
}

Texture Engine::loadTexture(const std::string &path) const
{
	Texture t = Texture(loadSDLTexture(path));
	return t;
}

void Engine::drawText(const Recti &dst, const std::string &text) const
{
	const Rectf floatDst = Rectf{static_cast<float>(dst.x), static_cast<float>(dst.y), static_cast<float>(dst.w),
	                             static_cast<float>(dst.h)};
	drawText(floatDst, text);
}

void Engine::drawText(const Rectf &dst, const std::string &text) const
{
	// OPTIONAL: Set the font style or outline
	// TTF_SetFontStyle(font_.get(), TTF_STYLE_BOLD);
	// TTF_SetFontOutline(font_.get(), 1);

	SDL_Color textColor = {0, 0, 0, 255};

	SDL_Surface *textSurface = TTF_RenderUTF8_Solid_Wrapped(font_.get(), text.c_str(), textColor, dst.w);
	if (!textSurface) {
		throw std::runtime_error(std::string("Error creating text surface: ") + SDL_GetError());
	}

	SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer_.get(), textSurface);
	if (!textTexture) {
		SDL_FreeSurface(textSurface);
		throw std::runtime_error(std::string("Error creating text texture: ") + SDL_GetError());
	}

	int textW = 0;
	int textH = 0;
	if (TTF_SizeText(font_.get(), text.c_str(), &textW, &textH) != 0) {
		SDL_DestroyTexture(textTexture);
		SDL_FreeSurface(textSurface);
		throw std::runtime_error(std::string("Error measuring text size: ") + SDL_GetError());
	}

	float verticalScale = 1.0f;
	if (textH > 0) {
		verticalScale = static_cast<float>(dst.h) / static_cast<float>(textH);
	}

	const float horizontalSpacingFactor = 1.5f;

	SDL_FRect dstRect;
	dstRect.x = dst.x;
	dstRect.y = dst.y;
	dstRect.h = dst.h;
	dstRect.w = textW * verticalScale * horizontalSpacingFactor;

	if (SDL_RenderCopyF(renderer_.get(), textTexture, nullptr, &dstRect) != 0) {
		SDL_DestroyTexture(textTexture);
		SDL_FreeSurface(textSurface);
		throw std::runtime_error(std::string("Error rendering text: ") + SDL_GetError());
	}

	SDL_DestroyTexture(textTexture);
	SDL_FreeSurface(textSurface);
}

// We might want to allow more fine-grained control over blending in the future.
void Engine::enableAlphaBlending()
{
	SDL_SetRenderDrawBlendMode(renderer_.get(), SDL_BLENDMODE_BLEND);
}
void Engine::disableAlphaBlending()
{
	SDL_SetRenderDrawBlendMode(renderer_.get(), SDL_BLENDMODE_NONE);
}