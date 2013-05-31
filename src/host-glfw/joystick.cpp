#include "joystick.h"
#include <string.h>

// == Joystick ===========================================
Joystick::Joystick( int id, int axesCnt, int buttonCnt ) : 
	mIsConnected( false ),
	mId( id ),
	mAxesCnt( axesCnt ), 
	mButtonCnt( buttonCnt ),
	mCurrentBuffer( 0 )
{
	mAxes[0] = new float[ mAxesCnt ];
	mAxes[1] = new float[ mAxesCnt ];
	memset( mAxes[0], 0, sizeof( float ) * mAxesCnt );
	memset( mAxes[1], 0, sizeof( float ) * mAxesCnt );

	mButtons[0] = new unsigned char[ mButtonCnt ];
	mButtons[1] = new unsigned char[ mButtonCnt ];
	memset( mButtons[0], 0, sizeof( unsigned char ) * mButtonCnt );
	memset( mButtons[1], 0, sizeof( unsigned char ) * mButtonCnt );
}

Joystick::~Joystick() {
	if( NULL != mButtons ) {
		delete [] mAxes[0];
		delete [] mAxes[1];
		delete [] mButtons[0];
		delete [] mButtons[1];
	}
}

void Joystick::update( JoystickAxisCallback axisCallback, JoystickButtonCallback buttonCallback, JoystickDPadCallback dpadCallback ) {
	glfwGetJoystickPos( mId, mAxes[ mCurrentBuffer ], mAxesCnt );
	glfwGetJoystickButtons( mId, &mButtons[ mCurrentBuffer ][0], mButtonCnt );

	int prevBuffer = (mCurrentBuffer == 0) ? 1 : 0;
	
	// DPad is buttons 0-3
	unsigned int pstate = 0;
	unsigned int state = 0;

	// UP
	state  |= mButtons[ mCurrentBuffer][ 0 ] == 0 ? 0 : 0x1;
	pstate |= mButtons[ prevBuffer ][ 0 ] == 0 ? 0 : 0x1;

	// DOWN
	state  |= mButtons[ mCurrentBuffer][ 1 ] == 0 ? 0 : 0x2;
	pstate |= mButtons[ prevBuffer ][ 1 ] == 0 ? 0 : 0x2;

	// LEFT
	state  |= mButtons[ mCurrentBuffer][ 2 ] == 0 ? 0 : 0x4;
	pstate |= mButtons[ prevBuffer ][ 2 ] == 0 ? 0 : 0x4;

	// RIGHT
	state  |= mButtons[ mCurrentBuffer][ 3 ] == 0 ? 0 : 0x8;
	pstate |= mButtons[ prevBuffer ][ 3 ] == 0 ? 0 : 0x8;

	// Neutral
	if( state == 0 && pstate != 0 ) {
		state = 0x16;
		pstate = 0; // force update
	}
	
	if( state != pstate )
		if( dpadCallback )
			dpadCallback( mId, 0, state );

	// Rest of buttons
	for( int i = 4; i < mButtonCnt; i++ ) {
		unsigned char currButtonState = mButtons[ mCurrentBuffer][ i ];
		if( mButtons[ prevBuffer ][ i ] != currButtonState ) {
			if( buttonCallback )
				buttonCallback( mId, i, currButtonState );
		}
	}
	// Lump into pairs of axes
	for( int i = 0; i < mAxesCnt; i += 2 ) {
		float x = mAxes[ mCurrentBuffer ][ i ];
		float y = mAxes[ mCurrentBuffer ][ i + 1 ];

		// Left/right Shoulder triggers 1(off) -> -1(full)
		if( i == 4 ) {
			// Map to range 0 - 1
			x = (x + 1) / 2;
			y = 1 - ((y + 1) / 2);
			axisCallback( mId, 2, x, y );
		}
		// Sticks
		else {
			float deadZone = 0.2;
			bool isX = (x < -deadZone) || (x > deadZone);
			bool isY = (y < -deadZone) || (y > deadZone);
			if( isX or isY ) {
				if( axisCallback ) {
					x = isX ? x : 0;
					y = isY ? y : 0;
					axisCallback( mId, i/2, x, y ); //i/2 so left stick = 0, right stick =1, triggers =2
				}
			}
			else {
				float px = mAxes[ prevBuffer ][ i ];
				float py = mAxes[ prevBuffer ][ i + 1 ];
				bool isPX = (px < -deadZone) || (px > deadZone);
				bool isPY = (py < -deadZone) || (py > deadZone);

				// Send a callback when joystick returns to deadzone for the first time
				if( isPX or isPY ) {
					if( axisCallback ) {
						axisCallback( mId, i/2, 0, 0 );
					}					
				}
			}
		}
	}

	mCurrentBuffer = prevBuffer;
}

void Joystick::dump() {
	printf( "[ %d ]%s %d axes, %d buttons\n", 
		mId, 
		mIsConnected ? "+" : "-",
		mAxesCnt,
		mButtonCnt
	);
}

// == Manager ===========================================
void JoystickManager::init() {
	for( int i = 0; i < MAX_JOYSTICKS; i++ ) {
		int id = GLFW_JOYSTICK_1 + i;
		int axes = glfwGetJoystickParam( id, GLFW_AXES );
	    int buttons = glfwGetJoystickParam( id, GLFW_BUTTONS );
		Joystick* newJS = new Joystick( id, axes, buttons );
		mJoysticks[ i ] = newJS;
	}
}

void JoystickManager::update() {
	for( int i = 0; i < MAX_JOYSTICKS; i++ ) {
		Joystick* j = mJoysticks[ i ];
		bool connected = isJoystickConnected( j->mId );
		
		// Connection state has changed
		if( j->mIsConnected != connected )
			if( mConnectionCallback )
				mConnectionCallback( j->mId, connected );

		j->mIsConnected = connected;
		
		if( j->mIsConnected )
			j->update( mAxesCallback, mButtonCallback, mDPadCallback );
	}	
}

void JoystickManager::dump() {
	for( int i = 0; i < MAX_JOYSTICKS; i++ ) {
		mJoysticks[ i ]->dump();
		printf( "---------------------\n" );
	}	
}

void JoystickManager::setAxisCallback( JoystickAxisCallback callback ) {
	mAxesCallback = callback;
}

void JoystickManager::setButtonCallback( JoystickButtonCallback callback ) {
	mButtonCallback = callback;
}

void JoystickManager::setDPadCallback( JoystickDPadCallback callback ) {
	mDPadCallback = callback;
}

void JoystickManager::setConnectionCallback( JoystickConnectionCallback callback ) {
	mConnectionCallback = callback;
}

JoystickManager::~JoystickManager() {
	for( int i = 0; i < MAX_JOYSTICKS; i++ ) {
		if( NULL != mJoysticks[ i ] )	
			delete mJoysticks[ i ];
	}
}


/*
	void JoystickInfo( int joystickNo ) {
	    // == GLFW_JOYSTICK_1 GLFW_JOYSTICK_16 -> 1 - 16
	    assert( joystickNo >= GLFW_JOYSTICK_1 && joystickNo <= GLFW_JOYSTICK_16 );

	    if( GL_TRUE == glfwGetJoystickParam( joystickNo, GLFW_PRESENT ) ) {

	        // D-pads are exposed as a set of four buttons and Analog sticks are as two axes.        
	        int axes = glfwGetJoystickParam( GLFW_JOYSTICK_1, GLFW_AXES );
	        int buttons = glfwGetJoystickParam( GLFW_JOYSTICK_1, GLFW_BUTTONS );

	        printf( "Joystick1 attached: %d axes, %d buttons", axes, buttons );        
	    }
	}*/
