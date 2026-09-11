cmake_minimum_required(VERSION 3.12)

project(bulk VERSION 1.0.0 LANGUAGES CXX)

add_executable(bulk
    src/main.cpp
    src/bulk_parser.cpp
    src/console_writer.cpp
    src/file_writer.cpp
)

set_target_properties(bulk PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(bulk
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)



install(TARGETS bulk RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_NAME bulk)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)
