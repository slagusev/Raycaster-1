/*
*****************************************
* Various functions for Input handling  *
*                                       *
* InputHandling.h                       *
*                                       *
* (c) 2017, 2018 Stefan Kubsch          *
*****************************************
*/

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

//
// Declare functions
//

void InitRawInputDevice(HWND hWnd);
void GetMousePosition();
void IncreaseMouseSensitivity();
void DecreaseMouseSensitivity();

//
// Variables and constants
//

// dwSize is 48 for 64bit, and 40 for 32bit
uint_fast32_t dwSize = 48;
static BYTE lpb[40];

RAWINPUTDEVICE RawInputDevice[1];
RAWINPUT* RawDev = (RAWINPUT*)lpb;
RECT WindowRect;

std::wstring MouseSensitivityPrint ;

// POINT structs for storing mouse coordinates
POINT MousePos;
POINT OldMousePos;

//
// Functions
//

void InitRawInputDevice(HWND hWnd)
{
	// Define RAWINPUTDEVICE
	RawInputDevice[0].usUsagePage	= HID_USAGE_PAGE_GENERIC;
	RawInputDevice[0].usUsage		= HID_USAGE_GENERIC_MOUSE;
	RawInputDevice[0].dwFlags		= RIDEV_INPUTSINK;
	RawInputDevice[0].hwndTarget	= hWnd;
	RegisterRawInputDevices(RawInputDevice, 1, sizeof(RawInputDevice[0]));

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

	// Initial conversion of MouseSensitivity to char (for HUD display)
	MouseSensitivityPrint = std::to_wstring(MouseSensitivity);
}

void GetMousePosition()
{
	if (RawDev->header.dwType == RIM_TYPEMOUSE)
	{
		MousePos.x = RawDev->data.mouse.lLastX;
		MousePos.y = RawDev->data.mouse.lLastY;
	}
}

void IncreaseMouseSensitivity()
{
	MouseSensitivity += 0.01f;
	MouseSensitivityPrint = std::to_wstring(MouseSensitivity);
}

void DecreaseMouseSensitivity()
{
	MouseSensitivity -= 0.01f;
	MouseSensitivityPrint = std::to_wstring(MouseSensitivity);
}