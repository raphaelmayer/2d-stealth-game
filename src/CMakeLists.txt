# Copy assets directory to the output directory
file(COPY ${CMAKE_SOURCE_DIR}/assets
     DESTINATION ${CMAKE_BINARY_DIR})

# all asset files that might change regularly, should be added via configure_file,
# so changes are tracked and changed files are automatically copied to the output directory.
configure_file(
    ${CMAKE_SOURCE_DIR}/assets/default_game.json
    ${CMAKE_BINARY_DIR}/assets/default_game.json
    COPYONLY
)
configure_file(
    ${CMAKE_SOURCE_DIR}/assets/default_game.json
    ${CMAKE_BINARY_DIR}/bin/savefile.json
    COPYONLY
)
configure_file(
    ${CMAKE_SOURCE_DIR}/assets/hero-spritesheet.png
    ${CMAKE_BINARY_DIR}/assets/hero-spritesheet.png
    COPYONLY
)
configure_file(
    ${CMAKE_SOURCE_DIR}/assets/ai/trees/DetectTree.xml
    ${CMAKE_BINARY_DIR}/assets/ai/trees/DetectTree.xml
    COPYONLY
)
configure_file(
    ${CMAKE_SOURCE_DIR}/assets/ai/trees/EngageTree.xml
    ${CMAKE_BINARY_DIR}/assets/ai/trees/EngageTree.xml
    COPYONLY
)
configure_file(
    ${CMAKE_SOURCE_DIR}/assets/ai/trees/IdleTree.xml
    ${CMAKE_BINARY_DIR}/assets/ai/trees/IdleTree.xml
    COPYONLY
)
configure_file(
    ${CMAKE_SOURCE_DIR}/assets/ai/trees/MainTree.xml
    ${CMAKE_BINARY_DIR}/assets/ai/trees/MainTree.xml
    COPYONLY
)

add_subdirectory(engine)

# Add cpp files here
add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE engine)

include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/include)


target_link_libraries(${PROJECT_NAME} PRIVATE BT::behaviortree_cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE easys)

# Enable warnings if supported by the compiler
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)