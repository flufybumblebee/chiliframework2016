#include "Tetris.h"

Tetris::Tetris(Keyboard& kbd, Graphics& gfx)
	:
	kbd(kbd),
	gfx(gfx)
{
	key[0] = false;
	key[1] = false;
	key[2] = false;
	key[3] = false;

	blockBuffer_Fixed = new char[width * height];
	blockBuffer_Shown = new char[width * height];

	blocks				= new Block[width * height];
	blocks_next			= new Block[4 * 4];
	blocks_pauseText	= new Block[35 * 7];
	blocks_gameOverText = new Block[29 * 13];

	{
		std::mt19937 rng;
		rng.seed(std::random_device()());
		std::uniform_int_distribution<std::mt19937::result_type> dist(0, 6); // distribution in range [1, 6]
		currentTetris = dist(rng);
	}

	{
		std::mt19937 rng;
		rng.seed(std::random_device()());
		std::uniform_int_distribution<std::mt19937::result_type> dist(0, 6); // distribution in range [1, 6]
		nextTetris = dist(rng);
	}
}

Tetris::~Tetris()
{
	delete[] blockBuffer_Fixed;
	blockBuffer_Fixed = nullptr;

	delete[] blockBuffer_Shown;
	blockBuffer_Shown = nullptr;

	delete[] blocks;
	blocks = nullptr;

	delete[] blocks_next;
	blocks_next = nullptr;

	delete[] blocks_pauseText;
	blocks_pauseText = nullptr;

	delete[] blocks_gameOverText;
	blocks_gameOverText = nullptr;
}

void Tetris::Setup()
{
	SetBlocks();		// set once at the start
	SetTetrominos();	// set once at the start
	SetTextPause();		// set once at the start
	SetTextGameOver();	// set once at the start

	ClearAndResetPlayingField();
	SetNext();			// used every time a new tetromino is needed
}

void Tetris::Update()
{
	if (!spaceIsPressed)
	{
		if (kbd.KeyIsPressed(VK_SPACE) && !gameIsPaused)
		{
			if (gameIsOver)
			{
				ClearAndResetPlayingField();
				gameIsOver = false;

				currentX = width / 2 - 2;
				currentY = 0;
				currentRotation = 0;

				{
					std::mt19937 rng;
					rng.seed(std::random_device()());
					std::uniform_int_distribution<std::mt19937::result_type> dist(0, 6); // distribution in range [1, 6]
					currentTetris = dist(rng);
				}

				{
					std::mt19937 rng;
					rng.seed(std::random_device()());
					std::uniform_int_distribution<std::mt19937::result_type> dist(0, 6); // distribution in range [1, 6]
					nextTetris = dist(rng);
				}

				UpdateNext();
			}
			else
			{
				gameIsPaused = true;
			}
			spaceIsPressed = true;
		}
		else if (kbd.KeyIsPressed(VK_SPACE) && gameIsPaused)
		{
			gameIsPaused = false;
			spaceIsPressed = true;
		}
	}
	else
	{
		if (!kbd.KeyIsPressed(VK_SPACE))
		{
			spaceIsPressed = false;
		}
	}

	if (!gameIsPaused)
	{
		if (!gameIsOver)
		{
			// game timing
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			speedCounter++;
			forceDown = (speedCounter == speed);

			// input

			if (kbd.KeyIsPressed(VK_LEFT))	key[0] = true;
			else							key[0] = false;

			if (kbd.KeyIsPressed(VK_RIGHT))	key[1] = true;
			else							key[1] = false;

			if (kbd.KeyIsPressed(VK_DOWN))	key[2] = true;
			else							key[2] = false;

			if (kbd.KeyIsPressed(VK_UP))	key[3] = true;
			else							key[3] = false;

			currentX -= (key[0] && DoesTetrisFit(currentTetris, currentRotation, currentX - 1, currentY + 0)) ? 1 : 0;
			currentX += (key[1] && DoesTetrisFit(currentTetris, currentRotation, currentX + 1, currentY + 0)) ? 1 : 0;
			currentY += (key[2] && DoesTetrisFit(currentTetris, currentRotation, currentX + 0, currentY + 1)) ? 1 : 0;

			// block up/rotate key from being pressed every frame
			if (key[3])
			{
				currentRotation += (!keyIsPressed && key[3] && DoesTetrisFit(currentTetris, currentRotation + 1, currentX, currentY)) ? 1 : 0;
				keyIsPressed = true;
			}
			else
			{
				keyIsPressed = false;
			}

			if (forceDown)
			{
				// update difficulty/speed every 50 tetris played
				speedCounter = 0;
				tetrisCounter++;
				if (tetrisCounter % 50 == 0)
				{
					if (speed >= 10) speed--;
				}

				if (DoesTetrisFit(currentTetris, currentRotation, currentX, currentY + 1))
				{
					currentY++; // force tetris down
				}
				else
				{
					// add tetris to fixed field buffer
					for (int px = 0; px < 4; px++)
					{
						for (int py = 0; py < 4; py++)
						{
							if (tetromino[currentTetris][Rotate(px, py, currentRotation)] == L'X')
							{
								blockBuffer_Fixed[(currentY + py) * width + (currentX + px)] =
									currentTetris + 1;
							}
						}
					}

					// check for lines
					for (int py = 0; py < 4; py++)
					{
						if (currentY + py < height - 1)
						{
							bool isLine = true;
							for (int px = 1; px < width - 1; px++)
							{
								isLine &= (blockBuffer_Fixed[(currentY + py) * width + px]) != 0;
							}

							if (isLine)
							{
								// remove line, set to =
								for (int px = 1; px < width - 1; px++)
								{
									blockBuffer_Fixed[(currentY + py) * width + px] = 8;

								}
								lines.push_back(currentY + py);
							}
						}
					}

					// calculate score
					score += 25;
					if (!lines.empty()) score += (1 << lines.size()) * 100;

					// randomly assign new tetris

					currentX = width / 2 - 2;
					currentY = 0;
					currentRotation = 0;

					currentTetris = nextTetris;

					std::mt19937 rng;
					rng.seed(std::random_device()());
					std::uniform_int_distribution<std::mt19937::result_type> dist(0, 6); // distribution in range [1, 6]
					nextTetris = dist(rng);

					UpdateNext();

					// if tetris doesn't fit...
					gameIsOver = !DoesTetrisFit(currentTetris, currentRotation, currentX, currentY);
				}
			}

			// add all fixed blocks to displayBuffer from fixedBuffer
			for (int x = 0; x < width; x++)
			{
				for (int y = 0; y < height; y++)
				{
					blockBuffer_Shown[y*width + x] =
						" ABCDEFG=#"[blockBuffer_Fixed[y*width + x]];
				}
			}

			// add current tetris piece to display buffer
			for (int x = 0; x < 4; x++)
			{
				for (int y = 0; y < 4; y++)
				{
					if (tetromino[currentTetris][Rotate(x, y, currentRotation)] != L'.')
					{
						blockBuffer_Shown[(currentY + y) * (width)+(currentX + x)] =
							currentTetris + 65;	// set to the ascii table value
					}
				}
			}

			// Animate Line Completion
			if (!lines.empty())
			{
				// i need to add a flashing effect to the blocks

				std::this_thread::sleep_for(std::chrono::milliseconds(800)); // delay

				for (auto &v : lines)
				{
					for (int px = 1; px < width - 1; px++)
					{
						for (int py = v; py > 0; py--)
						{
							blockBuffer_Fixed[py * width + px] = blockBuffer_Fixed[((py - 1)*width) + px];
						}
						blockBuffer_Fixed[px] = 0;
					}
				}

				lines.clear();
			}

			// set all displayed blocks colors
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					blocks[y*width + x].SetColor(blockBuffer_Shown[y*width + x]);
				}
			}
		}
	}
}

void Tetris::Draw()
{
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			blocks[y*width + x].Draw(gfx);
		}
	}

	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			blocks_next[y*4 + x].Draw(gfx);
		}
	}

	if (gameIsPaused)
	{
		for (int x = 0; x < 35; x++)
		{
			for (int y = 0; y < 7; y++)
			{
				blocks_pauseText[y * 35 + x].Draw(gfx);
			}
		}
	}

	if (gameIsOver)
	{
		for (int x = 0; x < 29; x++)
		{
			for (int y = 0; y < 13; y++)
			{
				blocks_gameOverText[y * 29 + x].Draw(gfx);
			}
		}
	}
}

void Tetris::SetBlocks()
{
	// set up block size and position on screen
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			blocks[y*width + x].Set(
				offsetWidth + (x * blockWidth),
				offsetHeight + (y* blockHeight),
				blockWidth,
				blockHeight,
				blockBuffer_Shown[y*width + x]);
		}
	}
}

void Tetris::SetTetrominos()
{
	tetromino[0].append("..X.");
	tetromino[0].append("..X.");
	tetromino[0].append("..X.");
	tetromino[0].append("..X.");

	tetromino[1].append("..X.");
	tetromino[1].append(".XX.");
	tetromino[1].append(".X..");
	tetromino[1].append("....");

	tetromino[2].append(".X..");
	tetromino[2].append(".XX.");
	tetromino[2].append("..X.");
	tetromino[2].append("....");

	tetromino[3].append("....");
	tetromino[3].append(".XX.");
	tetromino[3].append(".XX.");
	tetromino[3].append("....");

	tetromino[4].append("..X.");
	tetromino[4].append(".XX.");
	tetromino[4].append("..X.");
	tetromino[4].append("....");

	tetromino[5].append(".XX.");
	tetromino[5].append("..X.");
	tetromino[5].append("..X.");
	tetromino[5].append("....");

	tetromino[6].append(".XX.");
	tetromino[6].append(".X..");
	tetromino[6].append(".X..");
	tetromino[6].append("....");
}

void Tetris::ClearAndResetPlayingField()
{
	// set playing field border
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			blockBuffer_Fixed[y*width + x] =
				(x == 0 || x == width - 1 || y == height - 1) ? 9 : 0;

			//if (x == 0 || x == width - 1) field[y*width + x] = 9;
			//else if(y == height - 1) field[y*width + x] = 9;
			//else field[y*width + x] = 0;
		}
	}

	// set shown blocks buffer with fixed block buffer
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			blockBuffer_Shown[y*width + x] = blockBuffer_Fixed[y*width + x];
		}
	}
}

void Tetris::SetNext()
{
	char color;
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{			
			if (tetromino[nextTetris].at(y * 4 + x) == '.')
			{
				color = " ABCDEFG=#"[0];
			}
			else
			{
				color = " ABCDEFG=#"[nextTetris+1];
			}

			blocks_next[y*4 + x].Set(
				offsetWidth + (blockWidth * width) + (x * blockWidth),
				offsetHeight + (y * blockHeight),
				blockWidth,
				blockHeight,
				color);
		}
	}
}

void Tetris::SetTextPause()
{
	text_Pause.append("...................................");
	text_Pause.append("...XXXXX.XXXXX.X...X.XXXXX.XXXXX...");
	text_Pause.append("...X...X.X...X.X...X.X.....X.......");
	text_Pause.append("...X...X.X...X.X...X.XXXXX.XXXXX...");
	text_Pause.append("...XXXXX.XXXXX.X...X.....X.X.......");
	text_Pause.append("...X.....X...X.XXXXX.XXXXX.XXXXX...");
	text_Pause.append("...................................");

	char color;
	for (int x = 0; x < 35; x++)
	{
		for (int y = 0; y < 7; y++)
		{
			if (text_Pause.at(y * 35 + x) == '.')
			{
				color = " ABCDEFG=#"[0];
			}
			else
			{
				std::mt19937 rng;
				rng.seed(std::random_device()());
				std::uniform_int_distribution<std::mt19937::result_type> dist(0, 6);
				const int randCol = dist(rng);
				color = " ABCDEFG=#"[randCol + 1];
			}

			blocks_pauseText[y * 35 + x].Set(
				(gfx.ScreenWidth / 2) - ((35 / 2)*blockWidth) + (x * blockWidth),
				(gfx.ScreenHeight / 2) - ((7 / 2)*blockHeight) + (y * blockHeight),
				blockWidth,
				blockHeight,
				color);
		}
	}
}

void Tetris::SetTextGameOver()
{
	text_GameOver.append(".............................");
	text_GameOver.append("...XXXXX.XXXXX.XXXXX.XXXXX...");
	text_GameOver.append("...X.....X...X.X.X.X.X.......");
	text_GameOver.append("...X..XX.X...X.X.X.X.XXXXX...");
	text_GameOver.append("...X...X.XXXXX.X.X.X.X.......");
	text_GameOver.append("...XXXXX.X...X.X.X.X.XXXXX...");
	text_GameOver.append(".............................");
	text_GameOver.append("...XXXXX.X...X.XXXXX.XXXX....");
	text_GameOver.append("...X...X.X...X.X.....X...X...");
	text_GameOver.append("...X...X.X...X.XXXXX.X...X...");
	text_GameOver.append("...X...X..X.X..X.....XXXX....");
	text_GameOver.append("...XXXXX...X...XXXXX.X...X...");
	text_GameOver.append(".............................");

	char color;
	for (int x = 0; x < 29; x++)
	{
		for (int y = 0; y < 13; y++)
		{
			if (text_GameOver.at(y * 29 + x) == '.')
			{
				color = " ABCDEFG=#"[0];
			}
			else
			{
				std::mt19937 rng;
				rng.seed(std::random_device()());
				std::uniform_int_distribution<std::mt19937::result_type> dist(0, 6); // distribution in range [1, 6]
				const int randCol = dist(rng);
				color = " ABCDEFG=#"[randCol + 1];
			}

			blocks_gameOverText[y * 29 + x].Set(
				(gfx.ScreenWidth/2) - ((29/2)*blockWidth) + (x * blockWidth),
				(gfx.ScreenHeight/2) - ((13/2)*blockHeight) + (y * blockHeight),
				blockWidth,
				blockHeight,
				color);
		}
	}
}

void Tetris::UpdateNext()
{
	char color;
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			if (tetromino[nextTetris].at(y * 4 + x) == '.')
			{
				color = " ABCDEFG=#"[0];
			}
			else
			{
				color = " ABCDEFG=#"[nextTetris+1];
			}
			blocks_next[y * 4 + x].SetColor(color);
		}
	}
}

int Tetris::Rotate(int px, int py, int r)
{
	int pi = 0;

	switch (r % 4)
	{
	case 0: //   0 degrees 
		pi = 0 + (py * 4) + px;			//  0  1  2  3
		break;							//  4  5  6  7
										//  8  9 10 11
										// 12 13 14 15

	case 1: //  90 degrees
		pi = 12 + py - (px * 4);		// 12  8  4  0
		break;							// 13  9  5  1
										// 14 10  6  2
										// 15 11  7  3

	case 2: // 120 degrees
		pi = 15 - (py * 4) - px;		// 15 14 13 12
		break;							// 11 10  9  8
										//  7  6  5  4
										//  3  2  1  0

	case 3: // 270 degrees
		pi = 3 - py + (px * 4);			//  3  7 11 15
		break;							//  2  6 10 14
										//  1  5  9 13
										//  0  4  8 12
	}

	return pi;
}

bool Tetris::DoesTetrisFit(int tetrisID, int rotation, int posX, int posY)
{
	for (int px = 0; px < 4; px++)
	{
		for (int py = 0; py < 4; py++)
		{
			// get index into tetris
			int pi = Rotate(px, py, rotation);

			// get index into field
			int fi = (posY + py) * width + (posX + px);

			if (posX + px >= 0 && posX + px < width &&
				posY + py >= 0 && posY + py < height)
			{
				if (tetromino[tetrisID][pi] != L'.' && blockBuffer_Fixed[fi] != 0)
				{
					return false;
				}
			}
		}
	}
	return true;
}