/*
************************************
* Raycasting Game Engine           *
*                                  *
* Raycaster.cpp                    *
*                                  *
* (c) 2017, 2018 Stefan Kubsch     *
*                                  *
************************************
*/

#define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE

#define _USE_MATH_DEFINES
#define FLOAT_PI		3.14159265358979f
#define FLOAT_RAD2DEG	FLOAT_PI / 180.0f

#include <winsock2.h>
#include <boost/asio.hpp>
#include <windows.h>
#include <boost/thread.hpp>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Winmm.lib")

// LockBits.h provides fast pixel get/set operations
#include "LockBits.h"

// DataStructures.h contains all data structures used for entities, weapons etc.
#include "DataStructures.h"

// GlobalDefinitions.h contains all globally used variables and constants
#include "GlobalDefinitions.h"

// GameConfig.h contains functions for general configuration of the engine
#include "GameConfig.h"

// InputHandling.h provides various functions for mouse handling etc.
#include "InputHandling.h"

// LevelHandling.h provides various functions for Level handling
#include "LevelHandling.h"

// PlayerHandling.h provides various functions for entity handling
#include "PlayerHandling.h"

// EntityHandling.h provides various functions for entity handling
#include "EntityHandling.h"

// WeaponHandling.h provides various functions for weapon handling
#include "WeaponHandling.h"

// AudioHandling.h provides various functions audio handling
#include "AudioHandling.h"

//
// Declare functions
//

void CastGraphics(char RenderPart);
void RenderEntities();
void FPSCounter();
void DisplayHUD();
void DisplayMinimap();
void DrawPlayerWeapon();
void ControlPlayerMovement(HWND hWnd);

// Init GDI+,Buffer and other GDI+ objects for global use
static Gdiplus::GdiplusStartupInput gdiplusStartupInput;
static ULONG_PTR gdiplusToken;

static void InitGdiPlus()
{
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

// force GDI+ init
static bool unused = (InitGdiPlus(), false);

// Load general game config
// Small hack to run the function outside code body
namespace
{
	InitGameConfig InitGame;
}

// create bitmap and device for double buffer
Gdiplus::Bitmap BufferBMP(WindowViewPortWidth, WindowViewPortHeight, PixelFormat32bppPARGB);
Gdiplus::Graphics Buffer(&BufferBMP);
LockBits BufferLB(BufferBMP);

// define brush and font for displayed text
Gdiplus::SolidBrush HUDBrush(Gdiplus::Color(128, 255, 165, 0));
Gdiplus::Font HUDFont(L"Arial", 9);

// define brush for Minimap
Gdiplus::SolidBrush MinimapWallBrush(Gdiplus::Color(128, 255, 165, 0));
Gdiplus::SolidBrush MinimapPlayerBrush(Gdiplus::Color(255, 0, 255, 0));
Gdiplus::SolidBrush MinimapEnemyBrush(Gdiplus::Color(255, 255, 0, 0));

// Load crosshair bitmap
Gdiplus::Bitmap CrosshairIMG(CrosshairFileName.c_str(), PixelFormat32bppPARGB);

// Create cached bitmap for crosshair
Gdiplus::CachedBitmap Crosshair(&CrosshairIMG, &Buffer);

// MessageHandler
LRESULT CALLBACK WndProc(HWND, uint_fast32_t, WPARAM, LPARAM);

int_fast32_t WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR szCmdLine,int_fast32_t iCmdShow) 
{
	HWND				hWnd;
	MSG					msg;
	WNDCLASS			wc;
	
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance		= hInstance;
	wc.lpfnWndProc		= WndProc;
	wc.lpszClassName	= TEXT(WindowName.c_str());
	wc.lpszMenuName		= 0;
	wc.style			= CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wc);
		
	hWnd = CreateWindow(TEXT(WindowName.c_str()), TEXT(WindowName.c_str()), WS_OVERLAPPEDWINDOW, WindowPosX, WindowPosY, WindowOverallWidth, WindowOverallHeight, NULL, NULL, hInstance, NULL);

	// Init of level
	InitLevelConfig(SelectedLevel);
	InitPlayerConfig(SelectedLevel);
	InitLevelMapData(SelectedLevel);
	InitLevelTextures(SelectedLevel);

	// Init of player
	InitPlayerConfig(SelectedLevel);

	// Init of entities
	InitEntityConfig(SelectedLevel);
	InitEntityTextures(SelectedLevel);

	// Init of weapons
	InitWeaponConfig(SelectedLevel);
	InitWeaponTextures(SelectedLevel);
	InitWeaponMuzzleFlashTextures(SelectedLevel);

	// Init of audio
	InitLevelBackgroundMusic(SelectedLevel);
	InitPlayerAudio(SelectedLevel);
	InitWeaponAudio(SelectedLevel);

	// crosshair settings
	CrosshairPosX = WindowWidthMid - (CrosshairIMG.GetWidth() / 2);
	CrosshairPosY = WindowHeightMid - (CrosshairIMG.GetHeight() / 2);

	// Set initial position of player on EntityMap
	EntityMap[static_cast<uint_fast32_t>(PlayerPosX)][static_cast<uint_fast32_t>(PlayerPosY)] = EntityMapPlayerPos;

	// Seed random generator
	srand(uint_fast32_t(time(0)));

	// Start music
	PlayLevelBackgroundMusic();

	// Display window
	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	// Init device for raw input (mouse handling and keyboard)
	InitRawInputDevice(hWnd, HID_MOUSE);
	InitRawInputDevice(hWnd, HID_KEYBOARD);

	// Create device context (DC)
	PAINTSTRUCT	ps;
	HDC	hdc = BeginPaint(hWnd, &ps);

	// Create GDI DC for later BitBlt use
	HDC hdcMem = CreateCompatibleDC(hdc);

	// Create group for multithreading
	boost::thread_group GameLoopGroup;

	// Get maximum number of concurrent threads
	size_t NumberOfThreads = boost::thread::hardware_concurrency();

	// Main game loop
	// fixed timestep method
	// loop until ESC is pressed
	
	uint_fast32_t NextTick = GetTickCount();
	uint_fast32_t Loops;
	uint_fast32_t LengthOfFrame = 1000 / FrameLock;
		
	while (!(GetKeyState(VK_ESCAPE) & 0x8000))
	{
		// Handle Windows messages
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Loops = 0;
		SystemTime = GetTickCount();

		while (SystemTime > NextTick && ++Loops < MaxFrameSkip)
		{
			// Handle controls and change of direction
			ControlPlayerMovement(hWnd);

			// Update entities position
			MoveEntities();

			// Calculate remaining frametime
			NextTick += LengthOfFrame;
		}

		// Calculate frames per second
		FPSCounter();
		
		// Sort entities
		SortEntities();

		// Update RayCast view by player position
		RayPosX = PlayerPosX;
		RayPosY = PlayerPosY;

		// Init threadpool
		// see here http://think-async.com/Asio/Recipes

		boost::asio::io_service IOService;
		
		std::unique_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(IOService));

		for (size_t t = 0; t < NumberOfThreads; ++t)
		{
			GameLoopGroup.create_thread(boost::bind(&boost::asio::io_service::run, &IOService));
		}

		// Lock LockBits buffer
		BufferLB.Lock();

		// Post graphics jobs to Threadpool
		// Renderparts :
		// L = Left walls
		// R = Right walls
		// F = Floor
		// C = Ceiling

		// Left walls
		IOService.post(boost::bind(CastGraphics,'L'));
		// Right walls
		IOService.post(boost::bind(CastGraphics,'R'));
		// Floor
		IOService.post(boost::bind(CastGraphics,'F'));
		// Ceiling
		IOService.post(boost::bind(CastGraphics,'C'));

		// Wait for all started threads to finish
		IOService.stop();
		GameLoopGroup.join_all();

		// Display entities
		RenderEntities();

		// Draw player weapon to buffer
		DrawPlayerWeapon();

		// Release LockBits buffer
		BufferLB.Release();
		
		// Display HUD
		if (HUDEnabled)
		{
			DisplayHUD();
		}

		// Display Minimap
		if (MinimapEnabled)
		{
			DisplayMinimap();
		}
		
		// Draw crosshair to buffer
		Buffer.DrawCachedBitmap(&Crosshair, CrosshairPosX, CrosshairPosY);

		// Render buffer to screen via GDI / BitBlt
		HBITMAP hBitmap;
		BufferBMP.GetHBITMAP(Gdiplus::Color::Black, &hBitmap);
		SelectObject(hdcMem, hBitmap);
		BitBlt(hdc, 0, 0, WindowViewPortWidth, WindowViewPortHeight, hdcMem, 0, 0, SRCCOPY);
		DeleteObject(hBitmap);
	}

	return uint_fast32_t (msg.wParam);

	// Close window, end GDI+
	EndPaint(hWnd, &ps);
	CloseWindow(hWnd);
	Gdiplus::GdiplusShutdown(gdiplusToken);

	// remove RAW INPUT devices
	UnregisterRawInputDevice(HID_MOUSE);
	UnregisterRawInputDevice(HID_KEYBOARD);

	//  and exit program
	exit(0);
}

LRESULT CALLBACK WndProc(HWND hWnd, uint_fast32_t message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PAINT:
			return(0);

		case WM_CREATE:
			return(0);

		case WM_INPUT:
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

			if (RawDev->header.dwType == RIM_TYPEMOUSE)
			{
				// Get mouse position
				MousePos.x = RawDev->data.mouse.lLastX;
				MousePos.y = RawDev->data.mouse.lLastY;

				// Get state of mouse bot
				if (RawDev->data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_DOWN)
				{
					FireWeaponSingleShot();
				}
			}

			if (RawDev->header.dwType == RIM_TYPEKEYBOARD)
			{
				if (RawDev->data.keyboard.Message == WM_KEYDOWN || RawDev->data.keyboard.Message == WM_SYSKEYDOWN)
				{
					// Enable/disable HUD
					if (RawDev->data.keyboard.VKey == HUDKey)
					{
						HUDEnabled = !HUDEnabled;
					}

					// Toggle Minimap
					if (RawDev->data.keyboard.VKey == MiniMapKey)
					{
						MinimapEnabled = !MinimapEnabled;
					}

					// Increase mouse sensitivity "+"
					if ((RawDev->data.keyboard.VKey == VK_OEM_PLUS) || (RawDev->data.keyboard.VKey == VK_ADD))
					{
						IncreaseMouseSensitivity();
					}

					// Decrease mouse sensitivity "-"
					if ((RawDev->data.keyboard.VKey == VK_OEM_MINUS) || (RawDev->data.keyboard.VKey == VK_SUBTRACT))
					{
						DecreaseMouseSensitivity();
					}
				}
			}
			break;
		
		case MM_MCINOTIFY:
			switch (wParam)
			{
				// "rewind" footsteps audio if played completely
				case MCI_NOTIFY_SUCCESSFUL:
					mciSendString("seek PlayerFootSteps to start", NULL, 0, NULL);
					break;
			}
			
		case WM_DESTROY:
			PostQuitMessage(0);
			return(0);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void CastGraphics(char RenderPart)
{
	int_fast32_t Start = 0;
	int_fast32_t End = WindowViewPortWidth;

	switch (RenderPart)
	{
		case 'L':
		{
			End = WindowWidthMid;
			break;
		}
		case 'R':
		{
			Start = WindowWidthMid;
			break;
		}
	}

	for (int_fast32_t x = Start; x < End; ++x)
	{
		float Camera = (x << 1) / static_cast<float>(WindowViewPortWidth) - 1;
		float RayDirX = PlayerDirX + PlaneX * Camera;
		float RayDirY = PlayerDirY + PlaneY * Camera;

		int_fast32_t MapPosX = static_cast<int_fast32_t>(RayPosX);
		int_fast32_t MapPosY = static_cast<int_fast32_t>(RayPosY);

		float deltaDistX = std::abs(1 / RayDirX);
		float deltaDistY = std::abs(1 / RayDirY);

		float SideDistX;
		float SideDistY;

		int_fast32_t StepX;
		int_fast32_t StepY;

		if (RayDirX < 0)
		{
			StepX = -1;
			SideDistX = (RayPosX - MapPosX) * deltaDistX;
		}
		else
		{
			StepX = 1;
			SideDistX = (MapPosX + 1 - RayPosX) * deltaDistX;
		}

		if (RayDirY < 0)
		{
			StepY = -1;
			SideDistY = (RayPosY - MapPosY) * deltaDistY;
		}
		else
		{
			StepY = 1;
			SideDistY = (MapPosY + 1 - RayPosY) * deltaDistY;
		}

		bool WallHit = false;
		bool WallSide = false;

		while (!WallHit)
		{
			if (SideDistX < SideDistY)
			{
				SideDistX += deltaDistX;
				MapPosX += StepX;
				WallSide = false;
			}
			else
			{
				SideDistY += deltaDistY;
				MapPosY += StepY;
				WallSide = true;
			}

			if (LevelMap[MapPosX][MapPosY] > 0)
			{
				WallHit = true;
			}
		}

		float WallDist;

		if (WallSide)
		{
			WallDist = (MapPosY - RayPosY + (1 - StepY) / 2) / RayDirY;
		}
		else
		{
			WallDist = (MapPosX - RayPosX + (1 - StepX) / 2) / RayDirX;
		}

		int_fast32_t LineHeight = static_cast<int_fast32_t>(WindowViewPortHeight / WallDist);

		int_fast32_t LineStart = (-LineHeight / 2) + (WindowViewPortHeight / 2);

		LineStart = max(LineStart, 0);

		int_fast32_t LineEnd = (LineHeight >> 1) + (WindowViewPortHeight >> 1);

		LineEnd = min(LineEnd, WindowViewPortHeight);

		float WallX;

		if (WallSide)
		{
			WallX = RayPosX + WallDist * RayDirX;
		}
		else
		{
			WallX = RayPosY + WallDist * RayDirY;
		}

		WallX -= floor(WallX);

		if (RenderPart == 'L' || RenderPart == 'R')
		{
			int_fast32_t TextureX = static_cast<int_fast32_t>(WallX * static_cast<float>(TextureSize));

			if (!WallSide && RayDirX > 0)
			{
				TextureX = TextureSize - TextureX - 1;
			}

			if (WallSide && RayDirY < 0)
			{
				TextureX = TextureSize - TextureX - 1;
			}

			int_fast32_t LevelMapPos = LevelMap[MapPosX][MapPosY] - 1;

			uint_fast32_t ShadeFactorWalls = static_cast<uint_fast32_t>(72 - (WallDist * 14));

			for (int_fast32_t y = LineStart; y < LineEnd; ++y)
			{
				// Draw walls
				BufferLB.SetShadedPixel(x, y, Texture[LevelMapPos][TextureX][((((y << 8) - (WindowViewPortHeight << 7) + (LineHeight << 7)) * TextureSize) / LineHeight) >> 8], ShadeFactorWalls);
			}
		}

		if (RenderPart == 'F' || RenderPart == 'C')
		{
			float FloorWallX;
			float FloorWallY;
			
			if (!WallSide && RayDirX > 0)
			{
				FloorWallX = static_cast<float>(MapPosX);
				FloorWallY = MapPosY + WallX;
			}
			else if (!WallSide && RayDirX < 0)
			{
				FloorWallX = MapPosX + 1.0f;
				FloorWallY = MapPosY + WallX;
			}
			else if (WallSide && RayDirY > 0)
			{
				FloorWallX = MapPosX + WallX;
				FloorWallY = static_cast<float>(MapPosY);
			}
			else
			{
				FloorWallX = MapPosX + WallX;
				FloorWallY = MapPosY + 1.0f;
			}

			switch (RenderPart)
			{
				case 'F':
				{
					for (int_fast32_t y = LineEnd; y < WindowViewPortHeight; ++y)
					{
						float FactorW = (WindowViewPortHeight / (2.0f * y - WindowViewPortHeight)) / WallDist;

						// Draw floor
						BufferLB.SetShadedPixel(x, y, Texture[FloorTexture][static_cast<int_fast32_t>((FactorW * FloorWallX + (1 - FactorW) * PlayerPosX) * TextureSize) & TextureSizeBitwiseAnd][static_cast<int_fast32_t>((FactorW * FloorWallY + (1 - FactorW) * PlayerPosY) * TextureSize) & TextureSizeBitwiseAnd], 64 - ((WindowViewPortHeight - y) >> 2));
					}

					break;
				}

				case 'C':
				{
					// Store WallDist in 1D-ZBuffer for later calculation of entity distance
					// Needs only to be calculated once
					ZBuffer[x] = WallDist;

					for (int_fast32_t y = LineEnd; y < WindowViewPortHeight; ++y)
					{
						float FactorW = (WindowViewPortHeight / (2.0f * y - WindowViewPortHeight)) / WallDist;

						// Draw ceiling
						BufferLB.SetShadedPixel(x, (WindowViewPortHeight - y), Texture[CeilingTexture][static_cast<int_fast32_t>((FactorW * FloorWallX + (1 - FactorW) * PlayerPosX) * TextureSize) & TextureSizeBitwiseAnd][static_cast<int_fast32_t>((FactorW * FloorWallY + (1 - FactorW) * PlayerPosY) * TextureSize) & TextureSizeBitwiseAnd], 64 - ((WindowViewPortHeight - y) >> 2));
					}
				}
			}
		}
	}
}

void RenderEntities()
{
	// Set Matrix for sprite transformation
	float InverseMatrix = 1.0f / (PlaneX * PlayerDirY - PlayerDirX * PlaneY);

	for (uint_fast32_t i = 0; i < NumberOfUsedEntities; ++i)
	{
		float EntityPosX = Entity[EntityOrder[i]].PosX - PlayerPosX;
		float EntityPosY = Entity[EntityOrder[i]].PosY - PlayerPosY;

		float TransX = InverseMatrix * (PlayerDirY * EntityPosX - PlayerDirX * EntityPosY);
		float TransY = InverseMatrix * (-PlaneY * EntityPosX + PlaneX * EntityPosY);

		int_fast32_t EntitySX = static_cast<int_fast32_t>((WindowViewPortWidth >> 1) * (1 + TransX / TransY));
		int_fast32_t vScreen = static_cast<int_fast32_t>(EntityMoveV / TransY);

		int_fast32_t EntityHeight = abs(static_cast<int_fast32_t>(WindowViewPortHeight / (TransY))) / EntitySizeDivV;
		int_fast32_t LineStartY = ((-EntityHeight / 2) + (WindowViewPortHeight / 2) + vScreen);

		LineStartY = max(LineStartY, 0);

		int_fast32_t LineEndY = (EntityHeight >> 1) + (WindowViewPortHeight >> 1) + vScreen;

		LineEndY = min(LineEndY, WindowViewPortHeight);

		int_fast32_t EntityWidth = abs(static_cast<int_fast32_t>(WindowViewPortHeight / (TransY))) / EntitySizeDivU;
		int_fast32_t LineStartX = (-EntityWidth / 2) + EntitySX;

		int_fast32_t LineEndX = (EntityWidth >> 1) + EntitySX;

		LineEndX = min(LineEndX, WindowViewPortWidth);

		// Get angle between player and entity without atan2
		// Returns TextureIndex (0..7) for adressing correct texture

		float EntityTempX = Entity[Entity[EntityOrder[i]].Number].PosX - PlayerPosX;
		float EntityTempY = Entity[Entity[EntityOrder[i]].Number].PosY - PlayerPosY;

		float CosTheta1 = (EntityTempX + EntityTempY) * static_cast<float>(M_SQRT1_2);
		float CosTheta3 = (EntityTempY - EntityTempX) * static_cast<float>(M_SQRT1_2);

		uint_fast32_t TextureIndex = 0;
		float ClosestTheta = EntityTempX;

		if (ClosestTheta < CosTheta1) 
		{ 
			TextureIndex = 1; 
			ClosestTheta = CosTheta1; 
		}
		
		if (ClosestTheta < EntityTempY) 
		{ 
			TextureIndex = 2; 
			ClosestTheta = EntityTempY; 
		}
		
		if (ClosestTheta < CosTheta3) 
		{ 
			TextureIndex = 3; 
			ClosestTheta = CosTheta3; 
		}
		
		if (ClosestTheta < -EntityTempX) 
		{ 
			TextureIndex = 4; 
			ClosestTheta = -EntityTempX; 
		}
		
		if (ClosestTheta < -CosTheta1) 
		{ 
			TextureIndex = 5; 
			ClosestTheta = -CosTheta1; 
		}
		
		if (ClosestTheta < -EntityTempY) 
		{ 
			TextureIndex = 6; 
			ClosestTheta = -EntityTempY; 
		}
		
		if (ClosestTheta < -CosTheta3) 
		{ 
			TextureIndex = 7; 
		}

		// Add rotation factor to Textureindex dependent on heading direction of entity
		uint_fast32_t TextureIndexTemp = TextureIndex + Entity[Entity[EntityOrder[i]].Number].RotationFactor;

		if (TextureIndexTemp < 8)
		{
			TextureIndex = TextureIndexTemp;
		}
		else
		{
			TextureIndex = TextureIndexTemp - 8;
		}
			
		// Set shadefactor
		uint_fast32_t ShadeFactorEntity = static_cast<uint_fast32_t>(72 - (TransY * 14));

		for (int_fast32_t x = LineStartX; x < LineEndX; ++x)
		{
			int_fast32_t TextureX = static_cast<int_fast32_t>(x - ((-EntityWidth >> 1) + EntitySX)) * EntitySize / EntityWidth;

			if (TransY > 0 && x > 0 && x < WindowViewPortWidth && TransY < ZBuffer[x])
			{
				for (int_fast32_t y = LineStartY; y < LineEndY; ++y)
				{
					uint_fast32_t PixelColor = Entity[Entity[EntityOrder[i]].Number].Texture[TextureIndex][TextureX][(((((y - vScreen) << 8) - (WindowViewPortHeight << 7) + (EntityHeight << 7)) * EntitySize) / EntityHeight) >> 8];

					// Check if alphachannel of pixel ist not transparent and draw pixel
					if (PixelColor & 0xFFFFFFFF)
					{
						BufferLB.SetShadedPixel(x, y, PixelColor, ShadeFactorEntity);
					}
				}
			}
		}
	}
}

void FPSCounter()
{
	// Calculate FPS
	if (SystemTime - FPSUpdate >= 1000) 
	{
		FPS = FPSFrames;
		FPSUpdate = SystemTime;
		FPSFrames = 0;
	}
	
	// Convert FPS variable to string
	FPSPrint = std::to_wstring(FPS);

	++FPSFrames;
}

void DisplayHUD()
{
	// variable is used to keep vertical spacings between drawn textlines
	uint_fast32_t DiffYPos = 0;

	// Display options
	Buffer.DrawString(HUDOptionLabel.c_str(), -1, &HUDFont, Gdiplus::PointF(HUDMenuPosX, HUDMenuPosY), &HUDBrush);
	
	std::string TempString = std::string("Press ") + HUDKey + std::string(" ") + HUDHelpText;
	std::wstring TempStringWide = std::wstring(TempString.begin(), TempString.end());

	Buffer.DrawString(TempStringWide.c_str(), -1, &HUDFont, Gdiplus::PointF(HUDMenuPosX, HUDMenuPosY + (DiffYPos += 15)), &HUDBrush);
	
	TempString = std::string("Press ") + MiniMapKey + std::string(" ") + HUDToggleMinimapText;
	TempStringWide = std::wstring(TempString.begin(), TempString.end());

	Buffer.DrawString(TempStringWide.c_str(), -1, &HUDFont, Gdiplus::PointF(HUDMenuPosX, HUDMenuPosY + (DiffYPos += 15)), &HUDBrush);
	
	Buffer.DrawString(HUDMouseSensitivityText.c_str(), -1, &HUDFont, Gdiplus::PointF(HUDMenuPosX, HUDMenuPosY + (DiffYPos += 15)), &HUDBrush);
	Buffer.DrawString(HUDExitText.c_str(), -1, &HUDFont, Gdiplus::PointF(HUDMenuPosX, HUDMenuPosY + (DiffYPos += 15)),&HUDBrush);

	DiffYPos = 0;

	// Display FPS counter
	std::wstring FPSText = L"fps : " + FPSPrint;
	Buffer.DrawString(FPSText.c_str(), -1, &HUDFont, Gdiplus::PointF(10, HUDMenuPosY), &HUDBrush);

	// Display mouse sensitivity
	std::wstring MouseSensitivityText = L"mouse sensitivity : " + MouseSensitivityPrint;
	Buffer.DrawString(MouseSensitivityText.c_str(), -1, &HUDFont, Gdiplus::PointF(10, HUDMenuPosY + (DiffYPos += 15)), &HUDBrush);
}

void DisplayMinimap()
{
	int_fast32_t x = MinimapPosX;

	for (uint_fast32_t i = 0; i < MapHeight; ++i)
	{
		int_fast32_t y = MinimapPosY;

		for (uint_fast32_t j = 0; j < MapWidth; ++j)
		{
			// Draw player symbol
			if (EntityMap[j][i] == EntityMapPlayerPos)
			{
				Buffer.FillRectangle(&MinimapPlayerBrush, x + 1, y + 1, MinimapTileSize - 2, MinimapTileSize - 2);
			}
								
			// Draw walls
			if (LevelMap[j][i])
			{
				Buffer.FillRectangle(&MinimapWallBrush, x, y, MinimapTileSize, MinimapTileSize);
			}
			
			// Draw entity symbols
			if (EntityMap[j][i] == EntityMapEnemyPos)
			{
				Buffer.FillRectangle(&MinimapEnemyBrush, x + 1, y + 1, MinimapTileSize - 2, MinimapTileSize - 2);
			}
						
			y += MinimapTileSize;
		}

		x += MinimapTileSize;
	}
}

void DrawPlayerWeapon()
{
	CalculateWeaponSway();

	uint_fast32_t SwayTempX = Weapon[SelectedPlayerWeapon].PosX + static_cast<uint_fast32_t>(WeaponSwayX);
	uint_fast32_t SwayTempY = Weapon[SelectedPlayerWeapon].PosY + static_cast<uint_fast32_t>(WeaponSwayY);

	// Draw muzzle flash if weapon is fired
	if (WeaponFiredFlag == true)
	{
		for (uint_fast32_t y = 0; y < Weapon[SelectedPlayerWeapon].MuzzleFlashHeight; ++y)
		{
			for (uint_fast32_t x = 0; x < Weapon[SelectedPlayerWeapon].MuzzleFlashWidth; ++x)
			{
				// currently selected Texture[0] as placeholder for upcoming variable!
				uint_fast32_t PixelColor = Weapon[SelectedPlayerWeapon].MuzzleFlashTexture[x][y];

				if (PixelColor & 0xFFFFFFFF)
				{
					BufferLB.SetPixel(x + SwayTempX + Weapon[SelectedPlayerWeapon].MuzzleFlashPosX, y + SwayTempY - Weapon[SelectedPlayerWeapon].MuzzleFlashPosY, PixelColor);
				}
			}
		}
	
		if (--MuzzleFlashCounter == 0)
		{
			WeaponFiredFlag = false;
		}
	}

	// Draw weapon
	for (uint_fast32_t y = 0; y < Weapon[SelectedPlayerWeapon].Height; ++y)
	{
		for (uint_fast32_t x = 0; x < Weapon[SelectedPlayerWeapon].Width; ++x)
		{
			// currently selected Texture[0] as placeholder for upcoming variable!
			int_fast32_t PixelColor = Weapon[SelectedPlayerWeapon].Texture[0][x][y];

			int_fast32_t DrawX = x + SwayTempX;
			int_fast32_t DrawY = y + SwayTempY;

			if ((PixelColor & 0xFFFFFFFF) && DrawY < WindowViewPortHeight && DrawX < WindowViewPortWidth)
			{
				BufferLB.SetPixel(DrawX, DrawY, PixelColor);
			}
		}
	}
}

void ControlPlayerMovement(HWND hWnd)
{
	// Turn around (via mouse movement)
	if (MousePos.x != OldMousePos.x)
	{
		float Rotation = (MousePos.x * MouseSensitivity) * FLOAT_RAD2DEG;
		float oldDirX = PlayerDirX;

		float TmpCos = cosf(-Rotation);
		float TmpSin = sinf(-Rotation);

		PlayerDirX = PlayerDirX * TmpCos - PlayerDirY * TmpSin;
		PlayerDirY = oldDirX * TmpSin + PlayerDirY * TmpCos;
		float oldPlaneX = PlaneX;
		PlaneX = PlaneX * TmpCos - PlaneY * TmpSin;
		PlaneY = oldPlaneX * TmpSin + PlaneY * TmpCos;
		ClipCursor(&WindowRect);
		OldMousePos.x = MousePos.x;
	}

	// Move forward
	if (GetKeyState(MoveForwardKey) & 0x8000)
	{
		EntityMap[static_cast<uint_fast32_t>(PlayerPosX)][static_cast<uint_fast32_t>(PlayerPosY)] = EntityMapClearField;
		
		PlayerPosX += PlayerDirX * PlayerMoveSpeed;
		PlayerPosY += PlayerDirY * PlayerMoveSpeed;

		WeaponPace += Weapon[SelectedPlayerWeapon].PaceFactor;

		EntityMap[static_cast<uint_fast32_t>(PlayerPosX)][static_cast<uint_fast32_t>(PlayerPosY)] = EntityMapPlayerPos;

		// Collision detection against walls and entities
		if ((LevelMap[static_cast<uint_fast32_t>(PlayerPosX + PlayerDirX * PlayerCollisionDetectionFactor)][static_cast<uint_fast32_t>(PlayerPosY + PlayerDirY * PlayerCollisionDetectionFactor)]) 
			|| (EntityMap[static_cast<uint_fast32_t>(PlayerPosX + PlayerDirX * PlayerCollisionDetectionFactor)][static_cast<uint_fast32_t>(PlayerPosY + PlayerDirY * PlayerCollisionDetectionFactor)] == EntityMapEnemyPos))
		{
			PlayerPosX -= PlayerDirX * PlayerMoveSpeed;
			PlayerPosY -= PlayerDirY * PlayerMoveSpeed;
		}

		mciSendString("play PlayerFootSteps notify", NULL, 0, hWnd);
	}

	// Move backwards
	if (GetKeyState(MoveBackwardKey) & 0x8000)
	{
		EntityMap[static_cast<uint_fast32_t>(PlayerPosX)][static_cast<uint_fast32_t>(PlayerPosY)] = EntityMapClearField;
		
		PlayerPosX -= PlayerDirX * PlayerMoveSpeed;
		PlayerPosY -= PlayerDirY * PlayerMoveSpeed;

		WeaponPace += Weapon[SelectedPlayerWeapon].PaceFactor;

		EntityMap[static_cast<uint_fast32_t>(PlayerPosX)][static_cast<uint_fast32_t>(PlayerPosY)] = EntityMapPlayerPos;

		// Collision detection against walls and entities
		if ((LevelMap[static_cast<uint_fast32_t>(PlayerPosX - PlayerDirX * PlayerCollisionDetectionFactor)][static_cast<uint_fast32_t>(PlayerPosY - PlayerDirY * PlayerCollisionDetectionFactor)])
			|| (EntityMap[static_cast<uint_fast32_t>(PlayerPosX - PlayerDirX * PlayerCollisionDetectionFactor)][static_cast<uint_fast32_t>(PlayerPosY - PlayerDirY * PlayerCollisionDetectionFactor)] == EntityMapEnemyPos))
		{
			PlayerPosX += PlayerDirX * PlayerMoveSpeed;
			PlayerPosY += PlayerDirY * PlayerMoveSpeed;
		}

		mciSendString("play PlayerFootSteps notify", NULL, 0, hWnd);
	}

	// Strafe right
	if (GetKeyState(StrafeRightKey) & 0x8000)
	{
		EntityMap[static_cast<uint_fast32_t>(PlayerPosX)][static_cast<uint_fast32_t>(PlayerPosY)] = EntityMapClearField;

		PlayerPosX += PlaneX * PlayerMoveSpeed;
		PlayerPosY += PlaneY * PlayerMoveSpeed;

		EntityMap[static_cast<uint_fast32_t>(PlayerPosX)][static_cast<uint_fast32_t>(PlayerPosY)] = EntityMapPlayerPos;

		// Collision detection against walls and entities
		if ((LevelMap[static_cast<uint_fast32_t>(PlayerPosX + PlaneX * PlayerCollisionDetectionFactor)][static_cast<uint_fast32_t>(PlayerPosY + PlaneY * PlayerCollisionDetectionFactor)])
			|| (EntityMap[static_cast<uint_fast32_t>(PlayerPosX + PlaneX * PlayerCollisionDetectionFactor)][static_cast<uint_fast32_t>(PlayerPosY + PlaneY * PlayerCollisionDetectionFactor)] == EntityMapEnemyPos))
		{
			PlayerPosX -= PlaneX * PlayerMoveSpeed;
			PlayerPosY -= PlaneY * PlayerMoveSpeed;
		}

		mciSendString("play PlayerFootSteps notify", NULL, 0, hWnd);
	}

	// Strafe left
	if (GetKeyState(StrafeLeftKey) & 0x8000)
	{
		EntityMap[static_cast<uint_fast32_t>(PlayerPosX)][static_cast<uint_fast32_t>(PlayerPosY)] = EntityMapClearField;

		PlayerPosX -= PlaneX * PlayerMoveSpeed;
		PlayerPosY -= PlaneY * PlayerMoveSpeed;

		EntityMap[static_cast<uint_fast32_t>(PlayerPosX)][static_cast<uint_fast32_t>(PlayerPosY)] = EntityMapPlayerPos;

		// Collision detection against walls and entities
		if ((LevelMap[static_cast<uint_fast32_t>(PlayerPosX - PlaneX * PlayerCollisionDetectionFactor)][static_cast<uint_fast32_t>(PlayerPosY - PlaneY * PlayerCollisionDetectionFactor)])
			|| (EntityMap[static_cast<uint_fast32_t>(PlayerPosX - PlaneX * PlayerCollisionDetectionFactor)][static_cast<uint_fast32_t>(PlayerPosY - PlaneY * PlayerCollisionDetectionFactor)] == EntityMapEnemyPos))
		{
			PlayerPosX += PlaneX * PlayerMoveSpeed;
			PlayerPosY += PlaneY * PlayerMoveSpeed;
		}

		mciSendString("play PlayerFootSteps notify", NULL, 0, hWnd);
	}
}