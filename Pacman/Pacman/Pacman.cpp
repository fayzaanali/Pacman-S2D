#include "Pacman.h"
#include <sstream>
#include <time.h>
#include <iostream>

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv)
{
	_pacman = new Player();

	_pacman->frame = 0;
	_pacman->currentFrameTime = 0;
	_pacman->frameTime = 250;
	_pacman->direction = 0;
	_pacman->speed = 0.1f;
	_pacman->speedMultiplier = 1.0f;
	_pacman->dead = false;

	_cherry = new Enemy();

	_cherry->frame = 0;
	_cherry->currentFrameTime = 0;
	_cherry->frameCount;
	_cherry->frameTime = 250;

	srand(time(NULL));

	_munchie = new Enemy[MUNCHIECOUNT];

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchie[i].frame = rand() % 1;
		_munchie[i].currentFrameTime = 0;
		_munchie[i].frameCount;
		_munchie[i].frameTime = rand() % 500 + 50;
	}

	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_ghosts[i] = new MovingEnemy();

		_ghosts[i]->direction = 0;
		_ghosts[i]->speed = 0.08f;
	}

	_menu = new Menu();

	_menu->paused = false;
	_menu->pKeyDown = false;
	_menu->gameStart = true;
	_menu->spaceKeyDown = false;

	//Initialise important Game aspects
	Audio::Initialise();
	_pop = new SoundEffect();
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	delete _pacman->texture;
	delete _pacman->sourceRect;
	delete _pacman->position;
	delete _pacman;

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		delete _munchie[i].position;
		delete _munchie[i].sourceRect;
		delete _munchie[i].texture;
	}

	delete[] _munchie;

	delete _cherry->texture;
	delete _cherry->position;
	delete _cherry->sourceRect;
	delete _cherry;

	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		delete _ghosts[i]->texture;
		delete _ghosts[i]->position;
		delete _ghosts[i]->sourceRect;
	}
	delete[] _ghosts;

	delete _pop;
}

void Pacman::LoadContent()
{
	// Load Pacman
	_pacman->texture = new Texture2D();
	_pacman->texture->Load("Textures/PacmanV2.tga", false);
	_pacman->position = new Vector2(350.0f, 350.0f);
	_pacman->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// Load Cherry
	_cherry->texture = new Texture2D();
	_cherry->texture->Load("Textures/CherrySprite.png", true);
	_cherry->position = new Vector2(350.0f, 350.0f);
	_cherry->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// Load Munchie
	bool munchie = false;
	Texture2D* munchieTex = new Texture2D();
	munchieTex->Load("Textures/MunchieV2.png", true);
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchie[i].texture = munchieTex;
		_munchie[i].position = new Vector2(rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight());
		_munchie[i].sourceRect = new Rect(0.0f, 0.0f, 12, 12);
		/*std::cout << "_munchie[i].position " << _munchie[i].position << endl;*/
		while (!munchie)
		{
			munchie = true;
			Vector2* munchiePosition = new Vector2(rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight());
			/*std::cout << i << " munchiePosition " << munchiePosition << endl;*/
			if (_munchie[i].position == munchiePosition)
			{
				munchie == false;
			}
		}
	}

	
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_ghosts[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		_ghosts[i]->sourceRect = new Rect(0.0f, 0.0f, 20, 20);
		_ghosts[0]->texture = new Texture2D();
		_ghosts[0]->texture->Load("Textures/GhostBlue.png", false);
		_ghosts[1]->texture = new Texture2D();
		_ghosts[1]->texture->Load("Textures/GhostRed.png", false);
		_ghosts[2]->texture = new Texture2D();
		_ghosts[2]->texture->Load("Textures/GhostPink.png", false);
		_ghosts[3]->texture = new Texture2D();
		_ghosts[3]->texture->Load("Textures/GhostYellow.png", false);
	}

	// Load Sound
	_pop->Load("Sound/pop.wav");

	if (!Audio::IsInitialised())
	{
		std::cout << "Audio is not initialised" << std::endl;
	}
	else
	{
		std::cout << "Audio is initialised" << std::endl;
	}

	if (!_pop->IsLoaded())
	{
		std::cout << "_pop member sound effect has not loaded" << std::endl;
	} 
	else 
	{
		std::cout << "Audio is loaded" << std::endl;
	}

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	// Set menu parameters
	_menu->background = new Texture2D();
	_menu->background->Load("Textures/Transparency.png", false);
	_menu->rectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_menu->stringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);
}

void Pacman::Update(int elapsedTime)
{
	// Gets the current state of the mouse
	Input::MouseState* mouseState = Input::Mouse::GetState();

	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();

	if (keyboardState->IsKeyDown(Input::Keys::SPACE))
		_menu->gameStart = false;

	if (_menu->gameStart == false)
	{
		CheckPaused(keyboardState, Input::Keys::P);

		if (!_menu->paused)
		{
			Input(elapsedTime, keyboardState, mouseState);
			UpdatePacman(elapsedTime);
			CheckViewPortCollision();
			UpdateCherry(_cherry, elapsedTime);
			for (int i = 0; i < MUNCHIECOUNT; i++)
			{
				UpdateMunchie(*_munchie, elapsedTime);
			}
			for (int i = 0; i < GHOSTCOUNT; i++)
			{
				UpdateGhost(_ghosts[i], elapsedTime);
			}
			CheckGhostCollisions();
		}
	}
}

void Pacman::Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState)
{
	float pacmanSpeed = _pacman->speed * elapsedTime * _pacman->speedMultiplier;
	if (mouseState->LeftButton == Input::ButtonState::PRESSED)
	{
		_cherry->position->X = mouseState->X;
		_cherry->position->Y = mouseState->Y;
	}

	if (state->IsKeyDown(Input::Keys::LEFTSHIFT))
	{
		_pacman->speedMultiplier = 2.0f;
	}
	else 
	{
		_pacman->speedMultiplier = 1.0f;
	}

	if (state->IsKeyDown(Input::Keys::R))
	{
		srand(time(NULL));
		_cherry->position = new Vector2(rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight());
	}

	if (state->IsKeyDown(Input::Keys::W))
	{
		_pacman->position->Y -= pacmanSpeed;
		_pacman->direction = 3;
	} else
	if (state->IsKeyDown(Input::Keys::S))
	{
		_pacman->position->Y += pacmanSpeed;
		_pacman->direction = 1;
	} else
	if (state->IsKeyDown(Input::Keys::D))
	{
		_pacman->position->X += pacmanSpeed;
		_pacman->direction = 0;
	} else
	if (state->IsKeyDown(Input::Keys::A))
	{
		_pacman->position->X -= pacmanSpeed;
		_pacman->direction = 2;
	}
}

void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
{
	// Checks if P key is pressed for Pause Menu
	if (state->IsKeyDown(Input::Keys::P) && !_menu->pKeyDown)
	{
		_menu->pKeyDown = true;
		_menu->paused = !_menu->paused;
	}

	if (state->IsKeyUp(Input::Keys::P))
		_menu->pKeyDown = false;
}

void Pacman::CheckViewPortCollision()
{
	//Checks if Pacman is trying to disappear (Right)
	if (_pacman->position->X + _pacman->sourceRect->Width > Graphics::GetViewportWidth())
	{
		_pacman->position->X = 30 - _pacman->sourceRect->Width;
		_pacman->sourceRect->X = _pacman->sourceRect->Height * _pacman->direction;
		Audio::Play(_pop);

	}

	//Checks if Pacman is trying to disappear (Left)
	if (_pacman->position->X + _pacman->sourceRect->Width < 30)
	{
		_pacman->position->X = Graphics::GetViewportWidth() - _pacman->sourceRect->Width;
		Audio::Play(_pop);

	}

	//Checks if Pacman is trying to disappear (Bottom)
	if (_pacman->position->Y + _pacman->sourceRect->Height > Graphics::GetViewportHeight())
	{
		_pacman->position->Y = 30 - _pacman->sourceRect->Height;
		Audio::Play(_pop);
	}

	//Checks if Pacman is trying to disappear (Top)
	if (_pacman->position->Y + _pacman->sourceRect->Height < 30)
	{
		_pacman->position->Y = Graphics::GetViewportHeight() - _pacman->sourceRect->Height;
		Audio::Play(_pop);
	}
}

void Pacman::UpdatePacman(int elapsedTime)
{
	// Updates Current Frame
	_pacman->currentFrameTime += elapsedTime;

	// Updates the _pacmanFrame variable
	if (_pacman->currentFrameTime > _pacman->frameTime)
	{
		_pacman->frame++;

		if (_pacman->frame >= 2)
			_pacman->frame = 0;

		_pacman->currentFrameTime = 0;
	}

	//Change X of component source rectangle
	_pacman->sourceRect->X = _pacman->sourceRect->Width * _pacman->frame;

	//Change Y of component source rectangle 
	_pacman->sourceRect->Y = _pacman->sourceRect->Height * _pacman->direction;
}

void Pacman::UpdateMunchie(Enemy& munchie, int elapsedTime)
{
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		// Updates Current Frame
		_munchie[i].currentFrameTime += elapsedTime;

		// Updates the _munchieFrame variable
		if (_munchie[i].currentFrameTime > _munchie[i].frameTime)
		{
			_munchie[i].frame++;

			if (_munchie[i].frame >= 2)
				_munchie[i].frame = 0;

			_munchie[i].currentFrameTime = 0;
		}

		//Change X of component source rectangle
		_munchie[i].sourceRect->X = _munchie[i].sourceRect->Width * _munchie[i].frame;
	}
}

void Pacman::UpdateCherry(Enemy* cherry, int elapsedTime)
{
	// Updates Current Frame
	_cherry->currentFrameTime += elapsedTime;

	// Updates the _munchieFrame variable
	if (_cherry->currentFrameTime > _cherry->frameTime)
	{
		_cherry->frame++;

		if (_cherry->frame >= 2)
			_cherry->frame = 0;

		_cherry->currentFrameTime = 0;
	}

	//Change X of component source rectangle
	_cherry->sourceRect->X = _cherry->sourceRect->Width * _cherry->frame;
}

void Pacman::UpdateGhost(MovingEnemy* ghost, int elapsedTime)
{
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		if (_ghosts[i]->direction == 0)
		{
			_ghosts[i]->position->X += _ghosts[i]->speed * elapsedTime;
		}
		else if (_ghosts[i]->direction == 1)
		{
			_ghosts[i]->position->X -= _ghosts[i]->speed * elapsedTime;
		}

		if (_ghosts[i]->position->X + _ghosts[i]->sourceRect->Width >= Graphics::GetViewportWidth())
		{
			_ghosts[i]->direction = 1;
		}
		else if (_ghosts[i]->position->X <= 0)
		{
			_ghosts[i]->direction = 0;
		}
	}
}

void Pacman::CheckGhostCollisions()
{
	int i = 0;
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->position->X;
	int left2 = 0;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->position->Y;
	int top2 = 0;

	for (i = 0; i < GHOSTCOUNT; i++)
	{
		bottom2 = _ghosts[i]->position->Y + _ghosts[i]->sourceRect->Height;
		left2 = _ghosts[i]->position->X;
		right2 = _ghosts[i]->position->X + _ghosts[i]->sourceRect->Width;
		top2 = _ghosts[i]->position->Y;

		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			_pacman->dead = true;
			i = GHOSTCOUNT;
		}
	}
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Pacman X: " << _pacman->position->X << " Y: " << _pacman->position->Y;
	// Starts Drawing
	SpriteBatch::BeginDraw();

	// Draws Pacman
	SpriteBatch::Draw(_pacman->texture, _pacman->position, _pacman->sourceRect);

	// Draw Cherry
	SpriteBatch::Draw(_cherry->texture, _cherry->position, _cherry->sourceRect);

	// Draw Ghost
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		SpriteBatch::Draw(_ghosts[i]->texture, _ghosts[i]->position, _ghosts[i]->sourceRect);
	}

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		SpriteBatch::Draw(_munchie[i].texture, _munchie[i].position, _munchie[i].sourceRect);
	}

	if (!_pacman->dead)
	{
		SpriteBatch::Draw(_pacman->texture, _pacman->position, _pacman->sourceRect);
	}
	else {
		_pacman->position = new Vector2(9999.0f, 9999.0f);
		_menu->paused = true;
		std::stringstream deadStream;
		deadStream << "You died! Game Over!";
		SpriteBatch::Draw(_menu->background, _menu->rectangle, nullptr);
		SpriteBatch::DrawString(deadStream.str().c_str(), _menu->stringPosition, Color::Red);
	}

	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);

	if (_menu->paused && !_pacman->dead)
	{
		std::stringstream menuStream;
		menuStream << "PAUSED!";

		SpriteBatch::Draw(_menu->background, _menu->rectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menu->stringPosition, Color::Red);
	}

	if (_menu->gameStart)
	{
		std::stringstream menuStream;
		menuStream << "Press SPACE to start game";

		SpriteBatch::Draw(_menu->background, _menu->rectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menu->stringPosition, Color::Green);
	}

	// Ends Drawing
	SpriteBatch::EndDraw();
}