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
        src/FrameCounter.cpp
        src/FrameCounter.h
        src/Dashboard.cpp
        src/Dashboard.h
        src/Coin.cpp
        src/Coin.h
        src/SoundSystem.cpp
        src/SoundSystem.h
        src/sim/Item.cpp
        src/sim/Item.h
        src/sim/Modifier.cpp
        src/sim/Modifier.h
        src/sim/ScamInterface.h
        src/sim/Simulation.cpp
        src/sim/Simulation.h
        src/Texture.cpp
        src/Texture.h
        src/sim/Event.cpp
        src/sim/Event.h
)

add_executable(Scam WIN32 ${SCAM_SRC})

set_property(TARGET Scam PROPERTY CXX_STANDARD 20)

# OPENGL
find_package(OpenGL REQUIRED)

# SPDLOG
set(SPDLOG_USE_STD_FORMAT ON)
set(SPDLOG_BUILD_EXAMPLE OFF)
add_subdirectory(thirdparty/spdlog EXCLUDE_FROM_ALL)

# GLAD
set(BUILD_SHARED_LIBS OFF)
add_library(glad thirdparty/glad/src/glad.c)
target_include_directories(glad PUBLIC thirdparty/glad/include)
set(BUILD_SHARED_LIBS ON)

# SDL
add_subdirectory(thirdparty/SDL EXCLUDE_FROM_ALL)

# SDL_mixer
set(SDLMIXER_VENDORED OFF)
add_subdirectory(thirdparty/SDL_mixer EXCLUDE_FROM_ALL)

# IMGUI
add_library(imgui
        STATIC
        thirdparty/imgui-docking/imgui.cpp
        thirdparty/imgui-docking/imgui_demo.cpp
        thirdparty/imgui-docking/imgui_draw.cpp
        thirdparty/imgui-docking/imgui_tables.cpp
        thirdparty/imgui-docking/imgui_widgets.cpp
        thirdparty/imgui-docking/backends/imgui_impl_opengl3.cpp
        thirdparty/imgui-docking/backends/imgui_impl_sdl3.cpp
        thirdparty/imgui-docking/misc/cpp/imgui_stdlib.cpp
)

target_link_libraries(imgui SDL3::SDL3)
target_include_directories(imgui PUBLIC thirdparty/imgui-docking)
target_include_directories(imgui PUBLIC thirdparty/imgui-docking/backends)
target_include_directories(imgui PUBLIC thirdparty/imgui-docking/misc/cpp)

# IMPLOT
add_library(implot STATIC
        thirdparty/implot/implot.cpp
        thirdparty/implot/implot_demo.cpp
        thirdparty/implot/implot_items.cpp
)
target_link_libraries(implot imgui)
target_include_directories(implot PUBLIC thirdparty/implot)

# STB
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE thirdparty/stb)

target_link_libraries(Scam
        PRIVATE
        ${OPENGL_LIBRARIES}
        spdlog::spdlog
        glad
        imgui
        implot
        SDL3::SDL3
        SDL3_mixer::SDL3_mixer
        stb
)

add_custom_target(data_files ALL
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data
        COMMENT "Copying data files to output folder"
)

add_dependencies(Scam data_files)

if (MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Scam)

    add_custom_command(TARGET Scam POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:Scam> $<TARGET_RUNTIME_DLLS:Scam>
            COMMAND_EXPAND_LISTS
    )
endif ()

