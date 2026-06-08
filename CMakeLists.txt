cmake_minimum_required(VERSION 3.12)

project(custom_allocator VERSION 1.0.0 LANGUAGES CXX)

add_executable(app src/main.cpp)

add_library(auxil src/auxil.cpp)

include_directories(
    ${PROJECT_NAME}
    PRIVATE
    allocator
)

set_target_properties(app auxil PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(app PRIVATE 
    src
    src/allocator
    src/container
)

target_include_directories(auxil PRIVATE 
    src
)

target_link_libraries(
    app
    PRIVATE
    auxil
)



install(TARGETS app RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)
