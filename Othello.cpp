#include <bangtal>
#include <iostream>
using namespace std;
using namespace bangtal;


ObjectPtr board[8][8];

enum class State {
	BLANK,
	POSSIBLE,
	BLACK,
	WHITE
};
State board_state[8][8];

enum class Turn {
	BLACK,
	WHITE
};
Turn turn = Turn::BLACK;

void setState(int x, int y, State state)
{
	switch (state) {
	  case State::BLANK: board[y][x]->setImage("Images/blank.png");break;
	  case State::POSSIBLE: board[y][x]->setImage(turn == Turn::BLACK? "Images/black possible.png" : "Images/white possible.png");break;
	  case State::BLACK: board[y][x]->setImage("Images/black.png");break;
	  case State::WHITE: board[y][x]->setImage("Images/white.png");break;
	}
	board_state[y][x] = state;
}

bool  checkPossible(int x, int y, int dx, int dy)
{
	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;

	bool possible = false;

	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
		if (board_state[y][x] == other) {
			possible = true;
		}
		else if (board_state[y][x] == self) {
			return possible;
		}
		else {
			return false;
		}
	}

	return false;
}


bool checkPossible(int x, int y) {
	if (board_state[y][x] == State::BLACK) return false;
	if (board_state[y][x] == State::WHITE) return false;
	setState(x, y, State::BLANK);

	int delta[8][2] = {
		{0, 1},
		{1, 1},
		{1, 0},
		{1, -1},
		{0, -1},
		{-1, -1},
		{-1, 0},
		{-1, 1}
	};

	bool possible = false;
	for (auto d : delta) {
		if (checkPossible(x, y, d[0], d[1])) possible = true;
	}
	return possible;
}

void reverse(int x, int y, int dx, int dy) 
{
	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;

	bool possible = false;

	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
		if (board_state[y][x] == other) {
			possible = true;
		}
		else if (board_state[y][x] == self) {
			if (possible) {
				for (x -= dx, y -= dy; x >= 0 && x < 8 && y >= 0 && y < 8; x -= dx, y -= dy) {
					if (board_state[y][x] == other) {
						setState(x, y, self);
					}
					else {
						return;
					}
				}
			}
		}
		else {
			return;
		}
	}
}

void reverse(int x, int y) {
	
	int delta[8][2] = {
	  {0, 1},
	  {1, 1},
	  {1, 0},
	  {1, -1},
	  {0, -1},
	  {-1, -1},
	  {-1, 0},
	  {-1, 1}
	};

	bool possible = false;
	for (auto d : delta) {
		reverse(x, y, d[0], d[1]);
	}
}

bool setPossible()
{
	bool possible = false;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (checkPossible(x, y)) {
				setState(x, y, State::POSSIBLE);
				possible = true;
			}
		}
	}
	return possible;

}

ObjectPtr board_score[4];

void score() {
	int black = 0, white = 0;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (board_state[y][x] == State::BLACK) {
				black += 1;
			}
			else if (board_state[y][x] == State::WHITE) {
				white += 1;
			}
		};
	};
	string filename1 = "images/L" + to_string(black / 10) + ".png";
	string filename2 = "images/L" + to_string(black % 10) + ".png";
	string filename3 = "images/L" + to_string(white / 10) + ".png";
	string filename4 = "images/L" + to_string(white % 10) + ".png";
	board_score[0]->setImage(filename1);
	board_score[1]->setImage(filename2);
	board_score[2]->setImage(filename3);
	board_score[3]->setImage(filename4);

}


int main()
{
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

	
	auto scene = Scene::create("Othello", "Images/background.png");

	board_score[0] = Object::create("images/L0.png", scene, 750, 230);
	board_score[1] = Object::create("images/L2.png", scene, 820, 230);
	board_score[2] = Object::create("images/L0.png", scene, 1080, 230);
	board_score[3] = Object::create("images/L2.png", scene, 1150, 230);


	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			board[y][x] = Object::create("Images/blank.png", scene, 40 + x * 80, 40 + y * 80);
			board[y][x]->setOnMouseCallback([&, x, y](
				ObjectPtr object, int, int, MouseAction)-> bool {
					if (board_state[y][x] == State::POSSIBLE) {
						if (turn == Turn::BLACK) {
							setState(x, y, State::BLACK);
							reverse(x, y);
							score();
							turn = Turn::WHITE;
						}
						else {
							setState(x, y, State::WHITE);
							reverse(x, y);
							score();
							turn = Turn::BLACK;
						}
						if (!setPossible()) {
							score();
							turn = turn == Turn::BLACK ? Turn::WHITE : Turn::BLACK;

							if (!setPossible()) {
								score();
								turn = turn == Turn::BLACK ? Turn::WHITE : Turn::BLACK;

								if (!setPossible()) {
									showMessage("End Eame!!");
								}
							}
						};
					}
					return true;
				});
			board_state[y][x] = State::BLANK;
		}
	};



	setState(3, 3, State::BLACK);
	setState(4, 4, State::BLACK);
	setState(3, 4, State::WHITE);
	setState(4, 3, State::WHITE);

	setPossible();

	startGame(scene);
}