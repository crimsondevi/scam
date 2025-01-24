cmake_minimum_required(VERSION 3.5)

project(Scam)

set(CMAKE_OPTIMIZE_DEPENDENCIES ON)

if (MSVC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi /Gm- /MP /GF")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_WIN32_WINDOWS /D_WINSOCK_DEPRECATED_NO_WARNINGS")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup /DEBUG:FASTLINK /incremental")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /FUNCTIONPADMIN /OPT:NOREF /OPT:NOICF /DEBUG:FULL")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE")
    set_source_files_properties(${SCAM_SRC} PROPERTIES COMPILE_FLAGS "/WX")
endif ()

set(SCAM_SRC
        src/Main.cpp
        src/Gameloop.h
        src/Gameloop.cpp
        src/Prelude.h
)

add_executable(Scam WIN32 ${SCAM_SRC})

set_property(TARGET Scam PROPERTY CXX_STANDARD 20)

# SPDLOG
set(SPDLOG_USE_STD_FORMAT ON)
set(SPDLOG_BUILD_EXAMPLE OFF)
add_subdirectory(thirdparty/spdlog EXCLUDE_FROM_ALL)

# GLAD
set(BUILD_SHARED_LIBS OFF)
add_library(glad thirdparty/glad/src/glad.c)
target_include_directories(glad PUBLIC thirdparty/glad/include)
set(BUILD_SHARED_LIBS ON)

# IMGUI
add_library(imgui
        STATIC
        thirdparty/imgui/imgui.cpp
        thirdparty/imgui/imgui_demo.cpp
        thirdparty/imgui/imgui_draw.cpp
        thirdparty/imgui/imgui_tables.cpp
        thirdparty/imgui/imgui_widgets.cpp
        thirdparty/imgui/backends/imgui_impl_opengl3.cpp
        thirdparty/imgui/backends/imgui_impl_sdl3.cpp
        thirdparty/imgui/misc/cpp/imgui_stdlib.cpp
)

#SDL
add_subdirectory(thirdparty/SDL EXCLUDE_FROM_ALL)

target_link_libraries(imgui SDL3::SDL3)
target_include_directories(imgui PUBLIC thirdparty/imgui)
target_include_directories(imgui PUBLIC thirdparty/imgui/backends)
target_include_directories(imgui PUBLIC thirdparty/imgui/misc/cpp)
target_include_directories(imgui PUBLIC ${SDL2_INCLUDE_DIRS})

find_package(OpenGL REQUIRED)

target_link_libraries(Scam
        PRIVATE
        ${OPENGL_LIBRARIES}
        spdlog::spdlog
        glad
        imgui
        SDL3::SDL3
)

if (MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Scam)

    add_custom_command(TARGET Scam POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:Scam> $<TARGET_RUNTIME_DLLS:Scam>
            COMMAND_EXPAND_LISTS
    )
endif ()

