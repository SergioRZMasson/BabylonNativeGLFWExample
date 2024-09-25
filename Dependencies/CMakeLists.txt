set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
set(GLFW_LIBRARY_TYPE "STATIC")
add_subdirectory(glfw)

set_property(TARGET glfw PROPERTY FOLDER Dependencies/glfw)
set_property(TARGET update_mappings PROPERTY FOLDER Dependencies/glfw)

set(BABYLON_NATIVE_BUILD_APPS OFF)
# We need a very big Vertex buffer to update all our instances data.
add_compile_definitions(BGFX_CONFIG_TRANSIENT_VERTEX_BUFFER_SIZE=160000000)

add_subdirectory(BabylonNative)

# Babylon Native Core
set_property(TARGET AppRuntime PROPERTY FOLDER Dependencies/BabylonNative/Core/)
set_property(TARGET Graphics PROPERTY FOLDER Dependencies/BabylonNative/Core/)
set_property(TARGET JsRuntime PROPERTY FOLDER Dependencies/BabylonNative/Core/)
set_property(TARGET ScriptLoader PROPERTY FOLDER Dependencies/BabylonNative/Core/)
set_property(TARGET Foundation PROPERTY FOLDER Dependencies/BabylonNative/Core/)

# Babylon Native Plugins
if(BABYLON_NATIVE_PLUGIN_CHROMEDEVTOOLS)
    set_property(TARGET ChromeDevTools PROPERTY FOLDER Dependencies/BabylonNative/Plugins/)
endif()

if(BABYLON_NATIVE_PLUGIN_EXTERNALTEXTURE)
    set_property(TARGET ExternalTexture PROPERTY FOLDER Dependencies/BabylonNative/Plugins/)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVECAMERA)
    set_property(TARGET NativeCamera PROPERTY FOLDER Dependencies/BabylonNative/Plugins/)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVECAPTURE)
    set_property(TARGET NativeCapture PROPERTY FOLDER Dependencies/BabylonNative/Plugins/)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVEENGINE)
    set_property(TARGET NativeEngine PROPERTY FOLDER Dependencies/BabylonNative/Plugins/)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVEINPUT)
    set_property(TARGET NativeInput PROPERTY FOLDER Dependencies/BabylonNative/Plugins/)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVEOPTIMIZATIONS)
    set_property(TARGET NativeOptimizations PROPERTY FOLDER Dependencies/BabylonNative/Plugins/)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVETRACING)
    set_property(TARGET NativeTracing PROPERTY FOLDER Dependencies/BabylonNative/Plugins/)
endif()

if(BABYLON_NATIVE_PLUGIN_TESTUTILS)
    set_property(TARGET TestUtils PROPERTY FOLDER Dependencies/BabylonNative/Plugins/)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVEXR AND (WIN32 OR ANDROID OR IOS))
    set_property(TARGET NativeXr PROPERTY FOLDER Dependencies/BabylonNative/Plugins/)
endif()

# Babylon Native polyfills
if(BABYLON_NATIVE_POLYFILL_WINDOW)
    set_property(TARGET Window PROPERTY FOLDER Dependencies/BabylonNative/Polyfills/)
endif()

if(BABYLON_NATIVE_POLYFILL_CANVAS)
    set_property(TARGET Canvas PROPERTY FOLDER Dependencies/BabylonNative/Polyfills/)
endif()

if(JSRUNTIMEHOST_POLYFILL_CONSOLE)
    set_property(TARGET Console PROPERTY FOLDER Dependencies/BabylonNative/Polyfills/)
endif()

if(JSRUNTIMEHOST_POLYFILL_SCHEDULING)
    set_property(TARGET Scheduling PROPERTY FOLDER Dependencies/BabylonNative/Polyfills/)
endif()

if(JSRUNTIMEHOST_POLYFILL_ABORT_CONTROLLER)
    set_property(TARGET AbortController PROPERTY FOLDER Dependencies/BabylonNative/Polyfills/)
endif()

if(JSRUNTIMEHOST_POLYFILL_URL)
    set_property(TARGET URL PROPERTY FOLDER Dependencies/BabylonNative/Polyfills/)
endif()

if(JSRUNTIMEHOST_POLYFILL_WEBSOCKET)
    set_property(TARGET WebSocket PROPERTY FOLDER Dependencies/BabylonNative/Polyfills/)
endif()

if(JSRUNTIMEHOST_POLYFILL_XMLHTTPREQUEST)
    set_property(TARGET XMLHttpRequest PROPERTY FOLDER Dependencies/BabylonNative/Polyfills/)
endif()

add_subdirectory(imgui)
set_property(TARGET imgui PROPERTY FOLDER Dependencies/)
