# ## -----------------------------------------------------------------------
# ##
# ## If you want to learn CMake:
# ## https://julesfouchy.github.io/Learn--Clean-Code-With-Cpp/lessons/cmake/
# ##
# ## If you want to see how to add libraries to your project:
# ## https://julesfouchy.github.io/Learn--Clean-Code-With-Cpp/lessons/use-libraries/#how-to
# ##
# ## -----------------------------------------------------------------------

cmake_minimum_required(VERSION 3.20)

include(FetchContent)

# You can set the name of your project here
project(ImGuiTemplate)
add_executable(${PROJECT_NAME})

# Choose your C++ version
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# Enable many good warnings.
# /WX and -Werror enable "warnings as errors": This means that your code won't compile if you have any warning.
# This forces you to take warnings into account, which is a good practice because warnings are here for a reason and can save you from a lot of bugs!
# If this is too strict for you, you can remove /WX and -Werror.
if(MSVC)
# target_compile_options(${PROJECT_NAME} PRIVATE /WX /W4)
else()
    # target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra -Wpedantic -pedantic-errors -Wimplicit-fallthrough)
endif()

# Set the folder where the executable is created
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

# Prevents compiler-specific extensions to C++ because they might allow code to compile on your machine but not on other people's machine
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_EXTENSIONS OFF)

# Add all the source files
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS src/*)

target_sources(${PROJECT_NAME} PRIVATE ${MY_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src)

FetchContent_Declare(
    tinygltf
    GIT_REPOSITORY https://github.com/syoyo/tinygltf.git
    GIT_TAG v2.9.5
)
FetchContent_MakeAvailable(tinygltf)
target_include_directories(${PROJECT_NAME} PRIVATE ${tinygltf_SOURCE_DIR})

# Keep GLSL files visible in IDE and available at runtime.
file(GLOB SHADER_SOURCES CONFIGURE_DEPENDS src/Render/3D/shaders/*.glsl)
target_sources(${PROJECT_NAME} PRIVATE ${SHADER_SOURCES})

target_compile_definitions(${PROJECT_NAME} PRIVATE
    SHADER_DIR="${CMAKE_SOURCE_DIR}/src/Render/3D/shaders"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${SHADER_SOURCES}
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets/models"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/models"
)

# Add quick-imgui library
add_subdirectory(lib/quick_imgui)
target_link_libraries(${PROJECT_NAME} PRIVATE quick_imgui::quick_imgui)