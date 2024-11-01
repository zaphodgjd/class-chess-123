cmake_minimum_required(VERSION 3.2.0)
project(tictactoe VERSION 0.1.0 LANGUAGES C CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)

# Check for the operating system and compiler
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOS TRUE)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX TRUE)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(WINDOWS TRUE)
endif()

# Set compiler-specific flags for g++ and MinGW
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(LINUX OR WINDOWS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wno-ignored-qualifiers -Wno-cast-function-type -Wno-unused-variable -Wno-missing-field-initializers -Wno-overloaded-virtual")
    endif()
endif()

# Find OpenGL and other dependencies
find_package(OpenGL REQUIRED)

# Include GLFW for macOS and Linux, or other necessary libraries
if(MACOS OR LINUX)
    find_package(glfw3 REQUIRED)
    include_directories(${GLFW_INCLUDE_DIRS})
endif()

# Define the platform-specific source files
if(MACOS)
    set(MAIN_FILE "main_macos.cpp")
    set(IMPL_FILE "imgui/imgui_impl_glfw.cpp")
elseif(WINDOWS)
    set(MAIN_FILE "main_win32.cpp")
    set(IMPL_FILE "imgui/imgui_impl_win32.cpp")
else() # Linux
    set(MAIN_FILE "main_macos.cpp")
    set(IMPL_FILE "imgui/imgui_impl_glfw.cpp")
endif()

# Define the executable and sources
add_executable(tictactoe Application.cpp
                          imgui/imgui_demo.cpp
                          imgui/imgui_draw.cpp
                          imgui/imgui_tables.cpp
                          imgui/imgui_widgets.cpp
                          imgui/imgui.cpp
                          imgui/imgui_impl_opengl3.cpp
                          classes/Bit.cpp
                          classes/BitHolder.cpp
                          classes/Game.cpp
                          classes/Sprite.cpp
                          classes/Square.cpp
                          classes/ChessSquare.cpp
                          classes/Chess.cpp
                          ${MAIN_FILE}
                          ${IMPL_FILE}
                )

# Link libraries based on the platform
if(MACOS OR LINUX)
    target_link_libraries(tictactoe ${OPENGL_gl_LIBRARY} glfw)
elseif(WINDOWS)
    target_link_libraries(tictactoe ${OPENGL_gl_LIBRARY})
    if(MINGW)
        target_link_libraries(tictactoe dwmapi)
    endif()
endif()

# Set the project for packaging
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)