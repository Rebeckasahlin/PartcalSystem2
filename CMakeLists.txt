cmake_minimum_required(VERSION 3.21...3.25 FATAL_ERROR)

project(particlesystem LANGUAGES CXX)

enable_testing()
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER CMake)


khgjhjhkb
klklklklklklk
set(CMAKE_XCODE_SCHEME_ADDRESS_SANITIZER_USE_AFTER_RETURN ON)
set(CMAKE_XCODE_SCHEME_ADDRESS_SANITIZER ON)
set(CMAKE_XCODE_SCHEME_MAIN_THREAD_CHECKER_STOP ON)
set(CMAKE_XCODE_SCHEME_UNDEFINED_BEHAVIOUR_SANITIZER ON)
set(CMAKE_XCODE_SCHEME_UNDEFINED_BEHAVIOUR_SANITIZER_STOP ON)

add_library(project_warnings INTERFACE)
target_compile_options(project_warnings INTERFACE 
  $<$<CXX_COMPILER_ID:MSVC>:
    /W4                 # Enable the highest warning level
    /w44388             # eneble 'signed/unsigned mismatch' '(off by default)
    /we4715             # turn 'not all control paths return a value' into a compile error
    /permissive-        # Stick to the standard
    >
  $<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:
    -Wall               # Enable the highest warning level
    -Wextra 
    -Wconversion
    >
)

# Enable the Address Sanatizer, helps finding bugs at runtime
add_library(project_sanitize INTERFACE)
target_compile_options(project_sanitize INTERFACE 
  $<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-fsanitize=address>
)
target_link_options(project_sanitize INTERFACE 
  $<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-fsanitize=address>
)

# External libraries
find_package(Catch2 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(Tracy CONFIG REQUIRED)

# Rendering
set(RENDERING_HEADER_FILES include/rendering/window.h)
set(RENDERING_SOURCE_FILES src/rendering/window.cpp)
add_library(rendering ${RENDERING_HEADER_FILES} ${RENDERING_SOURCE_FILES})
add_library(rendering::rendering ALIAS rendering)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/rendering FILES ${RENDERING_HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/rendering FILES ${RENDERING_SOURCE_FILES})
target_include_directories(rendering PRIVATE "include")
target_link_libraries(rendering 
  PUBLIC 
    Tracy::TracyClient 
    glm::glm
    fmt::fmt 
    project_warnings
    project_sanitize
  PRIVATE 
    glad::glad 
    glfw
    imgui::imgui 
)

# Particle System Library
set(HEADER_FILES
  include/particlesystem/particlesystem.h
  # ADD HEADER FILES HERE
)
set(SOURCE_FILES
  src/particlesystem/particlesystem.cpp
  # ADD SOURCE FILES HERE
)

add_library(particlesystem ${SOURCE_FILES} ${HEADER_FILES})
add_library(particlesystem::particlesystem ALIAS particlesystem)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/particlesystem FILES ${HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/particlesystem FILES ${SOURCE_FILES})
target_include_directories(particlesystem PUBLIC "include")
target_link_libraries(particlesystem 
  PUBLIC 
    Tracy::TracyClient 
    project_warnings
    project_sanitize
)

# Unit tests
set(TEST_FILES
  unittest/vec2.cpp
  unittest/othertests.cpp
  # ADD TEST FILES HERE
)
add_executable(unittest ${TEST_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/unittest FILES ${TEST_FILES})
target_include_directories(unittest PUBLIC "include")
target_link_libraries(unittest 
  PUBLIC 
    Catch2::Catch2WithMain 
    particlesystem::particlesystem 
    project_warnings
    project_sanitize
)
add_test(NAME unittest COMMAND $<TARGET_FILE:unittest>)

# Application
set(APP_HEADER_FILES
  # ADD APP HEADER FILES HERE
)
set(APP_SOURCE_FILES
  src/application/main.cpp
  # ADD APP SOURCE FILES HERE
)

add_executable(application ${APP_SOURCE_FILES} ${APP_HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/application FILES ${APP_HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/application FILES ${APP_SOURCE_FILES})
target_include_directories(application PUBLIC "include")
target_link_libraries(application
  PRIVATE 
    particlesystem::particlesystem
    rendering::rendering
    Tracy::TracyClient 
    project_warnings
    project_sanitize
)

if(MSVC)
  set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT application)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang") 
  set_target_properties(application unittest PROPERTIES XCODE_GENERATE_SCHEME YES)
endif()


