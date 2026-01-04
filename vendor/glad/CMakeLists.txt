include(FetchContent)

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v0.1.33
    SOURCE_SUBDIR cmake
    CMAKE_ARGS
    -DCMAKE_POLICY_VERSION_MINIMUM=4.0
)

message("Fetching GLAD...")
FetchContent_MakeAvailable(glad)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
add_subdirectory(${glad_SOURCE_DIR} ${glad_BINARY_DIR})
