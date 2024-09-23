set(SOURCES
    "Include/imconfig.h"
    "Include/imgui_internal.h"
    "Include/imgui.h"
    "Include/imstb_rectpack.h"
    "Include/imstb_textedit.h"
    "Include/imstb_truetype.h"
    "Include/backends/imgui_impl_glfw.h"
    "Include/backends/imgui_impl_babylon.h"
    
    "Source/imgui_demo.cpp"
    "Source/imgui_draw.cpp"
    "Source/imgui_tables.cpp"
    "Source/imgui_widgets.cpp"
    "Source/imgui.cpp"
    "Source/backends/imgui_impl_glfw.cpp"
    "Source/backends/imgui_impl_babylon.cpp")

set(SHADERS 
    "Source/shaders/fs_imgui_color.bin.h"
    "Source/shaders/fs_imgui_cubemap.bin.h"
    "Source/shaders/fs_imgui_image_swizz.bin.h"
    "Source/shaders/fs_imgui_image.bin.h"
    "Source/shaders/fs_imgui_latlong.bin.h"
    "Source/shaders/fs_imgui_texture.bin.h"
    "Source/shaders/fs_ocornut_imgui.bin.h"
    "Source/shaders/vs_imgui_color.bin.h"
    "Source/shaders/vs_imgui_cubemap.bin.h"
    "Source/shaders/vs_imgui_image.bin.h"
    "Source/shaders/vs_imgui_latlong.bin.h"
    "Source/shaders/vs_imgui_texture.bin.h"
    "Source/shaders/vs_ocornut_imgui.bin.h")

set(ASSETS
    "Source/assets/icons_font_awesome.ttf.h"
    "Source/assets/icons_kenney.ttf.h"
    "Source/assets/roboto_regular.ttf.h"
    "Source/assets/robotomono_regular.ttf.h")

add_library(imgui ${SOURCES} ${SHADERS} ${ASSETS})

target_include_directories(imgui
    PUBLIC "Include"
    PRIVATE "Source")

target_link_libraries(imgui
    PUBLIC glfw
    PUBLIC GraphicsDeviceContext
    PRIVATE bgfx
    PRIVATE bx)

if(MSVC)
    target_compile_definitions(imgui
        PRIVATE NOMINMAX 
        PRIVATE "/Zc:preprocessor")
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
target_compile_features(imgui PRIVATE cxx_std_17)

