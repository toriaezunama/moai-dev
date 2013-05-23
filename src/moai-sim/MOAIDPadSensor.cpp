// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIDPadSensor.h>

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@name	setCallback
	@text	Sets or clears the callback to be issued when button events occur.

	@in		MOAIDPadSensor self
	@opt	function callback		Default value is nil.
	@out	nil
*/
int MOAIDPadSensor::_setCallback ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIDPadSensor, "U" )
	
	self->mCallback.SetStrongRef ( state, 2 );
	
	return 0;
}

//================================================================//
// MOAIDPadSensor
//================================================================//

//----------------------------------------------------------------//
/**	@name	getValue
	@text	Returns the current value of dpad

	@in		MOAIDPadSensor self
	@out	int value
*/
int MOAIDPadSensor::_getValue ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIDPadSensor, "U" )
	
	lua_pushinteger ( state, self->mValue );
	
	return 1;
}


//----------------------------------------------------------------//
void MOAIDPadSensor::HandleEvent ( ZLStream& eventStream ) {

	u32 value = eventStream.Read < u32 >( NEUTRAL );
		
	if ( this->mValue != value ) {
		this->mValue = value;
		MOAILuaStateHandle state = this->mCallback.GetSelf ();
		lua_pushinteger ( state, value );
		state.DebugCall ( 1, 0 );
	}
}

//----------------------------------------------------------------//
MOAIDPadSensor::MOAIDPadSensor () :
	mValue ( NEUTRAL ) {

	RTTI_SINGLE ( MOAISensor )
}

//----------------------------------------------------------------//
MOAIDPadSensor::~MOAIDPadSensor () {
}

//----------------------------------------------------------------//
void MOAIDPadSensor::RegisterLuaClass ( MOAILuaState& state ) {

	MOAISensor::RegisterLuaClass ( state );
}

//----------------------------------------------------------------//
void MOAIDPadSensor::RegisterLuaFuncs ( MOAILuaState& state ) {

	luaL_Reg regTable [] = {
		{ "getValue",			_getValue },
		{ "setCallback",		_setCallback },
		{ NULL, NULL }
	};

	luaL_register ( state, 0, regTable );
}

//----------------------------------------------------------------//
void MOAIDPadSensor::Reset () {

	// clear out the old events
	this->mValue = NEUTRAL;	
}

//----------------------------------------------------------------//
void MOAIDPadSensor::WriteEvent ( ZLStream& eventStream, u32 value ) {

	eventStream.Write < u32 >( value );
}
