Level_x_Level_MapData.conf

	Describes the map.

	Zero means empty space, all other numbers represent a texture-number.
	Playfield has to be completely surrounded/closed.

	Size will be gathered during runtime.

	Example:

		1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1
		2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2
		1, 0, 0,11, 0, 0, 0, 0, 0, 0, 1
		2, 0, 0, 0, 0, 0, 7, 0, 0, 0, 2
		1, 0,10, 0, 0, 0, 7, 0, 0, 0, 1
		2, 0, 0, 0, 0, 0, 7, 0, 0, 0, 2
		1, 3, 3, 3, 0, 7, 7, 0, 0, 0, 1
		3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2
		1, 8,12,13,12, 0, 0, 0, 0, 0, 1
		2, 0, 0, 0, 0, 0, 0, 4, 0, 0, 2
		1, 0,12, 0, 0, 0, 0, 0, 0, 0, 1
		2, 0,13, 0, 0, 0, 0, 4, 0, 0, 2
		1, 0,12, 0, 0, 0, 0, 0, 0, 0, 1
		2, 0,13, 0, 0, 0, 4, 0, 4, 0, 2
		1, 0,12, 0, 0, 0, 0, 0, 0, 0, 9
		2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2 


______________________________________


Level_x_Level_TexturesData.conf

	Contains the filenames of the used textures.
	Need to be in the right order as used in "Level_x_MapData".
	
	Format:

		One filename each line

	Example:

		GFX\\LevelTextures\\Wall1.png
		GFX\\LevelTextures\\Wall2.png
		GFX\\LevelTextures\\Wall3.png
		GFX\\LevelTextures\\Wall4.png 
		GFX\\LevelTextures\\Floor1.png
		GFX\\LevelTextures\\Ceiling1.png
		GFX\\LevelTextures\\Crate1.png
		GFX\\LevelTextures\\Console1.png
		GFX\\LevelTextures\\Console2.png 
		GFX\\LevelTextures\\Pipes1.png 
		GFX\\LevelTextures\\Pipes2.png 
		GFX\\LevelTextures\\Vent1.png 
		GFX\\LevelTextures\\Vent2.png

