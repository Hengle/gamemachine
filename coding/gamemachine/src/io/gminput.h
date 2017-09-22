﻿#ifndef __GMINPUT_H__
#define __GMINPUT_H__
#include <gmcommon.h>

#if _MSC_VER
#	include <Xinput.h>
#else
#	include <xinput_gm.h>
#endif

BEGIN_NS
struct IWindow;
struct GMJoystickState
{
	bool valid;
	WORD buttons;
	GMbyte leftTrigger;
	GMbyte rightTrigger;
	GMshort thumbLX;
	GMshort thumbLY;
	GMshort thumbRX;
	GMshort thumbRY;
};

struct GMMouseState
{
	GMint deltaX;
	GMint deltaY;
	GMint posX;
	GMint posY;
};

GM_INTERFACE(IJoystickState)
{
	virtual void joystickVibrate(GMushort leftMotorSpeed, GMushort rightMotorSpeed) = 0;
	virtual GMJoystickState joystickState() = 0;
};

GM_INTERFACE(IKeyboardState)
{
	virtual bool keydown(GMuint key) = 0;
	virtual bool keyTriggered(GMuint key) = 0;
	virtual void setIMEState(bool enabled) = 0;
};

GM_INTERFACE(IMouseState)
{
	virtual GMMouseState mouseState() = 0;
	virtual void setMouseEnable(bool enable) = 0;
};

GM_INTERFACE(IInput)
{
	virtual void update() = 0;
	virtual IKeyboardState& getKeyboardState() = 0;
	virtual IJoystickState& getJoystickState() = 0;
	virtual IMouseState& getMouseState() = 0;
};
END_NS
#endif