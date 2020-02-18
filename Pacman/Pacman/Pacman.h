#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif
#define MUNCHIECOUNT 50
#define GHOSTCOUNT 4

// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

// Structure Definition
struct Player
{
	Vector2* position;
	Rect* sourceRect;
	Texture2D* texture;
	int direction;
	float speed;
	float speedMultiplier;
	//Data for animation
	int frame;
	int currentFrameTime;
	int frameTime;
	bool dead;
};

struct MovingEnemy
{
	Vector2* position;
	Texture2D* texture;
	Rect* sourceRect;
	int direction;
	float speed;
};

struct Enemy
{
	int frameCount;
	Rect* rect;
	Texture2D* texture;
	Vector2* position;
	Rect* sourceRect;
	//Data for animation
	int frame;
	int frameTime;
	int currentFrameTime;
};

struct Menu
{
	Texture2D* background;
	Rect* rectangle;
	Vector2* stringPosition;

	bool paused;
	bool pKeyDown;
	bool gameStart;
	bool spaceKeyDown;
};

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Pacman : public Game
{
private:
	void Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState);
	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);
	void CheckViewPortCollision();
	void UpdatePacman(int elapsedTime);
	void UpdateMunchie(Enemy& munchie, int elapsedTime);
	void UpdateCherry(Enemy* cherry, int elapsedTime);
	void CheckGhostCollisions();
	void UpdateGhost(MovingEnemy*, int elapsedTime);

	Player* _pacman;
	Enemy* _munchie;
	Enemy* _cherry;
	MovingEnemy* _ghosts[GHOSTCOUNT];
	Menu* _menu;
	SoundEffect* _pop;

	// Position for String
	Vector2* _stringPosition;
public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);
};