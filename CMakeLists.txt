cmake_minimum_required(VERSION 3.20.0)
project(BabylonNativeExample VERSION 0.1.0)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_STANDARD} /W3 /Zc:__cplusplus /Zc:preprocessor")
endif()

add_subdirectory(Dependencies)

add_executable(BabylonNativeExample main.cpp)

target_link_libraries(BabylonNativeExample 
    PRIVATE AppRuntime
    PRIVATE GraphicsDevice
    PRIVATE NativeEngine
    PRIVATE NativeInput
    PRIVATE NativeOptimizations
    PRIVATE Console
    PRIVATE Window
    PRIVATE ScriptLoader
    PRIVATE XMLHttpRequest
    PRIVATE Canvas
    PRIVATE glfw
    PRIVATE imgui
    ${ADDITIONAL_LIBRARIES}
    ${BABYLON_NATIVE_PLAYGROUND_EXTENSION_LIBRARIES})

target_compile_features(BabylonNativeExample PRIVATE cxx_std_17)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/node_modules/babylonjs/babylon.max.js")
    set(BABYLON_SCRIPTS
        "node_modules/babylonjs-loaders/babylonjs.loaders.js"
        "node_modules/babylonjs/babylon.max.js"
        "node_modules/babylonjs/babylon.js.map"
        "node_modules/babylonjs-materials/babylonjs.materials.js"
        "node_modules/babylonjs-gui/babylon.gui.js")

    set(SCRIPTS
        "Scripts/game.js"
        "Scripts/ammo.js"
        "Scripts/meshwriter.min.js"
        "Scripts/recast.js")

    target_sources(BabylonNativeExample PRIVATE ${BABYLON_SCRIPTS} ${SCRIPTS})

    foreach(SCRIPT ${BABYLON_SCRIPTS} ${SCRIPTS})
        get_filename_component(SCRIPT_NAME "${SCRIPT}" NAME)
        add_custom_command(
            OUTPUT "${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
            COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
            COMMENT "Copying ${SCRIPT_NAME}"
            MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}")
    endforeach()
    
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${BABYLON_SCRIPTS} ${SCRIPTS})

else()
    message(FATAL_ERROR "Babylon.js package is required. Please run npm install on this folder.")
endif()

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT BabylonNativeExample)

if(MSVC)
    target_compile_definitions(BabylonNativeExample PRIVATE TARGET_PLATFORM_WINDOWS)
elseif(APPLE)
    target_compile_definitions(BabylonNativeExample PRIVATE TARGET_PLATFORM_OSX)
    find_library(JSCORE_LIBRARY JavaScriptCore)
    target_link_libraries(BabylonNativeExample PRIVATE ${JSCORE_LIBRARY})
    target_link_libraries(BabylonNativeExample PRIVATE "-framework Cocoa" "-framework MetalKit" "-framework QuartzCore" "-framework CoreFoundation" "-framework CoreGraphics")
else()
    target_compile_definitions(BabylonNativeExample PRIVATE TARGET_PLATFORM_LINUX)
endif()