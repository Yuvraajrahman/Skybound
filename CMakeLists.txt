cmake_minimum_required(VERSION 3.16)

project(SkyBound
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "SkyBound — cross-platform 2D platformer built with raylib"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SKYBOUND_USE_SYSTEM_RAYLIB "Link against a system-installed raylib" OFF)

if (SKYBOUND_USE_SYSTEM_RAYLIB)
    find_package(raylib 5.0 REQUIRED)
else()
    include(FetchContent)
    set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED)
        FetchContent_Populate(raylib)

        set(_raylib_cmakelists "${raylib_SOURCE_DIR}/CMakeLists.txt")
        if (EXISTS "${_raylib_cmakelists}")
            file(READ "${_raylib_cmakelists}" _raylib_cmake_contents)
            string(REPLACE "cmake_minimum_required(VERSION 3.0)" "cmake_minimum_required(VERSION 3.5)" _raylib_cmake_updated "${_raylib_cmake_contents}")
            if (NOT _raylib_cmake_contents STREQUAL _raylib_cmake_updated)
                file(WRITE "${_raylib_cmakelists}" "${_raylib_cmake_updated}")
            endif()
        endif()

        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()

file(GLOB SKYBOUND_SOURCES CONFIGURE_DEPENDS
    "src/*.cpp"
)

add_executable(SkyBound ${SKYBOUND_SOURCES})

target_include_directories(SkyBound PRIVATE src)

target_link_libraries(SkyBound PRIVATE raylib)

if (MSVC)
    target_compile_options(SkyBound PRIVATE /W4 /permissive-)
else()
    target_compile_options(SkyBound PRIVATE -Wall -Wextra -Wpedantic)
endif()

if (MINGW)
    target_link_libraries(SkyBound PRIVATE winmm)
endif()

if (APPLE)
    target_link_libraries(SkyBound PRIVATE
        "-framework IOKit"
        "-framework Cocoa"
        "-framework OpenGL"
    )
endif()

install(TARGETS SkyBound RUNTIME DESTINATION bin)

include(GNUInstallDirs)
install(DIRECTORY assets/ DESTINATION ${CMAKE_INSTALL_DATADIR}/SkyBound/assets)


