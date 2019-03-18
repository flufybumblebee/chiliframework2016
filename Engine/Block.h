#pragma once

#include "Graphics.h"

class Block
{
public:
	Block() = default;
	Block(const int& left, const int& top, const int& width, const int& height, const Color color)
		:
		left(left),
		top(top),
		width(width),
		height(height),
		color(color)
	{}
public:
	void Set(const int& left, const int& top, const int& width, const int& height, const char& color)
	{
		this->left = left;
		this->top = top;
		this->width = width;
		this->height = height;
		SetColor(color);
	}
	void SetColor(const char& color)
	{
		switch (color)
		{
		case ' ':
			this->color = Colors::Black;
			break;
		case 'A':
			this->color = Colors::Orange;
			break;
		case 'B':
			this->color = Colors::Green;
			break;
		case 'C':
			this->color = Colors::Cyan;
			break;
		case 'D':
			this->color = Colors::Red;
			break;
		case 'E':
			this->color = Colors::Yellow;
			break;
		case 'F':
			this->color = Colors::Blue;
			break;
		case 'G':
			this->color = Colors::Magenta;
			break;
		case '=':
			this->color = Colors::Red;
			break;
		case '#':
			this->color = Colors::White;
			break;
		default:
			break;
		}
	}
	void Draw(Graphics& gfx)
	{
		gfx.DrawRect(true, left, top, width, height, color);
	}
private:
	int left;
	int top;
	int width;
	int height;
	Color color;
};

