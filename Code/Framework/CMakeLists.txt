cmake_minimum_required(VERSION 3.17)
project(JummySalesman)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
add_subdirectory(lib)
add_subdirectory(includes)

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 3b15fa82ea74739b574d705fea44959b58142eb8)
FetchContent_Declare(argparse GIT_REPOSITORY https://github.com/p-ranav/argparse.git)
FetchContent_Declare(json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG 9cca280a4d0ccf0c08f47a99aa71d1b0e52f8d03)
FetchContent_MakeAvailable(cpr)
FetchContent_MakeAvailable(json)
FetchContent_MakeAvailable(argparse)

target_link_libraries(JummySalesman PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(JummySalesman PRIVATE cpr::cpr)
target_link_libraries(JummySalesman PRIVATE argparse::argparse)