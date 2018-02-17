# Raycaster
A (currently) unnamed raycasting game engine in C++

I´m working on a 2.5D, "Wolfenstein"-like game engine, written in modern C++.

Currently I´m developing in Visual Studio 2017, for Windows as the target platform.

The graphics-part of the engine is totally CPU-based, using Windows GDI+ and GDI, setting pixels via Lockbits, which is extremely fast.

The engine is able to draw 1024x768 at 140fps on an Intel Haswell Core i5-4440, only limited by memory bandwidth.

There is a long way to go with the engine, from entity AI to weapon handling, level editor etc. The project is still at the very beginning, although the render engine itself is stable and powerful, but lacking features like doors etc., which will come in the near future.

The whole engine is modular and written in C++ v11+, which will make it easy to port the engine to other platforms.

Current features :

  - variable texture size (from 128x128 to 1024x1024)
  - entities and weapons implemented as classes
  - really large map size if desired
  - maps, weapons and entities can be defined in textfiles which will be loaded into program
  - complete separation of content/config from code
  - fixed timestep gameloop with 60fps framelock
  - multidirectional sprites with basic automovement
  - multi-threaded rendering of graphics with dynamic resolving of concurrent threads and use of a threadpool via IOService
  - collision detection between walls, entities and player
  - per-pixel softwareshading for viewsize-dependent darkening of environment
  - movement via mouselook (x-axis) and wasd (including strafing)
  - adjustable mouse sensitivity
  - basic audio functions (background music, steps, weapon sound)
  - basic weapon behaviour (sway, muzzleflash)
  - realtime minimap showing entity positions
  - basic implementation of HUD / Crosshair
  - fps (frames per second) counter

Used technologies:

  - boost libraries for easy handling of multithreading (http://www.boost.org/)
  - Lockbits for fast pixel access (https://msdn.microsoft.com/de-de/library/5ey6h79d(v=vs.110).aspx)
  - RAWINPUT for precise and fast mousehandling (https://msdn.microsoft.com/de-de/library/windows/desktop/ms645565(v=vs.85).aspx)
  - Audio output on MCI-Device (Media Control Interface, https://msdn.microsoft.com/de-de/library/windows/desktop/dd757151.aspx)
