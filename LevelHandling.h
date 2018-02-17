/*
***********************************
* Functions for Level handling    *
*                                 *
* LevelHandling.h                 *
*                                 *
* (c) 2017, 2018 Stefan Kubsch    *
***********************************
*/

#pragma once

//
// Declare functions
//

void InitLevelConfig(uint_fast32_t LevelNumber);
void InitLevelMapData(uint_fast32_t LevelNumber);
void InitLevelTextures(uint_fast32_t LevelNumber);

//
// Variables and constants
//

// Vector to store LevelMap
std::vector<std::vector<uint_fast32_t>> LevelMap;

// 3D vector for level textures
std::vector<std::vector<std::vector<uint_fast32_t>>> Texture;

// Variables used for map dimensions (used for LevelMap and SpriteMap)
uint_fast32_t MapWidth;
uint_fast32_t MapHeight;

// Variables used for texture handling
uint_fast32_t NumberOfUsedTextures;
uint_fast32_t FloorTexture;
uint_fast32_t CeilingTexture;

//
// Functions
//

void InitLevelConfig(uint_fast32_t LevelNumber)
{
	// Build filepath/filename dynamically with LevelNumber
	std::string PathPart1 = "./DATA/LevelData/Level_";
	std::string PathPart2 = std::to_string(LevelNumber);
	std::string PathPart3 = "/Level_";
	std::string PathPart4 = std::to_string(LevelNumber);
	std::string PathPart5 = "_Level_Config.ini";

	std::string IniFile = PathPart1 + PathPart2 + PathPart3 + PathPart4 + PathPart5;
	
	// read data from ini file
	NumberOfUsedTextures = GetPrivateProfileInt("TEXTURES", "NumberOfUsedTextures", 13, IniFile.c_str());
	
	FloorTexture = GetPrivateProfileInt("TEXTURES", "FloorTexture ", 4, IniFile.c_str());
	
	CeilingTexture = GetPrivateProfileInt("TEXTURES", "CeilingTexture", 5, IniFile.c_str());
}

void InitLevelMapData(uint_fast32_t LevelNumber)
{
	std::string TempString;
	uint_fast32_t TempInt;
	char Delimiter;
	
	// Build filepath/filename dynamically with LevelNumber
	std::string PathPart1 = "./DATA/LevelData/Level_";
	std::string PathPart2 = std::to_string(LevelNumber);
	std::string PathPart3 = "/Level_";
	std::string PathPart4 = std::to_string(LevelNumber);
	std::string PathPart5 = "_Level_MapData.conf";

	std::string FileName = PathPart1 + PathPart2 + PathPart3 + PathPart4 + PathPart5;

	// Read level map data from file
	std::ifstream LevelMapDataFile;
	LevelMapDataFile.open(FileName, std::ios::in);

	if (LevelMapDataFile.good())
	{
		while (std::getline(LevelMapDataFile, TempString))
		{
			std::istringstream Stream(TempString);
			std::vector<uint_fast32_t> TempVector;

			while (Stream >> TempInt)
			{
				TempVector.push_back(TempInt);
				Stream >> Delimiter;
			}

			LevelMap.push_back(TempVector);
		}

		LevelMapDataFile.close();
	}

	// gather dimension of map
	MapWidth	= static_cast<uint_fast32_t>(LevelMap.size());
	MapHeight	= static_cast<uint_fast32_t>(LevelMap[0].size());
}

void InitLevelTextures(uint_fast32_t LevelNumber)
{
	std::string TempString;
	uint_fast32_t i = 0;

	// Build filepath/filename dynamically with LevelNumber
	std::string PathPart1 = "./DATA/LevelData/Level_";
	std::string PathPart2 = std::to_string(LevelNumber);
	std::string PathPart3 = "/Level_";
	std::string PathPart4 = std::to_string(LevelNumber);
	std::string PathPart5 = "_Level_TexturesData.conf";

	std::string FileName = PathPart1 + PathPart2 + PathPart3 + PathPart4 + PathPart5;

	// Read texture filenames from file
	std::ifstream LevelTexturesDataFile;
	LevelTexturesDataFile.open(FileName, std::ios::in);

	// resize 3D vector
	Texture.resize(NumberOfUsedTextures);
	
	if (LevelTexturesDataFile.good())
	{
		while (std::getline(LevelTexturesDataFile, TempString))
		{
			// Load & lock texture

			std::wstring FileName = std::wstring(TempString.begin(), TempString.end());

			Gdiplus::Bitmap TextureIMG(FileName.c_str(), PixelFormat32bppPARGB);
			LockBits TextureIMGLB(TextureIMG);
			TextureIMGLB.Lock();

			// resize X vector
			Texture[i].resize(TextureSize);

			for (uint_fast32_t x = 0; x < TextureSize; ++x)
			{
				// resize Y vector
				Texture[i][x].resize(TextureSize);
				
				for (uint_fast32_t y = 0; y < TextureSize; ++y)
				{
					// store color information from image to texture vector
					Texture[i][x][y] = TextureIMGLB.GetPixel(x, y);
				}
			}

			TextureIMGLB.Release(); 

			++i;
		}

		LevelTexturesDataFile.close();
	}
}