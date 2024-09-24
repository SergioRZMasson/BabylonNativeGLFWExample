#include <filesystem>
#include <iostream>
#include <stdio.h>
#include <random>
#include <vector>
#include <array>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/NativeOptimizations.h>
#include <Babylon/Plugins/NativeInput.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <Babylon/Polyfills/Canvas.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#if GLFW_VERSION_MINOR < 2
#error "GLFW 3.2 or later is required"
#endif // GLFW_VERSION_MINOR < 2

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_babylon.h"

#if TARGET_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#elif TARGET_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#elif TARGET_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#endif //
#include <GLFW/glfw3native.h>

std::unique_ptr<Babylon::AppRuntime> runtime{};
std::unique_ptr<Babylon::Graphics::Device> device{};
std::unique_ptr<Babylon::Graphics::DeviceUpdate> update{};
Babylon::Plugins::NativeInput* nativeInput{};
std::unique_ptr<Babylon::Polyfills::Canvas> nativeCanvas{};
bool minimized = false;

#define INITIAL_WIDTH 1920
#define INITIAL_HEIGHT 1080

static bool s_showImgui = false;

static void* glfwNativeWindowHandle( GLFWwindow* _window )
{
#if TARGET_PLATFORM_LINUX
	return (void*) (uintptr_t) glfwGetX11Window( _window );
#elif TARGET_PLATFORM_OSX
	return ((NSWindow*) glfwGetCocoaWindow( _window )).contentView;
#elif TARGET_PLATFORM_WINDOWS
	return glfwGetWin32Window( _window );
#endif // TARGET_PLATFORM_
}

class RandomNumberGenerator
{
public:
	RandomNumberGenerator( uint32_t s = 0xFEA4BEE5 )
	{
		SetSeed( s );
	}

	// Default int range is [MIN_INT, MAX_INT].  Max value is included.
	int32_t NextInt( void )
	{
		return (int32_t) Rand();
	}

	uint32_t NextInt( uint32_t MaxVal )
	{
		return Rand() % (MaxVal + 1);
	}

	int32_t NextInt( int32_t MinVal, int32_t MaxVal )
	{
		return MinVal + NextInt( MaxVal - MinVal );
	}

	// Default float range is [0.0f, 1.0f).  Max value is excluded.
	float NextFloat( float MaxVal = 1.0f )
	{
		union
		{
			uint32_t intRep; float fltRep;
		};
		intRep = 0x3f800000 | (NextInt() & 0x7FFFFF);
		return (fltRep - 1.0f) * MaxVal;
	}

	float NextFloat( float MinVal, float MaxVal )
	{
		return MinVal + NextFloat( MaxVal - MinVal );
	}

	void SetSeed( uint32_t s ) 
	{
		m_State[0] = s;

		for( int i = 1; i < n; ++i )
			m_State[i] = 1812433253ul * (m_State[i - 1] ^ (m_State[i - 1] >> 30)) + i;

		m_Pos = n;
	}

private:

	static const int n = 624, m = 397;

	uint32_t m_State[n];
	uint32_t m_Pos;

	inline uint32_t Twiddle( uint32_t u, uint32_t v )
	{
		return (((u & 0x80000000ul) | (v & 0x7FFFFFFFul)) >> 1) ^ ((v & 1ul) * 0x9908B0DFul);
	}

	uint32_t Rand()
	{
		if( m_Pos == n )
		{
			for( uint32_t i = 0; i < n - m; ++i )
				m_State[i] = m_State[i + m] ^ Twiddle( m_State[i], m_State[i + 1] );
			for( uint32_t i = n - m; i < n - 1; ++i )
				m_State[i] = m_State[i + m - n] ^ Twiddle( m_State[i], m_State[i + 1] );
			m_State[n - 1] = m_State[m - 1] ^ Twiddle( m_State[n - 1], m_State[0] );

			m_Pos = 0;
		}

		uint32_t x = m_State[m_Pos++];
		x ^= (x >> 11);
		x ^= (x << 7) & 0x9D2C5680ul;
		x ^= (x << 15) & 0xEFC60000ul;
		return x ^ (x >> 18);

	}
};


// Doom fire logic
namespace DoomFire
{

__declspec(align(16)) struct Color
{
	Color( float r, float g, float b ) : r{ r }, g{ g }, b{ b }, a{ 1.0f } {};
	float r;
	float g;
	float b;
	float a;
};

std::vector<Color> fireColorsPalette = { { 7.0f, 7.0f,  7.0f }, {  31.0f, 7.0f,  7.0f }, {  47.0f, 15.0f,  7.0f }, {  71.0f, 15.0f,  7.0f }, {  87.0f, 23.0f,  7.0f }, {  103.0f, 31.0f,  7.0f }, {  119.0f, 31.0f,  7.0f }, {  143.0f, 39.0f,  7.0f }, {  159.0f, 47.0f,  7.0f }, {  175.0f, 63.0f,  7.0f }, {  191.0f, 71.0f,  7.0f }, {  199.0f, 71.0f,  7.0f }, {  223.0f, 79.0f,  7.0f }, {  223.0f, 87.0f,  7.0f }, {  223.0f, 87.0f,  7.0f }, {  215.0f, 95.0f,  7.0f }, {  215.0f, 95.0f,  7.0f }, {  215.0f, 103.0f,  15.0f }, {  207.0f, 111.0f,  15.0f }, {  207.0f, 119.0f,  15.0f }, {  207.0f, 127.0f,  15.0f }, {  207.0f, 135.0f,  23.0f }, {  199.0f, 135.0f,  23.0f }, {  199.0f, 143.0f,  23.0f }, {  199.0f, 151.0f,  31.0f }, {  191.0f, 159.0f,  31.0f }, {  191.0f, 159.0f,  31.0f }, {  191.0f, 167.0f,  39.0f }, {  191.0f, 167.0f,  39.0f }, {  191.0f, 175.0f,  47.0f }, {  183.0f, 175.0f,  47.0f }, {  183.0f, 183.0f,  47.0f }, {  183.0f, 183.0f,  55.0f }, {  207.0f, 207.0f,  111.0f }, {  223.0f, 223.0f,  159.0f }, {  239.0f, 239.0f,  199.0f }, {  255.0f, 255.0f,  255.0f } };
RandomNumberGenerator generator;

inline void increaseFireSource( Napi::Int32Array& intensityArray, size_t numPerSide )
{
	size_t overflowPixelIndex = numPerSide * numPerSide * numPerSide;// all the pixels of the fire
	size_t sideSquare = numPerSide * numPerSide;

	for( size_t column = 0; column <= numPerSide; column++ )
	{
		size_t columnDelta = column * numPerSide;

		for( size_t depth = 0; depth <= numPerSide; depth++ )
		{
			size_t pixelIndex = (overflowPixelIndex - sideSquare) + columnDelta + depth;   //find last pixel of the colunm
			int32_t currentFireIntensity = intensityArray[pixelIndex];

			int32_t increase = static_cast<int32_t>(std::floor(generator.NextFloat( ) * 7.0f));
			int32_t newFireIntensity = std::min( 36, currentFireIntensity + increase );
			intensityArray[pixelIndex] = newFireIntensity;
		}
	}
}

inline void calculateFirePropagation( Napi::Int32Array& intensityArray, size_t numPerSide )
{
	auto instanceCount = numPerSide * numPerSide * numPerSide;
	auto sideSquare = numPerSide * numPerSide;

	for( auto currentPixelIndex = 0; currentPixelIndex < (instanceCount - sideSquare); currentPixelIndex++ )
	{
		auto belowPixelIndex = currentPixelIndex + (sideSquare);   // takes the reference value and adds a width

		int32_t decay = static_cast<int32_t>(generator.NextFloat( 2.0f ) );  // fire intensity discount
		int32_t belowPixelFireIntensity = intensityArray[belowPixelIndex];
		int32_t newFireIntensity = std::max( belowPixelFireIntensity - decay, 0 );
		int32_t direction = static_cast<int32_t>(generator.NextFloat( 2.0f ) ) - 1;
		direction = std::max( direction, 1 );
		int32_t decayDirection = decay * direction;
		intensityArray[currentPixelIndex - decayDirection] = newFireIntensity;
	}
}

inline void renderFire( Napi::Float32Array colorData, Napi::Int32Array& intensityArray, int numPerSide )
{
	for( size_t instanceID = 0; instanceID < (numPerSide * numPerSide * numPerSide); instanceID++ )
	{
		uint32_t fireIntensity = intensityArray[instanceID];
		Color& fireColor = fireColorsPalette[fireIntensity];

		colorData[instanceID * 4 + 0] = fireColor.r / 255.0f;
		colorData[instanceID * 4 + 1] = fireColor.g / 255.0f;
		colorData[instanceID * 4 + 2] = fireColor.b / 255.0f;
		colorData[instanceID * 4 + 3] = fireColor.a / 255.0f;
	}
}
}



void Uninitialize()
{
	if( device )
	{
		update->Finish();
		device->FinishRenderingCurrentFrame();
		ImGui_ImplBabylon_Shutdown();
	}

	nativeInput = {};
	runtime.reset();
	nativeCanvas.reset();
	update.reset();
	device.reset();
}

void RefreshBabylon( GLFWwindow* window )
{
	Uninitialize();

	int width, height;
	glfwGetWindowSize( window, &width, &height );

	Babylon::Graphics::Configuration graphicsConfig{};
	graphicsConfig.Window = (Babylon::Graphics::WindowT) glfwNativeWindowHandle( window );
	graphicsConfig.Width = width;
	graphicsConfig.Height = height;
	graphicsConfig.MSAASamples = 4;

	device = std::make_unique<Babylon::Graphics::Device>( graphicsConfig );
	update = std::make_unique<Babylon::Graphics::DeviceUpdate>( device->GetUpdate( "update" ) );
	device->StartRenderingCurrentFrame();
	update->Start();

	runtime = std::make_unique<Babylon::AppRuntime>();

	runtime->Dispatch( []( Napi::Env env )
	{
		env.Global().Set( "nativeUpdate", Napi::Function::New( env, []( const Napi::CallbackInfo& info )
		{
			auto colorArray = info[0].As<Napi::Float32Array>();
			auto intensityArray = info[1].As<Napi::Int32Array>();
			int numPerSide = info[2].As<Napi::Number>();

			DoomFire::increaseFireSource( intensityArray, numPerSide );
			DoomFire::calculateFirePropagation( intensityArray, numPerSide );
			DoomFire::renderFire( colorArray, intensityArray, numPerSide );

		} ) );

		device->AddToJavaScript( env );

		Babylon::Polyfills::Console::Initialize( env, []( const char* message, auto )
		{
			std::cout << message << std::endl;
		} );

		Babylon::Polyfills::Window::Initialize( env );
		Babylon::Polyfills::XMLHttpRequest::Initialize( env );

		nativeCanvas = std::make_unique <Babylon::Polyfills::Canvas>( Babylon::Polyfills::Canvas::Initialize( env ) );

		Babylon::Plugins::NativeEngine::Initialize( env );
		Babylon::Plugins::NativeOptimizations::Initialize( env );

		nativeInput = &Babylon::Plugins::NativeInput::CreateForJavaScript( env );
		auto context = &Babylon::Graphics::DeviceContext::GetFromJavaScript( env );

		ImGui_ImplBabylon_SetContext( context );
	} );

	Babylon::ScriptLoader loader{ *runtime };
	loader.Eval( "document = {}", "" );
	// Commenting out recast.js for now because v8jsi is ncompatible with asm.js.
	loader.LoadScript( "app:///Scripts/babylon.max.js" );
	loader.LoadScript( "app:///Scripts/babylonjs.loaders.js" );
	loader.LoadScript( "app:///Scripts/babylonjs.materials.js" );
	loader.LoadScript( "app:///Scripts/babylon.gui.js" );
	loader.LoadScript( "app:///Scripts/DoomFireJS.js" );

	ImGui_ImplBabylon_Init( width, height );
}

static void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if( key == GLFW_KEY_R && action == GLFW_PRESS )
	{
		RefreshBabylon( window );
	}
	else if( key == GLFW_KEY_D && action == GLFW_PRESS )
	{
		s_showImgui = !s_showImgui;
	}
}

void mouse_button_callback( GLFWwindow* window, int button, int action, int mods )
{
	if( s_showImgui )
		return;

	double xpos, ypos;
	glfwGetCursorPos( window, &xpos, &ypos );
	int32_t x = static_cast<int32_t>(xpos);
	int32_t y = static_cast<int32_t>(ypos);

	if( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS )
		nativeInput->MouseDown( Babylon::Plugins::NativeInput::LEFT_MOUSE_BUTTON_ID, x, y );
	else if( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE )
		nativeInput->MouseUp( Babylon::Plugins::NativeInput::LEFT_MOUSE_BUTTON_ID, x, y );
	else if( button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS )
		nativeInput->MouseDown( Babylon::Plugins::NativeInput::RIGHT_MOUSE_BUTTON_ID, x, y );
	else if( button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE )
		nativeInput->MouseUp( Babylon::Plugins::NativeInput::RIGHT_MOUSE_BUTTON_ID, x, y );
	else if( button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS )
		nativeInput->MouseDown( Babylon::Plugins::NativeInput::MIDDLE_MOUSE_BUTTON_ID, x, y );
	else if( button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE )
		nativeInput->MouseUp( Babylon::Plugins::NativeInput::MIDDLE_MOUSE_BUTTON_ID, x, y );
}

static void cursor_position_callback( GLFWwindow* window, double xpos, double ypos )
{
	int32_t x = static_cast<int32_t>(xpos);
	int32_t y = static_cast<int32_t>(ypos);

	nativeInput->MouseMove( x, y );
}

void scroll_callback( GLFWwindow* window, double xoffset, double yoffset )
{
	if( s_showImgui )
		return;

	nativeInput->MouseWheel( Babylon::Plugins::NativeInput::MOUSEWHEEL_Y_ID, static_cast<int>(-yoffset * 100.0) );
}

static void window_resize_callback( GLFWwindow* window, int width, int height )
{
	device->UpdateSize( width, height );
}


static void decrease_fire_intesnity()
{
	runtime->Dispatch( []( Napi::Env env )
	{
		env.Global().Get( "decreaseFireSource" ).As<Napi::Function>().Call( {} );
	} );
}

static void increase_fire_intesnity()
{
	runtime->Dispatch( []( Napi::Env env )
	{
		env.Global().Get( "increaseFireSource" ).As<Napi::Function>().Call( {} );
	} );
}

int main()
{
	if( !glfwInit() )
		exit( EXIT_FAILURE );

	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
	glfwWindowHint( GLFW_MAXIMIZED, GLFW_TRUE );

	auto window = glfwCreateWindow( INITIAL_WIDTH, INITIAL_HEIGHT, "Simple example", NULL, NULL );

	if( !window )
	{
		glfwTerminate();
		exit( EXIT_FAILURE );
	}

	glfwSetKeyCallback( window, key_callback );
	glfwSetWindowSizeCallback( window, window_resize_callback );
	glfwSetCursorPosCallback( window, cursor_position_callback );
	glfwSetMouseButtonCallback( window, mouse_button_callback );
	glfwSetScrollCallback( window, scroll_callback );

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void) io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOther( window, true );

	// Our state
	bool show_ball = true;
	bool show_floor = true;
	ImVec4 ballColor = ImVec4( 0.55f, 0.55f, 0.55f, 1.00f );

	RefreshBabylon( window );

	while( !glfwWindowShouldClose( window ) )
	{
		if( device )
		{
			update->Finish();
			device->FinishRenderingCurrentFrame();
			device->StartRenderingCurrentFrame();
			update->Start();
		}
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplBabylon_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		if( s_showImgui )
		{
			ImGui::NewFrame();

			static float ballSize = 1.0f;

			ImGui::Begin( "Scene Editor Example" );

			ImGui::Text( "Use this controllers to change values in the Babylon scene." );

			if( ImGui::Button( "Increase Fire Intensity" ) )
			{
				increase_fire_intesnity();
			}

			if( ImGui::Button( "Decrase Fire Intensity" ) )
			{
				decrease_fire_intesnity();
			}

			if( ImGui::Button( "Resume" ) )
			{
				s_showImgui = false;
			}

			ImGui::SameLine();

			ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate );
			ImGui::End();

			ImGui::Render();
			ImGui_ImplBabylon_RenderDrawData( ImGui::GetDrawData() );
		}
	}

	Uninitialize();

	// Cleanup
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow( window );
	glfwTerminate();
	exit( EXIT_SUCCESS );
}