/*
************************************
* Functions for weapon handling    *
*                                  *
* WeaponHandling.h                 *
*                                  *
* (c) 2017, 2018 Stefan Kubsch     *
************************************
*/

//
// Declare functions
//

void InitWeaponConfig(uint_fast32_t LevelNumber);
void InitWeaponTextures(uint_fast32_t LevelNumber);
void InitWeaponMuzzleFlashTextures(uint_fast32_t LevelNumber);
void CalculateWeaponSway();
void FireWeaponSingleShot();

//
// Variables and constants
//

// Vector and variables used for weapons init
uint_fast32_t NumberOfUsedWeapons;
uint_fast32_t NumberOfUsedWeaponTextures;
std::vector<WeaponStruct> Weapon;

// Variables used for weapon handling
float WeaponSwayX;
float WeaponSwayY;
float WeaponPace		= 0;
bool WeaponFiredFlag	= false;

// Variables used for muzzle flash
uint_fast32_t MuzzleFlashCounter;

//
// Functions
//

void InitWeaponConfig(uint_fast32_t LevelNumber)
{
	// Build filepath/filename dynamically with LevelNumber
	std::string PathPart1 = "./DATA/WeaponData/Level_";
	std::string PathPart2 = std::to_string(LevelNumber);
	std::string PathPart3 = "/Level_";
	std::string PathPart4 = std::to_string(LevelNumber);
	std::string PathPart5 = "_Weapon_Config.ini";

	std::string IniFile = PathPart1 + PathPart2 + PathPart3 + PathPart4 + PathPart5;

	// read data from ini file
	char TempVal[100];

	NumberOfUsedWeapons = GetPrivateProfileInt("GENERAL", "NumberOfUsedWeapons", 1, IniFile.c_str());
	
	NumberOfUsedWeaponTextures = GetPrivateProfileInt("GENERAL", "NumberOfUsedWeaponTextures", 1, IniFile.c_str());

	// Resize vectors to needed sizes
	Weapon.resize(NumberOfUsedWeapons);

	for (uint_fast32_t i = 0; i < NumberOfUsedWeapons; ++i)
	{
		Weapon[i].Number = i;

		// Build filepath/filename dynamically with LevelNumber and Weapon.Number
		std::string PathPart1 = "./DATA/WeaponData/Level_";
		std::string PathPart2 = std::to_string(LevelNumber);
		std::string PathPart3 = "/Level_";
		std::string PathPart4 = std::to_string(LevelNumber);
		std::string PathPart5 = "_Weapon_";
		std::string PathPart6 = std::to_string(i);
		std::string PathPart7 = "_Data.ini";

		IniFile = PathPart1 + PathPart2 + PathPart3 + PathPart4 + PathPart5 + PathPart6 + PathPart7;

		// read data from ini file
		GetPrivateProfileString("DATA", "Name", "AK12", TempVal, 100, IniFile.c_str());
		Weapon[i].Name = TempVal;
		
		GetPrivateProfileString("DATA", "Weight", "3.3", TempVal, 100, IniFile.c_str());
		Weapon[i].Weight = static_cast<float>(atof(TempVal));
		
		Weapon[i].Cadence = GetPrivateProfileInt("DATA", "Cadence", 600, IniFile.c_str());
		
		GetPrivateProfileString("DATA", "PaceFactor", "0.05", TempVal, 100, IniFile.c_str());
		Weapon[i].PaceFactor = static_cast<float>(atof(TempVal));

		Weapon[i].PosX = GetPrivateProfileInt("POSITION", "PosX", 440, IniFile.c_str());
		
		Weapon[i].PosY = GetPrivateProfileInt("POSITION", "PosY", 290, IniFile.c_str());
		
		Weapon[i].Width = GetPrivateProfileInt("POSITION", "Width", 400, IniFile.c_str());
		
		Weapon[i].Height = GetPrivateProfileInt("POSITION", "Height", 307, IniFile.c_str());
		
		Weapon[i].MuzzleFlashDuration = GetPrivateProfileInt("MUZZLEFLASH", "MuzzleFlashDuration", 3, IniFile.c_str());
		
		Weapon[i].MuzzleFlashPosX = GetPrivateProfileInt("MUZZLEFLASH", "MuzzleFlashPosX", 20, IniFile.c_str());
		
		Weapon[i].MuzzleFlashPosY = GetPrivateProfileInt("MUZZLEFLASH", "MuzzleFlashPosY", 20, IniFile.c_str());

		Weapon[i].PosX = WindowViewPortWidth - Weapon[i].PosX;
		Weapon[i].PosY = WindowViewPortHeight - Weapon[i].PosY;
	}
}

void InitWeaponTextures(uint_fast32_t LevelNumber)
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
		std::string PathPart7 = "_TexturesData.conf";

		std::string FileName = PathPart1 + PathPart2 + PathPart3 + PathPart4 + PathPart5 + PathPart6 + PathPart7;

		// Read texture filenames from file
		std::ifstream WeaponTexturesDataFile;
		WeaponTexturesDataFile.open(FileName, std::ios::in);

		// resize 3D vector
		Weapon[i].Texture.resize(NumberOfUsedWeaponTextures);

		if (WeaponTexturesDataFile.good())
		{
			std::string TempString;
			uint_fast32_t j = 0;

			while (std::getline(WeaponTexturesDataFile, TempString))
			{
				// Load & lock texture

				std::wstring FileName = std::wstring(TempString.begin(), TempString.end());

				Gdiplus::Bitmap WeaponIMG(FileName.c_str(), PixelFormat32bppPARGB);
				LockBits WeaponIMGLB(WeaponIMG);
				WeaponIMGLB.Lock();

				// resize X vector
				Weapon[i].Texture[j].resize(Weapon[i].Width);

				for (uint_fast32_t x = 0; x < Weapon[i].Width; ++x)
				{
					// resize Y vector
					Weapon[i].Texture[j][x].resize(Weapon[i].Height);

					for (uint_fast32_t y = 0; y < Weapon[i].Height; ++y)
					{
						Weapon[i].Texture[j][x][y] = WeaponIMGLB.GetPixel(x, y);
					}
				}

				WeaponIMGLB.Release(); 

				++j;
			}

			WeaponTexturesDataFile.close();
		}
	}
}

void InitWeaponMuzzleFlashTextures(uint_fast32_t LevelNumber)
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
		std::string PathPart7 = "_MuzzleFlashTexturesData.conf";

		std::string FileName = PathPart1 + PathPart2 + PathPart3 + PathPart4 + PathPart5 + PathPart6 + PathPart7;

		// Read texture filenames from file
		std::ifstream WeaponMuzzleFlashTextureDataFile;
		WeaponMuzzleFlashTextureDataFile.open(FileName, std::ios::in);

		if (WeaponMuzzleFlashTextureDataFile.good())
		{
			std::string TempString;

			while (std::getline(WeaponMuzzleFlashTextureDataFile, TempString))
			{
				// Load & lock texture

				std::wstring FileName = std::wstring(TempString.begin(), TempString.end());

				Gdiplus::Bitmap MuzzleFlashIMG(FileName.c_str(), PixelFormat32bppPARGB);

				Weapon[i].MuzzleFlashWidth = MuzzleFlashIMG.GetWidth();
				Weapon[i].MuzzleFlashHeight = MuzzleFlashIMG.GetHeight();

				LockBits MuzzleFlashIMGLB(MuzzleFlashIMG);
				MuzzleFlashIMGLB.Lock();

				// resize X vector
				Weapon[i].MuzzleFlashTexture.resize(Weapon[i].MuzzleFlashWidth);

				for (uint_fast32_t x = 0; x < Weapon[i].MuzzleFlashWidth; ++x)
				{
					// resize Y vector
					Weapon[i].MuzzleFlashTexture[x].resize(Weapon[i].MuzzleFlashHeight);

					for (uint_fast32_t y = 0; y < Weapon[i].MuzzleFlashHeight; ++y)
					{
						Weapon[i].MuzzleFlashTexture[x][y] = MuzzleFlashIMGLB.GetPixel(x, y);
					}
				}

				MuzzleFlashIMGLB.Release(); 
			}

			WeaponMuzzleFlashTextureDataFile.close();
		}
	}
}

void CalculateWeaponSway()
{
	WeaponSwayX = cosf(WeaponPace * Weapon[SelectedPlayerWeapon].Weight) * 6.0f;
	WeaponSwayY = sinf(WeaponPace * (Weapon[SelectedPlayerWeapon].Weight * 1.5f)) * 6.0f;
}

void FireWeaponSingleShot()
{
	// build mci command string
	std::string Command = std::string("play ") + std::string(Weapon[SelectedPlayerWeapon].Name) + std::string(" from 0");

	// play shot audio
	mciSendString(Command.c_str(), NULL, 0, NULL);
	
	// Set flag and duration for rendering muzzleflash (used in DrawPlayerWeapon in Raycaster.cpp)
	WeaponFiredFlag = true;
	MuzzleFlashCounter = Weapon[SelectedPlayerWeapon].MuzzleFlashDuration;
}