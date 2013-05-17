#include "joystick.h"

// == Joystick ===========================================
Joystick::Joystick( int id, int axesCnt, int buttonCnt ) : 
	mIsConnected( false ),
	mId( id ),
	mAxesCnt( axesCnt ), 
	mButtonCnt( buttonCnt ),
	mCurrentBuffer( 0 )
{
	mAxes = new float[ mAxesCnt ];
	mButtons[0] = new unsigned char[ mButtonCnt ];
	mButtons[1] = new unsigned char[ mButtonCnt ];
}

Joystick::~Joystick() {
	if( NULL != mButtons ) {
		delete [] mAxes;
		delete [] mButtons[0];
		delete [] mButtons[1];
	}
}

void Joystick::update( JoystickAxisCallback axisCallback, JoystickButtonCallback buttonCallback ) {
	glfwGetJoystickPos( mId, mAxes, mAxesCnt );
	glfwGetJoystickButtons( mId, &mButtons[ mCurrentBuffer ][0], mButtonCnt );

	int prevBuffer = (mCurrentBuffer == 0) ? 1 : 0;
	for( int i = 0; i < mButtonCnt; i++ ) {
		unsigned char currButtonState = mButtons[ mCurrentBuffer][ i ];
		if( mButtons[ prevBuffer ][ i ] != currButtonState ) {
			if( buttonCallback )
				buttonCallback( mId, i, currButtonState );
		}
	}
	// Lump into pairs of axes
	for( int i = 0; i < mAxesCnt; i += 2 ) {
		float x = mAxes[ i ];
		float y = mAxes[ i + 1 ];
		
		// Left Shoulder button 1(off) -> -1(full)
		if( i == 4 ) {
//			if( v > -1 ) {
//				// Map to range 0 - 1
//				v = (v + 1) / 2;
//				// TODO: axisCallback( mId, i, v );
//			}
		}
		// Left Shoulder button 1(off) -> -1(full)
		else if( i == 5 ) {
//			// Map to range 0 - 1
//			if( v < 1 ) {
//				v = 1 - ((v + 1) / 2);
//				// TODO: axisCallback( mId, i, v );
//			}
		}
		// Sticks		
		else {
			float deadZone = 0.2;
			bool isX = (x < -deadZone) || (x > deadZone);
			bool isY = (y < -deadZone) || (y > deadZone);
			if( (isX or isY) and axisCallback ) {
				x = isX ? x : 0;
				y = isY ? y : 0;
				axisCallback( mId, i/2, x, y ); //i/2 so left stick = 0, right stick =1, triggers =2
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
		newJS->mIsConnected = isJoystickConnected( id );
	}
}

void JoystickManager::update() {
	for( int i = 0; i < MAX_JOYSTICKS; i++ ) {
		Joystick* j = mJoysticks[ i ];
		if( j->mIsConnected )
			j->update( mAxesCallback, mButtonCallback );
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
