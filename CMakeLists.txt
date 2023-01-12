cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(TNM094)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER CMake)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

add_library(project_warnings INTERFACE)
if (WIN32)
  target_compile_options(project_warnings INTERFACE "/W4")
else ()
  target_compile_options(project_warnings INTERFACE "-Wall")
endif ()


###
# External libraries
###
#
# Tracy
#
option(TRACY_ENABLE "Enable Tracy Profiler")
add_library(tracy
  ext/tracy/TracyClient.cpp
)
target_include_directories(tracy PUBLIC ext/tracy)
if (WIN32)
  target_compile_options(tracy PRIVATE "/W0")
else ()
  target_compile_options(tracy PRIVATE "-w")
endif ()
target_link_libraries(tracy PRIVATE project_options)
if (TRACY_ENABLE)
  target_compile_definitions(tracy PUBLIC "TRACY_ENABLE")
endif ()
set_property(TARGET tracy PROPERTY FOLDER "External")

#
# Catch 2 for unit testing
#
add_library(catch2 INTERFACE)
target_include_directories(catch2 INTERFACE ext/catch2)

#
# GLFW
#
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(ext/glfw)
set_property(TARGET glfw PROPERTY FOLDER "External")

#
# ImGUI
#
add_library(imgui
  ext/imgui/imgui.cpp
  ext/imgui/imgui_demo.cpp
  ext/imgui/imgui_draw.cpp
  ext/imgui/imgui_widgets.cpp
  ext/imgui/imgui_impl_glfw.cpp
  ext/imgui/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC ext/imgui)
target_link_libraries(imgui PUBLIC glfw PRIVATE glad)
target_compile_definitions(imgui PRIVATE "IMGUI_IMPL_OPENGL_LOADER_GLAD")
set_property(TARGET imgui PROPERTY FOLDER "External")

#
# GLAD
#
add_library(glad ext/glad/src/glad.c)
target_include_directories(glad PUBLIC ext/glad/include)
set_property(TARGET glad PROPERTY FOLDER "External")

###
# Rendering
###
add_library(rendering include/rendering/window.h src/rendering/window.cpp)
target_include_directories(rendering PRIVATE "include")
target_link_libraries(rendering PUBLIC tracy PRIVATE glad glfw imgui project_options project_warnings)

###
# Particle System Library
###

# ADD HEADER FILES HERE
set(HEADER_FILES
  include/particlesystem/particlesystem.h
)

# ADD SOURCE FILES HERE
set(SOURCE_FILES
  src/particlesystem/particlesystem.cpp
)

add_library(particlesystem
  ${SOURCE_FILES}
  ${HEADER_FILES}
)

source_group("Header Files" FILES ${HEADER_FILES})
target_include_directories(particlesystem PRIVATE "include")
target_link_libraries(particlesystem PUBLIC tracy PRIVATE project_options project_warnings)

###
# Unit tests
###
add_executable(unittest
  unittest/main.cpp
  unittest/othertests.cpp
  unittest/vec2.cpp
)
target_include_directories(unittest PRIVATE "include")
target_link_libraries(unittest PUBLIC catch2 PRIVATE particlesystem project_options project_warnings)

###
# Application
###
add_executable(application
  src/main.cpp
)

target_include_directories(application PRIVATE "include")
target_link_libraries(application PUBLIC tracy PRIVATE particlesystem rendering project_options project_warnings)

if(MSVC)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT application)
endif()


if (EXISTS "${PROJECT_SOURCE_DIR}/solution")
  add_executable(solution
    solution/main.cpp
    solution/particlesystem.cpp
    src/util/rendering.cpp
  )
  target_include_directories(solution PRIVATE "include" "solution")
  target_link_libraries(solution PUBLIC tracy PRIVATE glad glfw imgui project_options project_warnings)
endif ()
