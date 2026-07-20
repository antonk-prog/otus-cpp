cmake_minimum_required(VERSION 3.12)

project(matrix VERSION 1.0.0 LANGUAGES CXX)

add_executable(matrix src/main.cpp)

set_target_properties(matrix PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(matrix
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)



install(TARGETS matrix RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_NAME matrix)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)
