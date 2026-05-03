cmake_minimum_required(VERSION 3.12)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(helloworld VERSION ${PROJECT_VERSION})

option(WITH_BOOST_TEST "Whether to build Boost test" ON)
option(WITH_GOOGLE_TEST "Whether to build Google test" ON)

configure_file(version.h.in version.h)

add_executable(helloworld_cli main.cpp)
add_library(helloworld lib.cpp)

set_target_properties(helloworld_cli helloworld PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(helloworld
    PRIVATE "${CMAKE_BINARY_DIR}"
)

target_link_libraries(helloworld_cli PRIVATE
    helloworld
)

if(WITH_BOOST_TEST)
    find_package(Boost COMPONENTS unit_test_framework REQUIRED)
    add_executable(test_version_boost test_version_boost.cpp)

    set_target_properties(test_version_boost PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
    )

    set_target_properties(test_version_boost PROPERTIES
        COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
        INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
    )

    target_link_libraries(test_version_boost
        ${Boost_LIBRARIES}
        helloworld
    )
endif()

if(WITH_GOOGLE_TEST)
    find_package(GTest QUIET)

    if(NOT TARGET GTest::gtest)
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third-party/googletest/CMakeLists.txt")
            add_subdirectory(third-party/googletest EXCLUDE_FROM_ALL)
        else()
            message(FATAL_ERROR "GoogleTest not found: neither system package nor third-party/googletest is available")
        endif()
    endif()

    add_executable(test_version_google test_version_google.cpp)

    set_target_properties(test_version_google PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
    )

    target_link_libraries(test_version_google PRIVATE
        GTest::gtest
        GTest::gtest_main
        helloworld
    )
endif()

if (MSVC)
    target_compile_options(helloworld_cli PRIVATE
        /W4
    )
    target_compile_options(helloworld PRIVATE
        /W4
    )
    if(WITH_BOOST_TEST)
        target_compile_options(test_version_boost PRIVATE
            /W4
        )
    endif()
    if(WITH_GOOGLE_TEST)
        target_compile_options(test_version_google PRIVATE
            /W4
        )
    endif()
else ()
    target_compile_options(helloworld_cli PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(helloworld PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    if(WITH_BOOST_TEST)
        target_compile_options(test_version_boost PRIVATE
            -Wall -Wextra -pedantic -Werror
        )
    endif()
    if(WITH_GOOGLE_TEST)
        target_compile_options(test_version_google PRIVATE
            -Wall -Wextra -pedantic -Werror
        )
    endif()
endif()

install(TARGETS helloworld_cli RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)

if(WITH_BOOST_TEST)
    enable_testing()
    add_test(test_version_boost test_version_boost)
endif()

if(WITH_GOOGLE_TEST)
    enable_testing()
    add_test(test_version_google test_version_google)
endif()
