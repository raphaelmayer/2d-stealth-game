cmake_minimum_required(VERSION 3.14) # FetchContent_MakeAvailable requires 3.14

project(
        Tactical_Squad
        VERSION 1.0
        DESCRIPTION "A simple RPG in C++ configured with CMake, SDL2 and Catch2"
        LANGUAGES CXX)

# set any variables and settings here
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) 
set_property(GLOBAL PROPERTY USE_FOLDERS ON) # support folders in IDEs
# This allows us to include CMake modules located in `./cmake`.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# The next 3 lines are for cross-compatibility, specifically for Windows. Windows looks 
# for dll's "differently", so we put all build files in a single directory. 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# include(clang-format)
include(FetchContent)
include(CTest)

# add dependencies here
FetchContent_Declare(
    SDL2
    URL https://libsdl.org/release/SDL2-2.30.11.tar.gz
)

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
set(SDL_LIBRARIES ${SDL_LIBRARIES} SDL2main SDL2-static SDL2_image SDL2_ttf SDL2_mixer)

# Fetch BehaviorTree.CPP
FetchContent_Declare(
    BehaviorTreeCPP
    GIT_REPOSITORY https://github.com/BehaviorTree/BehaviorTree.CPP.git
    GIT_TAG 4.6.2 # Replace with desired version tag
)
# Configure optional features
set(BTCPP_GROOT_INTERFACE OFF CACHE BOOL "Disable ZeroMQ support" FORCE) # Disable if you don't need Groot integration
set(BTCPP_SQLITE_LOGGING OFF CACHE BOOL "Disable SQLite logging" FORCE) # Disable SQLite logging
set(BTCPP_EXAMPLES OFF CACHE BOOL "Disable examples" FORCE) # Optional
set(BTCPP_UNIT_TESTS OFF CACHE BOOL "Disable unit tests" FORCE) # Optional
# Make the library available
FetchContent_MakeAvailable(BehaviorTreeCPP)

FetchContent_Declare(
    EASYS
    GIT_REPOSITORY  https://github.com/raphaelmayer/easys
    GIT_TAG         master
    GIT_PROGRESS    TRUE
)
FetchContent_MakeAvailable(EASYS)

# add sub-projects here
add_subdirectory(src)
add_subdirectory(tests)