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

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_NAME fashion_mnist)
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)
