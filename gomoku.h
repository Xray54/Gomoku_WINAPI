#pragma once

#include <vector>
using namespace std;

struct Point_ {
	int x;
	int y;
};

class Gomoku {
public:
	enum piece { none, black, white };
	Gomoku(int board_length = 19);

	//order
	//PieceOnBoard -> CheckWin -> PassTurn

	// return true when success
	bool PieceOnBoard(int x,int y);

	//Check base on last move
	bool CheckWin();

	//pass turn
	void PassTurn() {
		if (turn_ == black)
			turn_ = white;
		else
			turn_ = black;
	}
	
	piece getturn() const { return turn_; }
	int board_length() const { return board_length_; }
	vector<piece> board() const { return board_; }

private:
	vector<piece> board_;
	piece board(int x, int y) {
		if (x < 0 || board_length_ < x ||
			y < 0 || board_length_ < y) {
			return none;
		}
		return board_[x + y * board_length_];
	}
	piece turn_;
	int board_length_;

	Point_ last_move_;

};