// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIDPADSENSOR_H
#define MOAIDPADSENSOR_H

#include <moai-sim/MOAISensor.h>

//================================================================//
// MOAIDPadSensor
//================================================================//
/**	@name	MOAIDPadSensor
	@text	Button sensor.
*/
class MOAIDPadSensor :
	public MOAISensor {

private:

	u32	mValue;

	MOAILuaRef	mCallback;

	//----------------------------------------------------------------//
	static int		_getValue		( lua_State* L );
	static int		_setCallback		( lua_State* L );

public:
	enum {
		NEUTRAL 		= 0,
		UP  			= 0x1,
		DOWN 			= 0x2,
		LEFT 			= 0x4,
		RIGHT 		= 0x8
	};

	DECL_LUA_FACTORY ( MOAIDPadSensor )

	//----------------------------------------------------------------//
	void			HandleEvent				( ZLStream& eventStream );
					MOAIDPadSensor		();
					~MOAIDPadSensor		();
	void			RegisterLuaClass		( MOAILuaState& state );
	void			RegisterLuaFuncs		( MOAILuaState& state );
	void			Reset					();
	static void		WriteEvent				( ZLStream& eventStream, u32 value );
};

#endif
