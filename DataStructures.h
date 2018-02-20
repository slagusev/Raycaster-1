/*
***********************************
* Data Structure Definitions      *
*                                 *
* DataStructures.h                *
*                                 *
* (c) 2017, 2018 Stefan Kubsch    *
***********************************
*/

//
// Structure for entities
// Tried tp "pad" the elements by their size..
//

struct EntityStruct
{
	std::vector<std::vector<std::vector<uint_fast32_t>>> Texture;
	std::string Name;
	uint_fast32_t Number;
	uint_fast32_t MovementBehaviour;
	uint_fast32_t WaitTimer;
	uint_fast32_t RotationFactor;
	float StartPosX;
	float StartPosY;
	float EndPosX;
	float EndPosY;
	float MoveSpeed;
	float DirX;
	float DirY;
	float PosX;
	float PosY;
	char Direction;
};

//
// Structure for weapons
// Tried tp "pad" the elements by their size..
//

struct WeaponStruct
{
	std::vector<std::vector<std::vector<uint_fast32_t>>> Texture;
	std::vector<std::vector<uint_fast32_t>> MuzzleFlashTexture;
	std::string Name;
	uint_fast32_t Number;
	uint_fast32_t PosX;
	uint_fast32_t PosY;
	uint_fast32_t Cadence;
	uint_fast32_t Width;
	uint_fast32_t Height;
	uint_fast32_t MuzzleFlashDuration;
	uint_fast32_t MuzzleFlashPosX;
	uint_fast32_t MuzzleFlashPosY;
	uint_fast32_t MuzzleFlashWidth;
	uint_fast32_t MuzzleFlashHeight;
	float Weight;
	float PaceFactor;
};
