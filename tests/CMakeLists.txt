# Testing library
FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.13.6)
FetchContent_MakeAvailable(catch)

add_executable(Tactical_Squad_Tests main.test.cpp
	# add other source files here
)

include_directories(${CMAKE_SOURCE_DIR}/include)

target_compile_features(Tactical_Squad_Tests PRIVATE cxx_std_20)
target_link_libraries(Tactical_Squad_Tests PUBLIC Catch2::Catch2)

add_test(NAME test COMMAND Tactical_Squad_Tests) # Command can be a target
