#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#include <GL/glfw.h>
#include <stdio.h>

class JoystickManager;

typedef void (*JoystickAxisCallback)( int joystickId, int stickId, float x, float y );
typedef void (*JoystickButtonCallback)( int joystickId, int buttonId, int state );
typedef void (*JoystickDPadCallback)( int joystickId, int dpadId, int state );
typedef void (*JoystickConnectionCallback)( int joystickId, bool connected );

class Joystick {
	bool mIsConnected;
	int mId;
	int mAxesCnt;
	int mButtonCnt;
	unsigned char* mButtons[2];
	float* mAxes[2];
	int mCurrentBuffer;

public:
	Joystick( int id, int axesCnt, int buttonCnt );
	~Joystick();

	void update( JoystickAxisCallback axisCallback, JoystickButtonCallback buttonCallback, JoystickDPadCallback dpadCallback );
	void dump();


 	friend class JoystickManager;
};


class JoystickManager {
	static const int MAX_JOYSTICKS = 4;

	Joystick* mJoysticks[ GLFW_JOYSTICK_16 ];
	
	JoystickAxisCallback mAxesCallback;
	JoystickButtonCallback mButtonCallback;
	JoystickDPadCallback mDPadCallback;
	JoystickConnectionCallback mConnectionCallback;

public:
	JoystickManager() {};
	~JoystickManager();

	void init();
	void update();
	void dump();
	void setAxisCallback( JoystickAxisCallback );
	void setButtonCallback( JoystickButtonCallback );
	void setDPadCallback( JoystickDPadCallback );
	void setConnectionCallback( JoystickConnectionCallback );

private:
	inline bool isJoystickConnected( int joystickNo ) {
		return glfwGetJoystickParam( joystickNo, GLFW_PRESENT ) == GL_TRUE ? true : false;
	}
};

#endif // _JOYSTICK_H_