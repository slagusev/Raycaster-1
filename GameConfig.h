/*
************************************
* Game config handling             *
*                                  *
* GameConfig.h                     *
*                                  *
* (c) 2017, 2018 Stefan Kubsch     *
************************************
*/

//
// Declare functions
//

class InitGameConfig
{
	public:
		InitGameConfig();
		~InitGameConfig();
};
	
//
// Functions
//

InitGameConfig::InitGameConfig()
{
	char TempVal[100];
	std::string TempString;

	char IniFile[] = "./DATA/GameConfig/GameConfig.ini";

	SelectedLevel = GetPrivateProfileInt("BASICPRESET", "SelectedLevel", 0, IniFile);
	
	GetPrivateProfileString("BASICPRESET", "PlayerMoveSpeed", "0.05", TempVal, 100, IniFile);
	PlayerMoveSpeed = static_cast<float>(atof(TempVal));

	HUDMenuPosX = static_cast<Gdiplus::REAL>(GetPrivateProfileInt("HUD", "HUDMenuPosX", 744, IniFile));

	HUDMenuPosY = static_cast<Gdiplus::REAL>(GetPrivateProfileInt("HUD", "HUDMenuPosY", 15, IniFile));
	
	GetPrivateProfileString("HUD", "CrosshairFileName", "error", TempVal, 100, IniFile);
	TempString = TempVal;
	CrosshairFileName = std::wstring(TempString.begin(), TempString.end());

	GetPrivateProfileString("HUD", "HUDOptionLabel", "error", TempVal, 100, IniFile);
	TempString = TempVal;
	HUDOptionLabel = std::wstring(TempString.begin(), TempString.end());

	GetPrivateProfileString("HUD", "HUDHelpText ", "error", TempVal, 100, IniFile);
	HUDHelpText = TempVal;

	GetPrivateProfileString("HUD", "HUDToggleMinimapText  ", "error", TempVal, 100, IniFile);
	HUDToggleMinimapText = TempVal;

	GetPrivateProfileString("HUD", "HUDMouseSensitivityText   ", "error", TempVal, 100, IniFile);
	TempString = TempVal;
	HUDMouseSensitivityText = std::wstring(TempString.begin(), TempString.end());

	GetPrivateProfileString("HUD", "HUDExitText    ", "error", TempVal, 100, IniFile);
	TempString = TempVal;
	HUDExitText = std::wstring(TempString.begin(), TempString.end());

	MinimapPosX = GetPrivateProfileInt("MINIMAP", "MinimapPosX", 20, IniFile);

	MinimapPosY = GetPrivateProfileInt("MINIMAP", "MinimapPosY", 618, IniFile);
	
	MinimapTileSize = GetPrivateProfileInt("MINIMAP", "MinimapTileSize", 8, IniFile);

	GetPrivateProfileString("WINDOW", "WindowName", "Raycaster", TempVal, 100, IniFile);
	WindowName = TempVal;

	WindowPosX = GetPrivateProfileInt("WINDOW", "WindowPosX", 10, IniFile);

	WindowPosY = GetPrivateProfileInt("WINDOW", "WindowPosY", 10, IniFile);

	WindowViewPortWidth = GetPrivateProfileInt("WINDOW", "WindowViewPortWidth", 1024, IniFile);
	
	WindowViewPortHeight = GetPrivateProfileInt("WINDOW", "WindowViewPortHeight", 768, IniFile);

	TextureSize = GetPrivateProfileInt("TEXTURES", "TextureSize ", 1024, IniFile);

	GetPrivateProfileString("MOUSE", "MouseSensitivity ", "1.0", TempVal, 100, IniFile);
	MouseSensitivity = static_cast<float>(atof(TempVal));

	GetPrivateProfileString("KEYBOARD", "MoveForwardKey", "W", TempVal, 100, IniFile);
	MoveForwardKey = TempVal[0];
	
	GetPrivateProfileString("KEYBOARD", "MoveBackwardKey", "S", TempVal, 100, IniFile);
	MoveBackwardKey = TempVal[0];
	
	GetPrivateProfileString("KEYBOARD", "StrafeLeftKey", "A", TempVal, 100, IniFile);
	StrafeLeftKey = TempVal[0];
	
	GetPrivateProfileString("KEYBOARD", "StrafeRightKey", "D", TempVal, 100, IniFile);
	StrafeRightKey = TempVal[0];
	
	GetPrivateProfileString("KEYBOARD", "HUDKey", "H", TempVal, 100, IniFile);
	HUDKey = TempVal[0];
	
	GetPrivateProfileString("KEYBOARD", "MiniMapKey", "M", TempVal, 100, IniFile);
	MiniMapKey = TempVal[0];

	GetPrivateProfileString("RAYCASTER", "PlaneX ", "0.0", TempVal, 100, IniFile);
	PlaneX = static_cast<float>(atof(TempVal));
	
	GetPrivateProfileString("RAYCASTER", "PlaneY ", "0.66", TempVal, 100, IniFile);
	PlaneY = static_cast<float>(atof(TempVal));
	
	FrameLock = GetPrivateProfileInt("RAYCASTER", "FrameLock", 60, IniFile);
	
	MaxFrameSkip = GetPrivateProfileInt("RAYCASTER", "MaxFrameSkip", 10, IniFile);
	
	//
	// Calculate remaining global parameters
	//

	// Define middle of window
	WindowWidthMid = WindowViewPortWidth / 2;
	WindowHeightMid = WindowViewPortHeight / 2;

	// Size of window (WindowWidth/Height + controls)
	WindowOverallWidth = WindowViewPortWidth + 16;
	WindowOverallHeight = WindowViewPortHeight + 39;

	// Factor for bitwise texture operations
	TextureSizeBitwiseAnd = TextureSize - 1;

	// Set factor for bitshifting from TextureSize
	// 7 for 128x128, 8 for 256x256, 9 for 512x512, 10 for 1024x1024
	switch (TextureSize)
	{
		case 128: 
		{
			TextureSizeShiftFactor = 7;
		}
		case 256: 
		{
			TextureSizeShiftFactor = 8;
		}
		case 512:
		{
			TextureSizeShiftFactor = 9;
		}
		case 1024:
		{
			TextureSizeShiftFactor = 10;
		}
	}
	
	// Initial settings for player movement speed
	PlayerCollisionDetectionFactor = PlayerMoveSpeed + 0.25f;

	// FPS Counter
	FPSUpdate = 0;
	FPSFrames = 0;
	FPS = 0;
}

InitGameConfig::~InitGameConfig() {};
