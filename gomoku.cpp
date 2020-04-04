﻿#include "gomoku.h"

Gomoku::Gomoku(int board_length)
	: board_length_{ board_length },
	turn_{ black },
	board_{ board_length * board_length,none },
	last_move_{ -1,-1 }
{
	
}

bool Gomoku::PieceOnBoard(int x, int y)
{
	//check empty and set
	if (board_[x + y * board_length_] == none) {
		board_[x + y * board_length_] = turn_;

		//record last move to CheckWin()
		last_move_ = { x,y };

		return true;
	}
	return false;
}

bool Gomoku::CheckWin() {
	int x{ last_move_.x };
	int y{ last_move_.y };
	int connected_piece{ 1 };

	//check vertical 
	for (int i = 1; board(x, y-i) == turn_; i++) 
		connected_piece++;
	for (int i = 1; board(x, y+i) == turn_; i++) 
		connected_piece++;
	
	if (connected_piece >= 5)
		return true;

	//check diagonal right up
	connected_piece = 1;
	for (int i = 1; board(x+i, y - i) == turn_; i++) 
		connected_piece++;
	for (int i = 1; board(x-i, y + i) == turn_; i++) 
		connected_piece++;
	
	if (connected_piece >= 5)
		return true;

	//check diagonal left up
	connected_piece = 1;
	for (int i = 1; board(x - i, y - i) == turn_; i++)
		connected_piece++;
	for (int i = 1; board(x + i, y + i) == turn_; i++)
		connected_piece++;

	if (connected_piece >= 5)
		return true;

	//check horizontal
	connected_piece = 1;
	for (int i = 1; board(x+i, y ) == turn_; i++)
		connected_piece++;
	for (int i = 1; board(x-i, y ) == turn_; i++)
		connected_piece++;

	if (connected_piece >= 5)
		return true;

	return false;
}

void Gomoku::reset() {
	for (size_t i = 0; i < board_.size(); i++) 
		board_[i] = none;
	
	turn_ = black;
}
