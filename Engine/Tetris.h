#pragma once

#include <string>
#include <array>
#include <vector>
#include <thread>
#include <random>
#include <chrono>

#include "Block.h"
#include "Keyboard.h"

class Tetris
{
public:
	Tetris(Keyboard& kbd, Graphics& gfx);
	~Tetris();
public:
	void Setup();
	void Update();
	void Draw();
private:
	void SetBlocks();
	void SetTetrominos();
	void ClearAndResetPlayingField();
	void SetNext();
	void SetTextPause();
	void SetTextGameOver();
	void UpdateNext();
	int Rotate(int px, int py, int r);
	bool DoesTetrisFit(int tetrisID, int rotation, int posX, int posY);
	
private: 
	Keyboard& kbd;
	Graphics& gfx;

	static constexpr unsigned int	width = 12;
	static constexpr unsigned int	height = 18;
	static constexpr unsigned int	blockWidth = 15;
	static constexpr unsigned int	blockHeight = 15;

	const unsigned int	offsetWidth = (gfx.ScreenWidth / 2) - ((width / 2)*blockWidth);
	const unsigned int	offsetHeight = (gfx.ScreenHeight / 2) - ((height / 2)*blockHeight);

	std::string			tetromino[7];
	std::string			text_Pause;
	std::string			text_GameOver;
	std::vector<int>	lines;

	char*	blockBuffer_Fixed	= nullptr;
	char*	blockBuffer_Shown	= nullptr;

	Block*	blocks				= nullptr;
	Block*	blocks_next			= nullptr;
	Block*	blocks_pauseText	= nullptr;
	Block*	blocks_gameOverText = nullptr;

	bool	key[4];
	bool	keyIsPressed	= false;
	bool	spaceIsPressed	= false;
	bool	forceDown		= false;

	int		speed			= 20;
	int		score			= 0;
	int		tetrisCounter	= 0;
	int		speedCounter	= 0;

	int		nextTetris		= 0;
	int		currentTetris	= 0;
	int		currentRotation = 0;
	int		currentX		= width / 2 - 2;
	int		currentY		= 0;

	bool	gameIsPaused	= false;
	bool	gameIsOver		= false;
};
