include(FetchContent) # Needed for SDL

# Engine dependencies
FetchContent_Declare(
    SDL2
    URL https://libsdl.org/release/SDL2-2.30.11.tar.gz
)

# These options prevent SDL's components from trying to install themselves globally.
set(SDL2TTF_INSTALL OFF)
set(SDL2IMAGE_INSTALL OFF)
set(SDL2MIXER_INSTALL OFF)

set(BUILD_SHARED_LIBS FALSE)

FetchContent_Declare(
    SDL2_IMAGE
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL_image
    GIT_TAG         release-2.6.3
    GIT_PROGRESS    TRUE
)
FetchContent_Declare(
    SDL2_TTF
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL_TTF
    GIT_TAG         release-2.20.2
    GIT_PROGRESS    TRUE
)
FetchContent_Declare(
    SDL2_MIXER
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL_mixer
    GIT_TAG         release-2.6.3
    GIT_PROGRESS    TRUE
)

FetchContent_MakeAvailable(SDL2 SDL2_IMAGE SDL2_TTF SDL2_MIXER)

add_library(engine
    Engine.cpp
    audio/Audio.cpp
    input/Input.cpp
)

# PUBLIC: Makes engine's own headers findable by the main project if it includes them (e.g., #include "Engine.h")
target_include_directories(engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link SDL libraries PUBLICLY. 
# TODO: This really should not be public, but we run into the issue that sdl is only available within cpp files. 
# This is nice while we migrate sdl completely into the engine but should be addressed at some point, since it now leaks sdl to the engine user. 
target_link_libraries(engine PUBLIC
    SDL2::SDL2      # Main SDL2 library (static due to BUILD_SHARED_LIBS=FALSE)
    SDL2::SDL2main
    SDL2_image
    SDL2_ttf
    SDL2_mixer
)