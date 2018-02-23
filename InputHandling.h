/*
*****************************************
* Various functions for Input handling  *
*                                       *
* InputHandling.h                       *
*                                       *
* (c) 2017, 2018 Stefan Kubsch          *
*****************************************
*/

//
// Declare functions
//

void RegisterRawInputDevice(HWND hWnd, USHORT Device);
void UnregisterRawInputDevice(USHORT Device);
void CatchMouse(HWND hWnd);

//
// Variables and constants
//

// Init device identifier
const USHORT HID_MOUSE = 2;
const USHORT HID_KEYBOARD = 6;

// WindowRect is used to catch mouse in window
RECT WindowRect;

// POINT structs for storing mouse coordinates
POINT MousePos;
POINT OldMousePos;

//
// Functions
//

void RegisterRawInputDevice(HWND hWnd, USHORT Device)
{
	RAWINPUTDEVICE RawInputDevice;

	// Define RAWINPUTDEVICE
	RawInputDevice.usUsagePage	= 1;
	RawInputDevice.usUsage		= Device;
	RawInputDevice.dwFlags		= RIDEV_DEVNOTIFY;
	RawInputDevice.hwndTarget	= hWnd;

	RegisterRawInputDevices(&RawInputDevice, 1, sizeof(RawInputDevice));
}

void UnregisterRawInputDevice(USHORT Device)
{
	RAWINPUTDEVICE RawInputDevice;

	RawInputDevice.usUsagePage = 1;
	RawInputDevice.usUsage = Device;
	RawInputDevice.dwFlags = RIDEV_REMOVE;
	RawInputDevice.hwndTarget = NULL;

	RegisterRawInputDevices(&RawInputDevice, 1, sizeof(RAWINPUTDEVICE));
}

void CatchMouse(HWND hWnd)
{
	// Get focus
	SetCapture(hWnd);

	// Hide mouse cursor
	ShowCursor(FALSE);

	// Get & set window boundaries (used for ClipCursor)
	GetClientRect(hWnd, &WindowRect);

	POINT UpperLeft;
	UpperLeft.x = WindowRect.left;
	UpperLeft.y = WindowRect.top;

	POINT LowerRight;
	LowerRight.x = WindowRect.right;
	LowerRight.y = WindowRect.bottom;

	MapWindowPoints(hWnd, nullptr, &UpperLeft, 1);
	MapWindowPoints(hWnd, nullptr, &LowerRight, 1);

	WindowRect.left = UpperLeft.x;
	WindowRect.top = UpperLeft.y;

	WindowRect.right = LowerRight.x;
	WindowRect.bottom = LowerRight.y;

	// "catch" mousepointer in window
	ClipCursor(&WindowRect);
}