// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <moai_config.h>
#include <lua.h>
#include <lua-headers/moai_lua.h>
#include <joystick.h>
#include <GL/glfw.h>
#include <host-glfw/GlfwHost.h>
#include <string.h>

#if LUA_VERSION_NUM < 502
#ifdef MOAI_WITH_LUAEXT
#undef MOAI_WITH_LUAEXT
#define MOAI_WITH_LUAEXT 0
#endif
#endif

#define UNUSED(p) (( void )p)

#include <moai-http-client/host.h>
#include <moai-sim/host.h>
#include <moai-util/host.h>

#if MOAI_WITH_BOX2D
#include <moai-box2d/host.h>
#endif

#if MOAI_WITH_CHIPMUNK
#include <moai-chipmunk/host.h>
#endif

#if MOAI_WITH_FMOD_DESIGNER
#include <moai-fmod-designer/host.h>
#endif

#if MOAI_WITH_FMOD_EX
#include <moai-fmod-ex/host.h>
#endif

#if MOAI_WITH_HARNESS
#include <moai-harness/host.h>
#endif

#if MOAI_WITH_HTTP_CLIENT
#include <moai-http-client/host.h>
#endif

#if MOAI_WITH_LUAEXT
#include <moai-luaext/host.h>
#endif

#if MOAI_WITH_PARTICLE_PRESETS
#include <ParticlePresets.h>
#endif

#if MOAI_WITH_UNTZ
#include <moai-untz/host.h>
#endif

#ifdef _WIN32

#include <glut.h>

#if MOAI_WITH_FOLDER_WATCHER
#include <FolderWatcher-win.h>
#endif
#else
#ifdef MOAI_OS_LINUX
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif
#endif

#ifdef __APPLE__

#include <OpenGL/OpenGL.h>

#if MOAI_WITH_FOLDER_WATCHER
#include <FolderWatcher-mac.h>
#endif
#endif

namespace GlfwInputDeviceID {
	enum {
		JOYSTICK0,
		//JOYSTICK1,
		//JOYSTICK2,
		//JOYSTICK3,
		DEVICE,
		TOTAL,
	};
}

namespace GlfwInputDeviceSensorID {
	enum {
		KEYBOARD,
		POINTER,
		MOUSE_LEFT,
		MOUSE_MIDDLE,
		MOUSE_RIGHT,
		TOTAL,
	};
}

namespace GlfwJoystickSensorID {
	enum {
		JOYSTICK_ANALOG_LEFT,
		JOYSTICK_ANALOG_RIGHT,
		JOYSTICK_ANALOG_SHOULDER,
		
		JOYSTICK_DPAD_DOWN,
		JOYSTICK_DPAD_UP,
		JOYSTICK_DPAD_LEFT,
		JOYSTICK_DPAD_RIGHT,
		JOYSTICK_START,
		JOYSTICK_BACK,
		JOYSTICK_ANALOG_LEFT_BUTTON,
		JOYSTICK_ANALOG_RIGHT_BUTTON,
		JOYSTICK_SHOULDER_LEFT,
		JOYSTICK_SHOULDER_RIGHT,
		JOYSTICK_HOME,
		JOYSTICK_BUTTON_A,
		JOYSTICK_BUTTON_B,
		JOYSTICK_BUTTON_X,
		JOYSTICK_BUTTON_Y,
		TOTAL,
	};
}

// --==== Forward refs ====
void printVidmode( const GLFWvidmode& mode );
void printWindowParams();
void setOpenWindowHints();
static void GLFWCALL _onReshape( int width, int height );
static void GLFWCALL _onKey( int key, int state );
static void joystickAxisCallback( int joyId, int axisId, float state );
static void joystickButtonCallback( int joyId, int buttonId, int state );


static bool sHasWindow = false;
static bool sExitFullscreen = false;
// static bool sDynamicallyReevaluateLuaFiles = false;

static int sWinX;
static int sWinY;
static int sWinWidth;
static int sWinHeight;

//================================================================//
// helper functions
//================================================================//

static void _specialKeys( int key, int state ) {
	//	GLFW_KEY_SPECIAL      256
	//	GLFW_KEY_ESC          (GLFW_KEY_SPECIAL+1)
	//	GLFW_KEY_F1           (GLFW_KEY_SPECIAL+2)
	//		..
	//	GLFW_KEY_F25          (GLFW_KEY_SPECIAL+26)
	//	GLFW_KEY_UP           (GLFW_KEY_SPECIAL+27)
	//	GLFW_KEY_DOWN         (GLFW_KEY_SPECIAL+28)
	//	GLFW_KEY_LEFT         (GLFW_KEY_SPECIAL+29)
	//	GLFW_KEY_RIGHT        (GLFW_KEY_SPECIAL+30)
	//	GLFW_KEY_LSHIFT       (GLFW_KEY_SPECIAL+31)
	//	GLFW_KEY_RSHIFT       (GLFW_KEY_SPECIAL+32)
	//	GLFW_KEY_LCTRL        (GLFW_KEY_SPECIAL+33)
	//	GLFW_KEY_RCTRL        (GLFW_KEY_SPECIAL+34)
	//	GLFW_KEY_LALT         (GLFW_KEY_SPECIAL+35)
	//	GLFW_KEY_RALT         (GLFW_KEY_SPECIAL+36)
	//	GLFW_KEY_TAB          (GLFW_KEY_SPECIAL+37)
	//	GLFW_KEY_ENTER        (GLFW_KEY_SPECIAL+38)
	//	GLFW_KEY_BACKSPACE    (GLFW_KEY_SPECIAL+39)
	//	GLFW_KEY_INSERT       (GLFW_KEY_SPECIAL+40)
	//	GLFW_KEY_DEL          (GLFW_KEY_SPECIAL+41)
	//	GLFW_KEY_PAGEUP       (GLFW_KEY_SPECIAL+42)
	//	GLFW_KEY_PAGEDOWN     (GLFW_KEY_SPECIAL+43)
	//	GLFW_KEY_HOME         (GLFW_KEY_SPECIAL+44)
	//	GLFW_KEY_END          (GLFW_KEY_SPECIAL+45)
	//	GLFW_KEY_CAPS_LOCK    (GLFW_KEY_SPECIAL+64)
	//	GLFW_KEY_SCROLL_LOCK  (GLFW_KEY_SPECIAL+65)
	
	// --==== Return if not a special key ====
	if( key < GLFW_KEY_SPECIAL )
		return;
	
	// --==== Predefined keys ====
	if ( key == GLFW_KEY_F1 ) {
		
		static bool toggle = true;
		
		if ( toggle ) {
			AKUReleaseGfxContext();
		}
		else {
			AKUDetectGfxContext();
		}
		toggle = !toggle;
	}
	
	if ( key == GLFW_KEY_F2 ) {
		AKUSoftReleaseGfxResources ( 0 );
	}
	
	// --==== Send key event ====
	AKUEnqueueKeyboardEvent(
							GlfwInputDeviceID::DEVICE,
							GlfwInputDeviceSensorID::KEYBOARD,
							key,
							(state == GLFW_PRESS )
							);
	
	// --==== Send shift/ctrl/alt seperately too ====
	if ( key == GLFW_KEY_LSHIFT || key == GLFW_KEY_RSHIFT ) {
		AKUEnqueueKeyboardShiftEvent (
									  GlfwInputDeviceID::DEVICE,
									  GlfwInputDeviceSensorID::KEYBOARD,
									  (state == GLFW_PRESS)
									  );
	}
	if ( key == GLFW_KEY_LCTRL || key == GLFW_KEY_RCTRL ) {
		AKUEnqueueKeyboardControlEvent (
										GlfwInputDeviceID::DEVICE,
										GlfwInputDeviceSensorID::KEYBOARD,
										(state == GLFW_PRESS)
										);
	}
	if ( key == GLFW_KEY_LALT || key == GLFW_KEY_RALT ) {
		AKUEnqueueKeyboardAltEvent (
									GlfwInputDeviceID::DEVICE,
									GlfwInputDeviceSensorID::KEYBOARD,
									(state == GLFW_PRESS)
									);
	}
}

//================================================================//
// glut callbacks
//================================================================//

//----------------------------------------------------------------//
static void GLFWCALL _onKey( int key, int state ) {
	fprintf( stdout, "Key: %d %s\n", key, (( GLFW_PRESS == state ) ? "pressed" : "released"));
	
	_specialKeys( key, state );
	AKUEnqueueKeyboardEvent ( GlfwInputDeviceID::DEVICE, GlfwInputDeviceSensorID::KEYBOARD, key, state == GLFW_PRESS );
}

//----------------------------------------------------------------//
static void GLFWCALL _onMouseButton( int button, int state ) {
	fprintf( stdout, "mouse button: ");
	switch( button ) {
		case GLFW_MOUSE_BUTTON_2:	// a.k.a GLFW_MOUSE_BUTTON_RIGHT
			AKUEnqueueButtonEvent ( GlfwInputDeviceID::DEVICE,
								   GlfwInputDeviceSensorID::MOUSE_RIGHT, ( state == GLFW_PRESS ));
			fprintf( stdout, "right");
			break;
		case GLFW_MOUSE_BUTTON_LEFT: // a.k.a GLFW_MOUSE_BUTTON_1
			AKUEnqueueButtonEvent (
								   GlfwInputDeviceID::DEVICE,
								   GlfwInputDeviceSensorID::MOUSE_LEFT, ( state == GLFW_PRESS )
								   );
			fprintf( stdout, "left");
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:// a.k.a GLFW_MOUSE_BUTTON_3
			AKUEnqueueButtonEvent(
								  GlfwInputDeviceID::DEVICE,
								  GlfwInputDeviceSensorID::MOUSE_MIDDLE, ( state == GLFW_PRESS )
								  );
			fprintf( stdout, "middle");
			break;
			//case ￼GLFW_MOUSE_BUTTON_2: // through GLFW_MOUSE_BUTTON_1-8
			//	cout << "right";
			//	break;
	};
	fprintf( stdout, " %s\n", (( GLFW_PRESS == state ) ? "pressed" : "released"));
}

//----------------------------------------------------------------//
static void GLFWCALL _onMouseMove( int x, int y ) {
	AKUEnqueuePointerEvent ( GlfwInputDeviceID::DEVICE, GlfwInputDeviceSensorID::POINTER, x, y );
	//cout << "Mouse move: " << x << "," << y << endl;
}


//----------------------------------------------------------------//
static void GLFWCALL _onMouseWheel( int pos ) {
	fprintf( stdout, "Mouse wheel: %d\n", pos);
 	// TODO - MOAI
 	//AKUEnqueuePointerEvent ( GlfwInputDeviceID::DEVICE, GlfwInputDeviceSensorID::POINTER, x, y );
}

//----------------------------------------------------------------//
static void _onMouseDrag ( int x, int y ) {
	AKUEnqueuePointerEvent ( GlfwInputDeviceID::DEVICE, GlfwInputDeviceSensorID::POINTER, x, y );
}

//----------------------------------------------------------------//
static void _onPaint () {
	
	AKURender ();
	// Swap front and back buffers call glfwPollEvents (unless glfwDisable( GLFW_AUTO_POLL_EVENTS) )
	glfwSwapBuffers();
}

//----------------------------------------------------------------//
static void GLFWCALL _onReshape( int width, int height ) {
	fprintf( stdout, "Window resized: %dx%d\n", width, height );
	
	if ( sExitFullscreen ) {
		width = sWinWidth;
		height = sWinHeight;
		
		sExitFullscreen = false;
	}
	
	glfwSetWindowSize( width, height );
	AKUSetScreenSize ( width, height );
	AKUSetViewSize ( width, height );
}



//================================================================//
// aku callbacks
//================================================================//

//void	_AKUEnterFullscreenModeFunc		();
//void	_AKUExitFullscreenModeFunc		();
void	_AKUOpenWindowFunc				( const char* title, int width, int height );

//----------------------------------------------------------------//
//void _AKUEnterFullscreenModeFunc () {
//	
//	if ( sHasWindow ) {
//		
//		sExitFullscreen = false;
//		glutFullScreen ();
//	}
//}

//----------------------------------------------------------------//
//void _AKUExitFullscreenModeFunc () {
//	
//	if ( sHasWindow ) {
//		
//		sExitFullscreen = true;
//		glutPositionWindow ( sWinX, sWinY );
//		glutReshapeWindow ( sWinWidth, sWinHeight );
//	}
//}

//----------------------------------------------------------------//
void _AKUOpenWindowFunc ( const char* title, int width, int height ) {
	GLFWvidmode mode;
	glfwGetDesktopMode( &mode );
	sWinX = (mode.Width - width) / 2;
	sWinY = (mode.Height - height) / 2;
	
	sWinWidth = width;
	sWinHeight = height;
	
	if ( !sHasWindow ) {
		
		setOpenWindowHints();
		
		// --==== Open OpenGL window ====
		if( !glfwOpenWindow(
							width, height, // 0 = use default 640x480
							0,0,0,0,       // int RGBA bit depth  0 = use default
							16,            // int depth bit depth
							0,             // int stencil bit depth
							GLFW_WINDOW    // [GLFW_WINDOW | GLFW_FULLSCREEN]
							)) {
			fprintf( stderr, "Failed to open GLFW window\n" );
			glfwTerminate();
			exit( EXIT_FAILURE );
		}
		sHasWindow = true;
	}
	
	// --==== Set GLFW event callbacks ====
	glfwSetWindowSizeCallback( _onReshape );
	
	glfwSetMouseButtonCallback( _onMouseButton );
	glfwSetMousePosCallback( _onMouseMove );
	glfwSetMouseWheelCallback( _onMouseWheel );
	
	glfwSetKeyCallback( _onKey );
	// glfwSetCharCallback( onChar );
	
	// --==== Key related ====
	// glfwEnable( GLFW_KEY_REPEAT );
	// glfwDisable( GLFW_SYSTEM_KEYS );	// Turn of system key combinations
	// glfwEnable( GLFW_SYSTEM_KEYS ); // Default
	
	// --==== Window size/pos/title ====
	glfwSetWindowSize( sWinWidth, sWinHeight );
	glfwSetWindowPos( sWinX, sWinY );
	glfwSetWindowTitle( title );
	
	// --==== Setup ====
	glfwEnable( GLFW_MOUSE_CURSOR );
	// glfwDisable( GLFW_MOUSE_CURSOR );
	glfwSwapInterval( 1 );	// vsync on
	
	AKUDetectGfxContext();
	AKUSetScreenSize ( width, height );
	
	printWindowParams();
	
//	TODO - what does this code do?
//#ifdef __APPLE__
//	GLint sync = 1;
//	CGLContextObj ctx = CGLGetCurrentContext();
//	CGLSetParameter (ctx, kCGLCPSwapInterval, &sync);
//#endif
}

//================================================================//
// aku-harness callbacks
//================================================================//

#if MOAI_WITH_HARNESS
void _debuggerTracebackFunc      ( const char* message, lua_State* L, int level );

void _debuggerTracebackFunc ( const char* message, lua_State* L, int level ) {
	AKUDebugHarnessHandleError ( message, L, level );
}
#endif

//================================================================//
// GlutHost
//================================================================//

//----------------------------------------------------------------//
static void _cleanup () {
	
	// TODO:
	// don't call this on windows; atexit conflict with untz
	// possible to fix?
	//AKUClearMemPool ();
	
#if MOAI_WITH_BOX2D
	AKUFinalizeBox2D ();
#endif
	
#if MOAI_WITH_CHIPMUNK
	AKUFinalizeChipmunk ();
#endif
	
#if MOAI_WITH_HTTP_CLIENT
	AKUFinalizeHttpClient ();
#endif
	
	AKUFinalizeUtil ();
	AKUFinalizeSim ();
	AKUFinalize ();
	
	// if ( sDynamicallyReevaluateLuaFiles ) {
	//  #ifdef _WIN32
	//    winhostext_CleanUp ();
	//  #elif __APPLE__
	//    FWStopAll ();
	//  #endif
	// }
}

//----------------------------------------------------------------//
void _printMoaiVersion();
void _printMoaiVersion () {
	
	static const int length = 255;
	char version [ length ];
	AKUGetMoaiVersion ( version, length );
	printf ( "%s\n", version );
}

//----------------------------------------------------------------//
void joystickAxisCallback( int joyId, int axisId, float lr, float ud );
void joystickAxisCallback( int joyId, int axisId, float lr, float ud ) {
	//printf( "Joystick[%d], axis[%d]: %f,%f\n", joyId, axisId, lr, ud );
	
	switch( axisId ) {
		case 0: // Left stick (range -1 to +1)
			AKUEnqueueJoystickEvent( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_ANALOG_LEFT, lr, ud );
			break;
		case 1: // Right stick (range -1 to +1)
			AKUEnqueueJoystickEvent( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_ANALOG_RIGHT, lr, ud );
			break;
		case 2: // Left/right (range 0 to 1)
			AKUEnqueueJoystickEvent( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_ANALOG_SHOULDER, lr, ud );
			break;
	}
	// JOYSTICK_ANALOG_RIGHT,
}

void joystickButtonCallback( int joyId, int buttonId, int state );
void joystickButtonCallback( int joyId, int buttonId, int state ){
	// printf( "Joystick[%d], Button[%d]: %c\n", joyId, buttonId, (state == GLFW_PRESS) ? '+' : '-' );
	
	switch( buttonId ) {
		case 0: // DPAD UP
			AKUEnqueueButtonEvent( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_DPAD_UP, state == GLFW_PRESS );
			break;
		case 1: // DPAD DOWN
			AKUEnqueueButtonEvent( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_DPAD_DOWN, state == GLFW_PRESS );
			break;
		case 2: // DPAD LEFT
			AKUEnqueueButtonEvent( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_DPAD_LEFT, state == GLFW_PRESS );
			break;
		case 3: // DPAD RIGHT
			AKUEnqueueButtonEvent( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_DPAD_RIGHT, state == GLFW_PRESS );
			break;
		case 4: // Start button
			AKUEnqueueButtonEvent( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_START, state == GLFW_PRESS );
			break;
		case 5: // Back button
			AKUEnqueueButtonEvent( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_BACK, state == GLFW_PRESS );
			break;
		case 6: // Left analog stick's button
			AKUEnqueueButtonEvent( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_ANALOG_LEFT_BUTTON, state == GLFW_PRESS );
			break;
		case 7: // Right analog stick's button
			AKUEnqueueButtonEvent( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_ANALOG_RIGHT_BUTTON, state == GLFW_PRESS );
			break;
		case 8: // Left shoulder button
			AKUEnqueueButtonEvent( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_SHOULDER_LEFT, state == GLFW_PRESS );
			break;
		case 9: // Right shoulder button
			AKUEnqueueButtonEvent( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_SHOULDER_RIGHT, state == GLFW_PRESS );
			break;
		case 10: // Home button (XBox button on xbox controller)
			AKUEnqueueButtonEvent( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_HOME, state == GLFW_PRESS );
			break;
		case 11: // A
			AKUEnqueueButtonEvent( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_BUTTON_A, state == GLFW_PRESS );
			break;
		case 12: // B
			AKUEnqueueButtonEvent( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_BUTTON_B, state == GLFW_PRESS );
			break;
		case 13: // X
			AKUEnqueueButtonEvent( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_BUTTON_X, state == GLFW_PRESS );
			break;
		case 14: // Y
			AKUEnqueueButtonEvent( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_BUTTON_Y, state == GLFW_PRESS );
			break;
	}
}

void joystickConnectionCallback( int joyId, bool connected );
void joystickConnectionCallback( int joyId, bool connected ){
	fprintf( stdout, "JOYSTICK0: %s", (connected ? "connected" : "disconnected") );
	AKUSetInputDeviceActive( GlfwInputDeviceID::JOYSTICK0, connected );
}

//----------------------------------------------------------------//
int GlfwHost ( int argc, char** argv ) {
	
	_printMoaiVersion ();
	
#ifdef _DEBUG
	printf ( "DEBUG BUILD\n" );
#endif
	
	// TODO: integrate this nicely with host
	//AKUInitMemPool ( 100 * 1024 * 1024 );
	atexit ( _cleanup );

	// --==== Initialize GLFW ====
	if( !glfwInit() ) {
		fprintf( stderr, "Failed to initialize GLFW\n" );
		exit( EXIT_FAILURE );
	}
	
	// --==== Setup AKU and load scripts from command line ====
	GlfwRefreshContext();
	
    // --==== Joystick setup ====
	JoystickManager jm;
	jm.init();
	jm.setAxisCallback( joystickAxisCallback );
	jm.setButtonCallback( joystickButtonCallback );
	jm.setConnectionCallback( joystickConnectionCallback );
	jm.dump();
	
	char* lastScript = NULL;
	
	if ( argc < 2 ) {
		AKURunScript ( "main.lua" );
	}
	else {
		
		AKUSetArgv ( argv );
		
		for ( int i = 1; i < argc; ++i ) {
			char* arg = argv [ i ];
			if ( strcmp( arg, "-e" ) == 0 ) {
				// sDynamicallyReevaluateLuaFiles = true;
			}
			else if ( strcmp( arg, "-s" ) == 0 && ++i < argc ) {
				char* script = argv [ i ];
				AKURunString ( script );
			}
			else {
				AKURunScript ( arg );
				lastScript = arg;
			}
		}
	}
	
	//assuming that the last script is the entry point we watch for that directory and its subdirectories
#if MOAI_WITH_FOLDER_WATCHER
	if ( lastScript && sDynamicallyReevaluateLuaFiles ) {
#ifdef _WIN32
		winhostext_WatchFolder ( lastScript );
#elif __APPLE__
		FWWatchFolder( lastScript );
#endif
	}
#endif
	
	// --==== Video mode enumeration ====
	/*
	 const unsigned int modeListLen = 100;
	 GLFWvidmode modeList[ modeListLen ];
	 int modeCnt = glfwGetVideoModes( modeList, modeListLen );
	 cout << "Video Modes:" << endl;
	 for( int i = 0; i<modeCnt; i++ ) {
	 printVidmode( modeList[ i ] );
	 }
	 GLFWvidmode mode;
	 cout << "Desktop Mode:" << endl;
	 glfwGetDesktopMode( &mode );
	 printVidmode( mode );
	 */
	
	// --==== Window creation ====
	if( !sHasWindow ) {
		_AKUOpenWindowFunc( "Todo", 320, 480 );
		
		//	glutTimerFunc ( 0, _onTimer, 0 );
		//
		//	glutMainLoop ();
		// */
		//
		//	return 0;
	}
	
	// --==== Initialize time ====
	double prevTime, deltaTime;	// Current time and enlapsed time
	prevTime = glfwGetTime();
	
	// --==== Main loop ====
	bool isRunning = true;
	double timerInterval = 0;
	while( isRunning ) {
		glClear( GL_COLOR_BUFFER_BIT );
		
		// --==== Timer ====
		deltaTime = glfwGetTime() - prevTime;
		// Clamp
		if( deltaTime < 0 )
			deltaTime = 0;
		
		prevTime += deltaTime;
		
		timerInterval -= deltaTime;
		if( timerInterval < 0 ) {
			timerInterval += AKUGetSimStep();
			
			jm.update();
			
#if MOAI_HOST_USE_DEBUGGER
			AKUDebugHarnessUpdate ();
#endif
			
			AKUUpdate ();
			
#if MOAI_HOST_USE_FMOD_EX
			AKUFmodUpdate ();
#endif
			
#if MOAI_HOST_USE_FMOD_DESIGNER
			AKUFmodDesignerUpdate (( float )fSimStep );
#endif
			
			// if ( sDynamicallyReevaluateLuaFiles ) {
			//  #ifdef _WIN32
			//    winhostext_Query ();
			//  #elif __APPLE__
			//    FWReloadChangedLuaFiles ();
			//  #endif
			// }
			
			_onPaint();
		}
		
		// Min value: 5 − 20ms. Less than this may result in glfwSleep returning immediately, without
		// putting the thread to sleep.
		// glfwSleep( double secs )
		
		isRunning = !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );
	}
	
	// --==== Cleanup ====
	if( glfwGetWindowParam( GLFW_OPENED ) ) {
		//glDeleteTextures( NUM_TEXTURES, tex_id );
	}
	
	// Terminate GLFW
	glfwTerminate();
	
	return EXIT_SUCCESS;
}

void GlfwRefreshContext () {
	
	AKUContextID context = AKUGetContext ();
	if ( context ) {
		AKUDeleteContext ( context );
	}
	AKUCreateContext ();
	
	AKUInitializeUtil ();
	AKUInitializeSim ();
	
#if MOAI_WITH_BOX2D
	AKUInitializeBox2D ();
#endif
	
#if MOAI_WITH_CHIPMUNK
	AKUInitializeChipmunk ();
#endif
	
#if MOAI_WITH_FMOD_EX
	AKUFmodLoad ();
#endif
	
#if MOAI_WITH_FMOD_DESIGNER
	AKUFmodDesignerInit ();
#endif
	
#if MOAI_WITH_LUAEXT
	AKUExtLoadLuacrypto ();
	AKUExtLoadLuacurl ();
	AKUExtLoadLuafilesystem ();
	AKUExtLoadLuasocket ();
	AKUExtLoadLuasql ();
#endif
	
#if MOAI_WITH_HARNESS
	AKUSetFunc_ErrorTraceback ( _debuggerTracebackFunc );
	AKUDebugHarnessInit ();
#endif
	
#if MOAI_WITH_HTTP_CLIENT
	AKUInitializeHttpClient ();
#endif
	
#if MOAI_WITH_PARTICLE_PRESETS
	ParticlePresets ();
#endif
	
#if MOAI_WITH_UNTZ
	AKUInitializeUntz ();
#endif
	
	AKUSetInputConfigurationName ( "AKUGlfw" );

	AKUReserveInputDevices			( GlfwInputDeviceID::TOTAL );
	
	// Mouse/keyboard
	AKUSetInputDevice				( GlfwInputDeviceID::DEVICE, "device" );
	AKUReserveInputDeviceSensors	( GlfwInputDeviceID::DEVICE, GlfwInputDeviceSensorID::TOTAL );
	
	AKUSetInputDeviceKeyboard		( GlfwInputDeviceID::DEVICE, GlfwInputDeviceSensorID::KEYBOARD,		"keyboard" );
	AKUSetInputDevicePointer		( GlfwInputDeviceID::DEVICE, GlfwInputDeviceSensorID::POINTER,		"pointer" );
	AKUSetInputDeviceButton			( GlfwInputDeviceID::DEVICE, GlfwInputDeviceSensorID::MOUSE_LEFT,	"mouseLeft" );
	AKUSetInputDeviceButton			( GlfwInputDeviceID::DEVICE, GlfwInputDeviceSensorID::MOUSE_MIDDLE,	"mouseMiddle" );
	AKUSetInputDeviceButton			( GlfwInputDeviceID::DEVICE, GlfwInputDeviceSensorID::MOUSE_RIGHT,	"mouseRight" );
	
	// Joypad 0
	AKUSetInputDevice				( GlfwInputDeviceID::JOYSTICK0, "joystick0" );
	AKUReserveInputDeviceSensors	( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::TOTAL );
	AKUSetInputDeviceButton( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_DPAD_UP,	"DPAD_UP" );
	AKUSetInputDeviceButton( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_DPAD_DOWN,	"DPAD_DOWN" );
	AKUSetInputDeviceButton( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_DPAD_LEFT,	"DPAD_LEFT" );
	AKUSetInputDeviceButton( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_DPAD_RIGHT,	"DPAD_RIGHT" );
	AKUSetInputDeviceButton( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_START,		"START" );
	AKUSetInputDeviceButton( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_BACK,		"BACK" );
	AKUSetInputDeviceButton( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_ANALOG_LEFT_BUTTON, "ANALOG_LEFT_BTN" );
	AKUSetInputDeviceButton( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_ANALOG_RIGHT_BUTTON, "ANALOG_RIGHT_BTN" );
	AKUSetInputDeviceButton( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_SHOULDER_LEFT, "SHOULDER_LEFT" );
	AKUSetInputDeviceButton( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_SHOULDER_RIGHT, "SHOULDER_RIGHT" );
	AKUSetInputDeviceButton( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_HOME,		"HOME" );
	AKUSetInputDeviceButton( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_BUTTON_A,	"A" );
	AKUSetInputDeviceButton( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_BUTTON_B,	"B" );
	AKUSetInputDeviceButton( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_BUTTON_X,	"X" );
	AKUSetInputDeviceButton( GlfwInputDeviceID::JOYSTICK0, GlfwJoystickSensorID::JOYSTICK_BUTTON_Y,	"Y" );
	
	AKUSetInputDeviceJoystick(
							  GlfwInputDeviceID::JOYSTICK0,
							  GlfwJoystickSensorID::JOYSTICK_ANALOG_LEFT,
							  "ANALOG_LEFT"
							  );
	AKUSetInputDeviceJoystick(
							  GlfwInputDeviceID::JOYSTICK0,
							  GlfwJoystickSensorID::JOYSTICK_ANALOG_RIGHT,
							  "ANALOG_RIGHT"
							  );
	AKUSetInputDeviceJoystick(
							  GlfwInputDeviceID::JOYSTICK0,
							  GlfwJoystickSensorID::JOYSTICK_ANALOG_SHOULDER,
							  "ANALOG_SHOULDER"
							  );

	// TODO
	//	 AKUSetFunc_EnterFullscreenMode ( _AKUEnterFullscreenModeFunc );
	//	 AKUSetFunc_ExitFullscreenMode ( _AKUExitFullscreenModeFunc );
	AKUSetFunc_OpenWindow ( _AKUOpenWindowFunc );

	
	AKURunData ( moai_lua, moai_lua_SIZE, AKU_DATA_STRING, AKU_DATA_ZIPPED );
}

// --==== Helper ====
void printVidmode( const GLFWvidmode& mode ) {
	fprintf( stdout, "%dx%d rgb bits: %d,%d,%d\n", mode.Width, mode.Height, mode.RedBits, mode.GreenBits, mode.BlueBits );
}

void printWindowParams() {
	const char * tab = "   " ;
	fprintf( stdout, "Window:\n" );
	fprintf( stdout, "%s%s\n", tab, (glfwGetWindowParam( GLFW_OPENED ) == GL_TRUE ? "OPEN": "CLOSED"));
	// GL_TRUE if window is opened, else GL_FALSE.
	
	fprintf( stdout, "%s%s\n", tab, (glfwGetWindowParam( GLFW_ACTIVE ) == GL_TRUE ? "Has focus": "Lost focus"));
	// GL_TRUE if window has focus, else GL_FALSE.
	
	fprintf( stdout, "%s%s\n", tab, (glfwGetWindowParam( GLFW_ICONIFIED ) == GL_TRUE ? "Minimised": "Maximised"));
	// GL_TRUE if window is iconified, else GL_FALSE.
	
	fprintf( stdout, "%s%s\n", tab, (glfwGetWindowParam( GLFW_WINDOW_NO_RESIZE ) == GL_TRUE ? "Can resize": "Resize disabled"));
	// GL_TRUE if the window cannot be resized by the user, else GL_FALSE.
	
	fprintf( stdout, "%sOpenGL context: %d.%d%s\n", tab, glfwGetWindowParam( GLFW_OPENGL_VERSION_MAJOR ),
			glfwGetWindowParam( GLFW_OPENGL_VERSION_MINOR ),
			(glfwGetWindowParam( GLFW_OPENGL_DEBUG_CONTEXT ) == GL_TRUE ? "(** DEBUG **)": ""));
	// Major number of the actual version of the context.
	// Minor number of the actual version of the context.
	// GL_TRUE if the context is a debug context.
	
	fprintf( stdout, "%s%s\n", tab, (glfwGetWindowParam( GLFW_ACCELERATED ) == GL_TRUE ? "Hardware accelerated": "No acceleration (software)"));
	// GL_TRUE if window is hardware accelerated, else GL_FALSE.
	
	fprintf( stdout, "%sRGBA: %d,%d,%d,%d\n", tab,
			glfwGetWindowParam( GLFW_RED_BITS ),
			glfwGetWindowParam( GLFW_GREEN_BITS ),
			glfwGetWindowParam( GLFW_BLUE_BITS ),
			glfwGetWindowParam( GLFW_ALPHA_BITS ));
	
	fprintf( stdout, "%sAccum RGBA: %d,%d,%d,%d\n", tab,
			glfwGetWindowParam( GLFW_ACCUM_RED_BITS ),
			glfwGetWindowParam( GLFW_ACCUM_GREEN_BITS ),
			glfwGetWindowParam( GLFW_ACCUM_BLUE_BITS ),
			glfwGetWindowParam( GLFW_ACCUM_ALPHA_BITS ) );
	
	fprintf( stdout, "%sDepth bits: %d\n", tab, glfwGetWindowParam( GLFW_DEPTH_BITS ) );
	// Number of bits for the depth buffer.
	
	fprintf( stdout,  "%sStencil bits: %d\n", tab, glfwGetWindowParam( GLFW_STENCIL_BITS ) );
	// Number of bits for the stencil buffer.
	
	fprintf( stdout,  "%sRefresh rate: %d\n", tab, glfwGetWindowParam( GLFW_REFRESH_RATE ) );
	// Vertical monitor refresh rate in Hz. Zero indicates an unknown or a default refresh rate.
	
	fprintf( stdout,  "%s# aux buffers: %d\n", tab, glfwGetWindowParam( GLFW_AUX_BUFFERS ) );
	// Number of auxiliary buffers.
	
	fprintf( stdout,  "%s%s\n", tab, (glfwGetWindowParam( GLFW_STEREO ) == GL_TRUE ? "Stereo": "Mono"));
	// GL_TRUE if stereo rendering is supported, else GL_FALSE.
	
	fprintf( stdout,  "%s# multisampling buffer samples: %d\n", tab, glfwGetWindowParam( GLFW_FSAA_SAMPLES ));
	// Number of multisampling buffer samples. Zero indicated multisampling is disabled.
	
	fprintf( stdout,  "%s%s\n", tab, (glfwGetWindowParam( GLFW_OPENGL_FORWARD_COMPAT ) == GL_TRUE ? "forward-compatible": "NOT forward-compatible"));
	// GL_TRUE if the context is forward-compatible, else GL_FALSE.
	
	int profile = glfwGetWindowParam( GLFW_OPENGL_PROFILE );
	// The profile implemented by the context, or zero.
	if( GLFW_OPENGL_CORE_PROFILE == profile ) {
		fprintf( stdout, "GLFW_OPENGL_CORE_PROFILE\n" );
	}
	else if( GLFW_OPENGL_COMPAT_PROFILE == profile ) {
		fprintf( stdout, "GLFW_OPENGL_COMPAT_PROFILE\n" );
	}
	else {
		fprintf( stdout, "Unknown PROFILE !!??\n" );
	}
	
}

void setOpenWindowHints() {
	// ** 2nd param: default value **
	/*
	 void glfwOpenWindowHint( GLFW_REFRESH_RATE, 0 );
	 // Vertical monitor refresh rate in Hz (only used for fullscreen windows).
	 
	 void glfwOpenWindowHint( GLFW_ACCUM_RED_BITS, 0 );
	 void glfwOpenWindowHint( GLFW_ACCUM_GREEN_BITS, 0 );
	 void glfwOpenWindowHint( GLFW_ACCUM_BLUE_BITS, 0 );
	 void glfwOpenWindowHint( GLFW_ACCUM_ALPHA_BITS, 0 );
	 // Number of bits for the red/green/blue/alpha channel of the ac- cumulation buffer.
	 
	 void glfwOpenWindowHint( GLFW_AUX_BUFFERS, 0 );
	 // Number of auxiliary buffers.
	 
	 void glfwOpenWindowHint( GLFW_STEREO, GL_FALSE );
	 // Specify if stereo rendering should be supported (can be GL_TRUE or GL_FALSE).
	 
	 void glfwOpenWindowHint( GLFW_WINDOW_NO_RESIZE, GL_FALSE );
	 // Specify whether the window can be resized by the user (not used for fullscreen windows).
	 
	 void glfwOpenWindowHint( GLFW_FSAA_SAMPLES, 0 );
	 // Number of samples to use for the multisam- pling buffer. Zero disables multisampling.
	 
	 void glfwOpenWindowHint( GLFW_OPENGL_VERSION_MAJOR, 1 );
	 // Major number of the desired minimum OpenGL version.
	 
	 void glfwOpenWindowHint( GLFW_OPENGL_VERSION_MINOR, 1 );
	 // Minor number of the desired minimum OpenGL version.
	 
	 void glfwOpenWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE );
	 //Specify whether the OpenGL context should be forward-compatible (i.e. disallow legacy functionality). This should only be used when requesting OpenGL version 3.0 or above.
	 
	 void glfwOpenWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE );
	 // Specify whether a debug context should be created.
	 
	 void glfwOpenWindowHint( GLFW_OPENGL_PROFILE, 0 );
	 // The OpenGL profile the context should implement, or zero to let the system choose. Available profiles are GLFW_OPENGL_CORE_PROFILE and GLFW_OPENGL_COMPAT_PROFILE.
	 */
}
