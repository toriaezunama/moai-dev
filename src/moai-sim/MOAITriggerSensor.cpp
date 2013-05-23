// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAITriggerSensor.h>


//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@name	getValue
	@text	Returns the current value of a trigger

	@in		MOAITriggerSensor self
	@out	number value
*/
int MOAITriggerSensor::_getValue ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAITriggerSensor, "U" )
	
	lua_pushnumber ( state, self->mValue );
	
	return 1;
}

//----------------------------------------------------------------//
/**	@name	setCallback
	@text	Sets or clears the callback to be issued on a trigger event

	@in		MOAITriggerSensor self
	@opt	function callback		Default value is nil.
	@out	nil
*/
int MOAITriggerSensor::_setCallback ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAITriggerSensor, "U" )
	
	self->mCallback.SetStrongRef ( state, 2 );
	
	return 0;
}

//================================================================//
// MOAITriggerSensor
//================================================================//

//----------------------------------------------------------------//
void MOAITriggerSensor::HandleEvent ( ZLStream& eventStream ) {
	
	this->mValue = eventStream.Read < float >( 0.0f );
	
	if ( this->mCallback ) {
		MOAILuaStateHandle state = this->mCallback.GetSelf ();
		lua_pushnumber ( state, this->mValue );
		state.DebugCall ( 1, 0 );
	}
}

//----------------------------------------------------------------//
MOAITriggerSensor::MOAITriggerSensor () :
	mValue ( 0 ) {
	RTTI_SINGLE ( MOAISensor )
}

//----------------------------------------------------------------//
MOAITriggerSensor::~MOAITriggerSensor () {
}

//----------------------------------------------------------------//
void MOAITriggerSensor::RegisterLuaClass ( MOAILuaState& state ) {

	MOAISensor::RegisterLuaClass ( state );
}

//----------------------------------------------------------------//
void MOAITriggerSensor::RegisterLuaFuncs ( MOAILuaState& state ) {

	luaL_Reg regTable [] = {
		{ "getValue",				_getValue },
		{ "setCallback",			_setCallback },
		{ NULL, NULL }
	};

	luaL_register ( state, 0, regTable );
}

//----------------------------------------------------------------//
void MOAITriggerSensor::Reset () {
}

//----------------------------------------------------------------//
void MOAITriggerSensor::WriteEvent ( ZLStream& eventStream, float value ) {

	eventStream.Write < float >( value );
}
