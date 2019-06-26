/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa
  Portions copyright (c) 2005 Jens Granseuer

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "clue/inphdl/inphdl.h"

#include "common/system.h"
#include "common/events.h"

struct IHandler {
    int32 ul_XSensitivity;
    int32 ul_YSensitivity;
    uint32 ul_WaitTicks;

    /* us_KeyCode; */

    bool MouseExists;
    bool EscStatus;
    bool FunctionKeyStatus;
    bool MouseStatus;

    bool JoyExists;
    /* SDL_Joystick *Joystick; */
};

struct IHandler IHandler;


void gfxWaitTOF(void);


static void inpDoPseudoMultiTasking(void)
{
    animator();
}

void inpOpenAllInputDevs(void)
{
    inpSetKeyRepeat((1 << 5) | 10);

    IHandler.EscStatus = true;
    IHandler.FunctionKeyStatus = true;

    IHandler.MouseExists = true;
    IHandler.MouseStatus = true;

    IHandler.JoyExists = false;
/*
    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) == 0) {
        if (SDL_NumJoysticks() > 0) {
            IHandler.Joystick = SDL_JoystickOpen(0);

            if (IHandler.Joystick) {
                IHandler.JoyExists = true;
            } else {
                DebugMsg(ERR_DEBUG, ERROR_MODULE_INPUT,
                         "Failed to open Joystick 0");
            }
        }
    } else {
        DebugMsg(ERR_DEBUG, ERROR_MODULE_INPUT,
                 "SDL_InitSubSystem: %s", SDL_GetError());
    }
*/
    inpClearKbBuffer();
}

void inpCloseAllInputDevs(void)
{
    /*
    if (SDL_JoystickOpened(0)) {
        SDL_JoystickClose(IHandler.Joystick);
    }

    if (SDL_WasInit(SDL_INIT_JOYSTICK) != 0) {
        SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
    }
    */
}

void inpMousePtrOn(void)
{
/*      SDL_ShowCursor(SDL_ENABLE); */
}

void inpMousePtrOff(void)
{
/*      SDL_ShowCursor(SDL_DISABLE); */
}

int32 inpWaitFor(int32 l_Mask)
{
    Common::Event ev;
    int32 action;
    uint32 WaitTime = 0;

    if ((IHandler.EscStatus) && (!(l_Mask & INP_NO_ESC))) {
	l_Mask |= INP_ESC;
    }

    if ((IHandler.FunctionKeyStatus) && (!(l_Mask & INP_FUNCTION_KEY))) {
	l_Mask |= INP_FUNCTION_KEY;
    }

    action = 0;

    /* Nun wird auf den Event gewartet... */
    WaitTime = 0;

    while (action == 0) {
	gfxWaitTOF();

	WaitTime++;
	/* Abfrage des Zeit-Flags */
	if ((l_Mask & INP_TIME) && WaitTime >= IHandler.ul_WaitTicks) {
	    action |= INP_TIME;
        }

	while (g_system->getEventManager()->pollEvent(ev)) {
	    switch (ev.type) {
		case Common::EVENT_KEYDOWN:
		{
		    switch (ev.kbd.keycode) {
		    case Common::KEYCODE_LEFT:
			if ((l_Mask & INP_LEFT))
			    action |= INP_KEYBOARD + INP_LEFT;
			break;

		    case Common::KEYCODE_RIGHT:
			if ((l_Mask & INP_RIGHT))
			    action |= INP_KEYBOARD + INP_RIGHT;
			break;

		    case Common::KEYCODE_UP:
			if ((l_Mask & INP_UP))
			    action |= INP_KEYBOARD + INP_UP;
			break;

		    case Common::KEYCODE_DOWN:
			if ((l_Mask & INP_DOWN))
			    action |= INP_KEYBOARD + INP_DOWN;
			break;

		    case Common::KEYCODE_SPACE:
		    case Common::KEYCODE_RETURN:
		    case Common::KEYCODE_KP_ENTER:
			if ((l_Mask & (INP_LBUTTONP | INP_LBUTTONR)))
			    action |= INP_KEYBOARD + INP_LBUTTONP;
			break;

		    case Common::KEYCODE_ESCAPE:
			if (IHandler.EscStatus)
			    action |= INP_KEYBOARD + INP_ESC;
			break;

		    case Common::KEYCODE_F1:
		    case Common::KEYCODE_F2:
		    case Common::KEYCODE_F3:
		    case Common::KEYCODE_F4:
		    case Common::KEYCODE_F5:
		    case Common::KEYCODE_F6:
		    case Common::KEYCODE_F7:
		    case Common::KEYCODE_F8:
		    case Common::KEYCODE_F9:
		    case Common::KEYCODE_F10:
		    case Common::KEYCODE_F11:
		    case Common::KEYCODE_F12:
		    case Common::KEYCODE_F13:
		    case Common::KEYCODE_F14:
		    case Common::KEYCODE_F15:
			if (IHandler.FunctionKeyStatus)
			    action |= INP_KEYBOARD + INP_FUNCTION_KEY;
			break;

		    default:
			break;
		    }
		}
		break;

		case Common::EVENT_MOUSEMOVE:
		if (IHandler.MouseExists && IHandler.MouseStatus) {/*
		    if ((l_Mask & INP_LEFT) && (ev.motion.xrel < 0))
			action |= INP_MOUSE + INP_LEFT;
		    if ((l_Mask & INP_RIGHT) && (ev.motion.xrel > 0))
			action |= INP_MOUSE + INP_RIGHT;
		    if ((l_Mask & INP_UP) && (ev.motion.yrel < 0))
			action |= INP_MOUSE + INP_UP;
		    if ((l_Mask & INP_DOWN) && (ev.motion.yrel > 0))
			action |= INP_MOUSE + INP_DOWN;
		*/}
		break;

		case Common::EVENT_LBUTTONDOWN:
		if (IHandler.MouseExists && IHandler.MouseStatus) {
			action |= INP_MOUSE + INP_LBUTTONP;
		}
		break;
		case Common::EVENT_RBUTTONDOWN:
		if (IHandler.MouseExists && IHandler.MouseStatus) {
			action |= INP_MOUSE + INP_RBUTTONP;
		}
		break;

		case Common::EVENT_QUIT:
			g_system->quit(); // TODO: Hack
		break;

    /*
	    case SDL_JOYBUTTONDOWN:
		if (IHandler.JoyExists) {
		    switch (ev.jbutton.button) {
		    case 0:
			action |= INP_MOUSE + INP_LBUTTONP;
			break;

		    case 1:
			action |= INP_MOUSE + INP_RBUTTONP;
			break;

		    default:
			break;
		    }
		}
		break;
    */
	    default:
		break;
	    }
	}

        inpClearKbBuffer();
	inpDoPseudoMultiTasking();
    }

    return action;
}

void inpSetWaitTicks(uint32 l_Ticks)
{
    IHandler.ul_WaitTicks = l_Ticks;
}

void inpTurnESC(bool us_NewStatus)
{
    IHandler.EscStatus = us_NewStatus;
}

void inpTurnFunctionKey(bool us_NewStatus)
{
    IHandler.FunctionKeyStatus = us_NewStatus;
}

void inpTurnMouse(bool us_NewStatus)
{
    IHandler.MouseStatus = us_NewStatus;
}

void inpDelay(int32 l_Ticks)
{
    int32 i;

    for (i = 0; i < l_Ticks; i++) {
	gfxWaitTOF();
	inpDoPseudoMultiTasking();
    }
}

void inpSetKeyRepeat(unsigned char rate)
{
    int delay, interval;
	Common::Event ev;

    delay = (rate >> 5);
    interval = (rate & 0x1f);

    if (delay == 0)
	delay = 1000;
    else
	delay = 750;

    if (interval == 0)
	interval = 30;
    else
	interval = 60;

/*    SDL_EnableKeyRepeat(delay, interval);*/

    /* flush event queue */
    while (g_system->getEventManager()->pollEvent(ev)) {
        /* do nothing. */
    }
}

void inpClearKbBuffer(void)
{
}
