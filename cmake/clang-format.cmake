file(GLOB_RECURSE ALL_SOURCE_FILES apps/*.cpp apps/*.hpp)
file(GLOB_RECURSE ALL_SOURCE_FILES ${ALL_SOURCE_FILES} include/*.hpp)
file(GLOB_RECURSE ALL_SOURCE_FILES ${ALL_SOURCE_FILES} src/*.cpp src/*.hpp)
file(GLOB_RECURSE ALL_SOURCE_FILES ${ALL_SOURCE_FILES} tests/*.cpp tests/*.hpp)

message(ALL_SOURCE_FILES=${ALL_SOURCE_FILES})

add_custom_target(
    clangformat
    ALL
    COMMAND clang-format
    -style=file
    -i
    --verbose
    ${ALL_SOURCE_FILES}
)