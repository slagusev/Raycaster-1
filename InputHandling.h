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

void InitRawInputDevice(HWND hWnd, USHORT Device);
void UnregisterRawInputDevice(USHORT Device);
void IncreaseMouseSensitivity();
void DecreaseMouseSensitivity();

//
// Variables and constants
//

// Init device identifier
const USHORT HID_MOUSE = 2;
const USHORT HID_KEYBOARD = 6;

// dwSize is 48 for 64bit systems, and 40 for 32bit systems
uint_fast32_t dwSize = 48;
LPBYTE lpb = new BYTE[dwSize];

RAWINPUT* RawDev = (RAWINPUT*)lpb;

RECT WindowRect;

std::wstring MouseSensitivityPrint;

// POINT structs for storing mouse coordinates
POINT MousePos;
POINT OldMousePos;

//
// Functions
//

void InitRawInputDevice(HWND hWnd, USHORT Device)
{
	RAWINPUTDEVICE RawInputDevice;

	// Define RAWINPUTDEVICE
	RawInputDevice.usUsagePage	= 1;
	RawInputDevice.usUsage		= Device;
	RawInputDevice.dwFlags		= RIDEV_DEVNOTIFY | RIDEV_INPUTSINK;
	RawInputDevice.hwndTarget	= hWnd;

	RegisterRawInputDevices(&RawInputDevice, 1, sizeof(RawInputDevice));

	if (Device == HID_MOUSE)
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

		// Initial conversion of MouseSensitivity to char (for HUD display)
		MouseSensitivityPrint = std::to_wstring(MouseSensitivity);
	}
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