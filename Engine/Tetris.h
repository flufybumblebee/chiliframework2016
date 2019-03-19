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
	void	Setup();
	void	Update();
	void	Draw();
private:
	void	SetBlocks();
	void	SetTetrominos();
	void	SetTextPause();
	void	SetTextGameOver();
	void	SetNext();
	void	UpdateNext();
	void	ClearAndResetPlayingField();
	int		Random(const int min, const int max);

	int Rotate(int px, int py, int r);
	bool DoesTetrisFit(int tetrisID, int rotation, int posX, int posY);
	
private: 
	Keyboard& kbd;
	Graphics& gfx;

	static constexpr unsigned int	width	= 12u;
	static constexpr unsigned int	height	= 18u;
	static constexpr unsigned int	blocksW = 15u;
	static constexpr unsigned int	blocksH = 15u;

	const unsigned int	offsetWidth = (gfx.ScreenWidth / 2) - ((width / 2)*blocksW);
	const unsigned int	offsetHeight = (gfx.ScreenHeight / 2) - ((height / 2)*blocksH);

	std::string			tetromino[7];
	std::string			text_Pause;
	std::string			text_GameOver;
	std::vector<int>	lines;
		
	static constexpr unsigned int pauseW	= 35u;
	static constexpr unsigned int pauseH	= 7u;
	static constexpr unsigned int gameOverW = 29u;
	static constexpr unsigned int gameOverH = 13u;

	std::array<Block, (width*height)>			blocks;
	std::array<Block, (4 * 4)>					blocks_Next;
	std::array<Block, (pauseW*pauseH)>			blocks_PauseText;
	std::array<Block, (gameOverW*gameOverH)>	blocks_GameOverText;

	std::array<char, (width*height)>			blockBuffer_Fixed;
	std::array<char, (width*height)>			blockBuffer_Shown;

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
