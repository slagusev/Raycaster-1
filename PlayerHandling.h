/*
************************************
* Functions for player handling    *
*                                  *
* PlayerHandling.h                 *
*                                  *
* (c) 2017, 2018 Stefan Kubsch     *
************************************
*/

#pragma once

//
// Declare functions
//

void InitPlayerConfig(uint_fast32_t LevelNumber);

//
// Variables and constants
//

// Variables used for player position
float PlayerPosX;
float PlayerPosY;

// Variables used for view direction of player
float PlayerDirX;
float PlayerDirY;

// Current selected player weapon  (will be replaced with dynamic solution...)
uint_fast32_t SelectedPlayerWeapon;

//
// Functions
//

void InitPlayerConfig(uint_fast32_t LevelNumber)
{
	char TempVal[100];
	
	// Build filepath/filename dynamically with LevelNumber
	std::string PathPart1 = "./DATA/PlayerData/Level_";
	std::string PathPart2 = std::to_string(LevelNumber);
	std::string PathPart3 = "/Level_";
	std::string PathPart4 = std::to_string(LevelNumber);
	std::string PathPart5 = "_Player_Config.ini";

	std::string IniFile = PathPart1 + PathPart2 + PathPart3 + PathPart4 + PathPart5;

	// read data from ini file
	GetPrivateProfileString("POSITION", "PlayerPosX", "14.5", TempVal, 100, IniFile.c_str());
	PlayerPosX = static_cast<float>(atof(TempVal));
	
	GetPrivateProfileString("POSITION", "PlayerPosY", "1.5", TempVal, 100, IniFile.c_str());
	PlayerPosY = static_cast<float>(atof(TempVal));
	
	GetPrivateProfileString("POSITION", "PlayerDirX", "-1.0", TempVal, 100, IniFile.c_str());
	PlayerDirX = static_cast<float>(atof(TempVal));
	
	GetPrivateProfileString("POSITION", "PlayerDirY", "0.0", TempVal, 100, IniFile.c_str());
	PlayerDirY = static_cast<float>(atof(TempVal));

	SelectedPlayerWeapon = GetPrivateProfileInt("WEAPON", "SelectedPlayerWeapon ", 0, IniFile.c_str());
}

