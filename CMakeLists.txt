cmake_minimum_required(VERSION 3.12)

project(join_server VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

add_executable(join_server
    src/main.cpp
    src/server.cpp
    src/session.cpp
    src/database.cpp
    src/operations.cpp
)

target_include_directories(join_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(join_server PRIVATE Boost::boost Threads::Threads)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(join_server PRIVATE -Wall -Wextra)
elseif(MSVC)
    target_compile_options(join_server PRIVATE /W4)
endif()

install(TARGETS join_server RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_NAME join_server)
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)
