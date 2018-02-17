/*
***********************************
* Data Structure Definitions      *
*                                 *
* DataStructures.h                *
*                                 *
* (c) 2017, 2018 Stefan Kubsch    *
***********************************
*/

#pragma once

//
// Structure for entities
//

struct EntityStruct
{
	uint_fast32_t Number;
	std::string Name;
	float StartPosX;
	float StartPosY;
	float EndPosX;
	float EndPosY;
	float MoveSpeed;
	uint_fast32_t MovementBehaviour;
	float DirX;
	float DirY;
	float PosX;
	float PosY;
	uint_fast32_t WaitTimer;
	std::vector<std::vector<std::vector<uint_fast32_t>>> Texture;
};


//
// Structure for weapons
//

struct WeaponStruct
{
	uint_fast32_t Number;
	std::string Name;
	uint_fast32_t PosX;
	uint_fast32_t PosY;
	float Weight;
	uint_fast32_t Cadence;
	uint_fast32_t Width;
	uint_fast32_t Height;
	float PaceFactor;
	uint_fast32_t MuzzleFlashDuration;
	uint_fast32_t MuzzleFlashPosX;
	uint_fast32_t MuzzleFlashPosY;
	uint_fast32_t MuzzleFlashWidth;
	uint_fast32_t MuzzleFlashHeight;
	std::vector<std::vector<std::vector<uint_fast32_t>>> Texture;
	std::vector<std::vector<uint_fast32_t>> MuzzleFlashTexture;
};
