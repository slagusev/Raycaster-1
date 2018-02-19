/*
**************************************************************
* AudioFunctions.h provides various functions audio handling *
*                                                            *
* AudioHandling.h                                            *
*                                                            *
* (c) 2017, 2018 Stefan Kubsch                               *
**************************************************************
*/

//
// Declare functions
//

void InitLevelBackgroundMusic(uint_fast32_t LevelNumber);
void InitPlayerAudio(uint_fast32_t LevelNumber);
void InitWeaponAudio(uint_fast32_t LevelNumber);
void PlayLevelBackgroundMusic();

//
// Functions
//

void InitLevelBackgroundMusic(uint_fast32_t LevelNumber)
{
	// Build filepath/filename dynamically with LevelNumber
	std::string PathPart1 = "./DATA/LevelData/Level_";
	std::string PathPart2 = std::to_string(LevelNumber);
	std::string PathPart3 = "/Level_";
	std::string PathPart4 = std::to_string(LevelNumber);
	std::string PathPart5 = "_Level_Config.ini";
	
	std::string IniFile = PathPart1 + PathPart2 + PathPart3 + PathPart4 + PathPart5;

	// read data from ini file
	char TempVal[100];

	GetPrivateProfileString("AUDIO", "BackgroundMusic", "SFX\\BackgroundMusic\\AtmoLoop.mp3", TempVal, 100, IniFile.c_str());

	// build MCI command string
	std::string Command = std::string("open ") + std::string(TempVal) + std::string(" type mpegvideo alias ") + std::string("BackgroundMusic");

	mciSendString(Command.c_str(), NULL, 0, NULL);
}

void InitPlayerAudio(uint_fast32_t LevelNumber)
{
	// Build filepath/filename dynamically with LevelNumber
	std::string PathPart1 = "./DATA/PlayerData/Level_";
	std::string PathPart2 = std::to_string(LevelNumber);
	std::string PathPart3 = "/Level_";
	std::string PathPart4 = std::to_string(LevelNumber);
	std::string PathPart5 = "_Player_Config.ini";

	std::string IniFile = PathPart1 + PathPart2 + PathPart3 + PathPart4 + PathPart5;

	// read data from ini file
	char TempVal[100];

	GetPrivateProfileString("AUDIO", "PlayerFootSteps", "SFX\\PlayerSounds\\PlayerFootSteps.wav", TempVal, 100, IniFile.c_str());
	
	// build MCI command string
	std::string Command = std::string("open ") + std::string(TempVal) + std::string(" type waveaudio alias ") + std::string("PlayerFootSteps");

	mciSendString(Command.c_str(), NULL, 0, NULL);
}

void InitWeaponAudio(uint_fast32_t LevelNumber)
{
	for (uint_fast32_t i = 0; i < NumberOfUsedWeapons; ++i)
	{
		// Build filepath/filename dynamically with LevelNumber and Weapon.Number
		std::string PathPart1 = "./DATA/WeaponData/Level_";
		std::string PathPart2 = std::to_string(LevelNumber);
		std::string PathPart3 = "/Level_";
		std::string PathPart4 = std::to_string(LevelNumber);
		std::string PathPart5 = "_Weapon_";
		std::string PathPart6 = std::to_string(i);
		std::string PathPart7 = "_Data.ini";

		std::string IniFile = PathPart1 + PathPart2 + PathPart3 + PathPart4 + PathPart5 + PathPart6 + PathPart7;

		// read data from ini file
		char TempVal[100];

		GetPrivateProfileString("AUDIO", "SingleShotAudio", "SFX\\WeaponSounds\\AK12\\AK12_SingleShot.wav", TempVal, 100, IniFile.c_str());
		
		// build MCI command string
		std::string Command = std::string("open ") + std::string(TempVal) + std::string(" type waveaudio alias ") + std::string(Weapon[i].Name);

		mciSendString(Command.c_str(), NULL, 0, NULL);
	}
}

void PlayLevelBackgroundMusic()
{
	mciSendString("play BackgroundMusic repeat", NULL, 0, NULL);
}
