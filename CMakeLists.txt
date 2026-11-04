cmake_minimum_required(VERSION 3.12)

project(fashion_mnist VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(fashion_mnist
    src/main.cpp
    src/mlp.cpp
    src/dataset.cpp
    src/io.cpp
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(fashion_mnist PRIVATE -Wall -Wextra -O2)
elseif(MSVC)
    target_compile_options(fashion_mnist PRIVATE /W4 /O2)
endif()

find_package(Threads QUIET)
if(Threads_FOUND)
    target_link_libraries(fashion_mnist PRIVATE Threads::Threads)
endif()

install(TARGETS fashion_mnist RUNTIME DESTINATION bin)
