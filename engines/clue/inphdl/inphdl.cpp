/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa
  Portions copyright (c) 2005 Jens Granseuer

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/


#include "common/system.h"
#include "common/events.h"
#include "graphics/cursorman.h"

#include "clue/planing/player.h"
#include "clue/base/base.h"
#include "clue/sound/newsound.h"

namespace Clue {

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

#define X 15
#define x 0
#define O 255

static const byte cursorSprite[] = {
	x, x, O, O, O, O, O, O, O, O, O,
	x, X, x, O, O, O, O, O, O, O, O,
	x, X, X, x, O, O, O, O, O, O, O,
	x, X, X, X, x, O, O, O, O, O, O,
	x, X, X, X, X, x, O, O, O, O, O,
	x, X, X, X, X, X, x, O, O, O, O,
	x, X, X, X, X, X, X, x, O, O, O,
	x, X, X, X, X, X, X, X, x, O, O,
	x, X, X, X, X, X, X, X, X, x, O,
	x, X, X, X, X, X, x, x, x, x, x,
	x, X, X, x, X, X, x, O, O, O, O,
	x, X, x, x, x, X, X, x, O, O, O,
	x, x, O, O, x, X, X, x, O, O, O,
	O, O, O, O, O, x, X, X, x, O, O,
	O, O, O, O, O, x, X, X, x, O, O,
	O, O, O, O, O, O, x, x, O, O, O
};

#undef X
#undef x
#undef O

void inpInitMouse() {
	CursorMan.replaceCursor(cursorSprite, 11, 16, 0, 0, 255);
	CursorMan.showMouse(true);
}

static void inpDoPseudoMultiTasking() {
	// 2014-06-30 LucyG : rewritten
	static uint32 timePrev = 0;
	uint32 timeNow = g_system->getMillis();
	if (!timePrev) {
		timePrev = timeNow;
	}
	uint32 timePassed = timeNow - timePrev;
	if (timePassed >= INP_TICKS_TO_MS(1)) {
		timePrev = timeNow;

		g_clue->_sndMgr->sndDoFading(); // 2014-07-17 LucyG

		g_clue->_animMgr->animator();
	}
}

void inpOpenAllInputDevs() {
	inpSetKeyRepeat((1 << 5) | 10);

	IHandler.EscStatus = true;
	IHandler.FunctionKeyStatus = true;

	IHandler.MouseExists = true;
	IHandler.MouseStatus = true;

	IHandler.JoyExists = false;

	inpInitMouse();
}

void inpMousePtrOn() {
	CursorMan.showMouse(true);
}

void inpMousePtrOff() {
	CursorMan.showMouse(false);
}

int32 inpWaitFor(int32 l_Mask) {
	if ((IHandler.EscStatus) && (!(l_Mask & INP_NO_ESC)))
		l_Mask |= INP_ESC;

	if ((IHandler.FunctionKeyStatus) && (!(l_Mask & INP_FUNCTION_KEY)))
		l_Mask |= INP_FUNCTION_KEY;

	int32 action = 0;

	/* Nun wird auf den Event gewartet... */
	uint32 WaitTime = 0;
	uint32 timePrev = g_system->getMillis();

	while (action == 0) {
		Common::Event ev;
		while (g_system->getEventManager()->pollEvent(ev)) {
			switch (ev.type) {
			case Common::EVENT_KEYDOWN: {
				switch (ev.kbd.keycode) {
				case Common::KEYCODE_LEFT:
					if ((l_Mask & INP_LEFT))
						action |= INP_KEYBOARD | INP_LEFT;
					break;

				case Common::KEYCODE_RIGHT:
					if ((l_Mask & INP_RIGHT))
						action |= INP_KEYBOARD | INP_RIGHT;
					break;

				case Common::KEYCODE_UP:
					if ((l_Mask & INP_UP))
						action |= INP_KEYBOARD | INP_UP;
					break;

				case Common::KEYCODE_DOWN:
					if ((l_Mask & INP_DOWN))
						action |= INP_KEYBOARD | INP_DOWN;
					break;

				case Common::KEYCODE_SPACE:
				case Common::KEYCODE_RETURN:
				case Common::KEYCODE_KP_ENTER:
					if ((l_Mask & (INP_LBUTTONP | INP_LBUTTONR)))
						action |= INP_KEYBOARD | INP_LBUTTONP;
					break;

				case Common::KEYCODE_ESCAPE:
					if (IHandler.EscStatus)
						action |= INP_KEYBOARD | INP_ESC;
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
						action |= INP_KEYBOARD | INP_FUNCTION_KEY;
					break;

				default:
					break;
				}
			}
			break;

			case Common::EVENT_MOUSEMOVE:
				if (IHandler.MouseExists && IHandler.MouseStatus) {
					if (l_Mask & INP_LEFT)
						action |= INP_MOUSE | INP_LEFT;
					if (l_Mask & INP_RIGHT)
						action |= INP_MOUSE | INP_RIGHT;
					if (l_Mask & INP_UP)
						action |= INP_MOUSE | INP_UP;
					if (l_Mask & INP_DOWN)
						action |= INP_MOUSE | INP_DOWN;
					/*
					if ((l_Mask & INP_LEFT) && (ev.motion.xrel < 0))
					action |= INP_MOUSE | INP_LEFT;
					if ((l_Mask & INP_RIGHT) && (ev.motion.xrel > 0))
					action |= INP_MOUSE | INP_RIGHT;
					if ((l_Mask & INP_UP) && (ev.motion.yrel < 0))
					action |= INP_MOUSE | INP_UP;
					if ((l_Mask & INP_DOWN) && (ev.motion.yrel > 0))
					action |= INP_MOUSE | INP_DOWN;
					*/
				}
				break;

			case Common::EVENT_LBUTTONDOWN:
				if (IHandler.MouseExists && IHandler.MouseStatus) {
					if (l_Mask & INP_LBUTTONP)
						action |= INP_MOUSE | INP_LBUTTONP;
				}
				break;
			case Common::EVENT_RBUTTONDOWN:
				if (IHandler.MouseExists && IHandler.MouseStatus) {
					if (l_Mask & INP_RBUTTONP)
						action |= INP_MOUSE | INP_RBUTTONP;
				}
				break;

			case Common::EVENT_QUIT:
				tcDone();
				g_system->quit(); // TODO: Hack
				break;

			default:
				break;
			}
		}

		inpDoPseudoMultiTasking();
		wfd();

		WaitTime = g_system->getMillis() - timePrev;
		if ((l_Mask & INP_TIME) && (WaitTime >= INP_TICKS_TO_MS(IHandler.ul_WaitTicks)))
			action |= INP_TIME;
	}

	return action;
}

void inpSetWaitTicks(uint32 l_Ticks) {
	// 2018-09-26 : timing fixes
	if (l_Ticks < INP_AS_FAST_AS_POSSIBLE) {
		l_Ticks = INP_AS_FAST_AS_POSSIBLE;
	}
	IHandler.ul_WaitTicks = l_Ticks;
}

void inpTurnESC(bool us_NewStatus) {
	IHandler.EscStatus = us_NewStatus;
}

void inpTurnFunctionKey(bool us_NewStatus) {
	IHandler.FunctionKeyStatus = us_NewStatus;
}

void inpTurnMouse(bool us_NewStatus) {
	// This is only used to hide the mouse in the planner,
	// doesn't seem to have any side-effects if we keep the mouse on
	/*
	IHandler.MouseStatus = us_NewStatus;
	CursorMan.showMouse(us_NewStatus);
	*/
}

void inpDelay(uint32 l_Ticks) {
	// 2014-07-03 LucyG : rewritten
	uint32 timePrev = g_system->getMillis();
	l_Ticks = INP_TICKS_TO_MS(l_Ticks);
	Common::Event ev;
	while ((g_system->getMillis() - timePrev) < l_Ticks) {
		while (g_system->getEventManager()->pollEvent(ev));
		wfr();
		inpDoPseudoMultiTasking();
	}
}

void inpSetKeyRepeat(unsigned char rate) {
#if 0
	int delay = (rate >> 5);
	int interval = (rate & 0x1f);

	if (delay == 0)
		delay = 1000;
	else
		delay = 750;

	if (interval == 0)
		interval = 30;
	else
		interval = 60;

	SDL_EnableKeyRepeat(delay, interval);

	/* flush event queue */
	Common::Event ev;
	while (g_system->getEventManager()->pollEvent(ev)) {
		/* do nothing. */
		g_system->delayMillis(10);
	}
#endif
}

} // End of namespace Clue
