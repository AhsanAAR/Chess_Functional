#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <time.h>
#include <fstream>
#include <iomanip>
#include <string>
#include <stdio.h>

#define Dimension 10
#define Rows 80
#define Cols 80

using namespace std;
const char block = -37;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

enum color { Grey = 8, Silver = 7, White = 3, Black = 0, Green = 10, Yellow = 14, Red = 12 };
enum pieces { Rook = 0, Knight = 1, Bishop = 2, Queen = 3, King = 4, Empty = 5, Pawn = 6 };

void gotoRowCol(int rpos, int cpos)
{
	COORD scrn;
	HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
	scrn.X = cpos;
	scrn.Y = rpos;
	SetConsoleCursorPosition(hOuput, scrn);
}

void getRowColbyLeftClick(int& rpos, int& cpos)
{
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD Events;
	INPUT_RECORD InputRecord;
	SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
	do
	{
		ReadConsoleInput(hInput, &InputRecord, 1, &Events);
		if (InputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			cpos = InputRecord.Event.MouseEvent.dwMousePosition.X;
			rpos = InputRecord.Event.MouseEvent.dwMousePosition.Y;
			break;
		}
	} while (true);
}

void printMsg(string msg, int R = 3, int C = 8 * Dimension + 10) {
	gotoRowCol(R, C);
	cout << "                                                             ";
	gotoRowCol(R, C);
	cout << msg;
}

struct pos {
	int x, y;
};

bool validSelect(int rPos, int cPos) {
	return rPos >= 0 && rPos < 8 && cPos >= 0 && cPos < 8;
}

enum Castle { No, Left, Right };

struct piece {
	color pieceColor;
	color backColor;
	pieces type;
	pos position;
	bool valid;
	bool castling = false;

	void promote(piece**& board) {
		char in;
		SetConsoleTextAttribute(hConsole, White);
		do {
			printMsg("Pawn Promotion!! Which piece do you want to replace: ");
			in = _getch();
			switch (in) {
			case 'R':
			case 'r':
				type = Rook; break;
			case 'N':
			case 'n':
				type = Knight; break;
			case 'B':
			case 'b':
				type = Bishop; break;
			case 'Q':
			case 'q':
				type = Queen; break;
			default:
				continue;
			}
			break;
		} while (true);
		SetConsoleTextAttribute(hConsole, 7);
		display();
		return;
	}

	void printBox(color C) {
		SetConsoleTextAttribute(hConsole, C);
		for (int ri = 0; ri < Dimension; ri++) {
			for (int ci = 0; ci < Dimension; ci++) {
				if (ri == 0 || ri == Dimension - 1 || ci == 0 || ci == Dimension - 1) {
					gotoRowCol((position.y * Dimension) + 3 + ri, (position.x * Dimension) + 3 + ci);
					cout << '*';
				}
			}
		}
	}

	bool markHorizontalVertical(piece**& board, bool mark = true) {
		bool ret = false;

		int x = position.x;
		int y = position.y;
		while (validSelect(--y, x) && (board[y][x].type == Empty || board[y][x].pieceColor != pieceColor)) {
			board[y][x].valid = true;
			if (mark)
				board[y][x].display();
			if (board[y][x].type != Empty && board[y][x].pieceColor != pieceColor)
			{
				if (board[y][x].type == Rook || board[y][x].type == Queen)
					ret = true;
				break;
			}
		}

		x = position.x;
		y = position.y;
		while (validSelect(++y, x) && (board[y][x].type == Empty || board[y][x].pieceColor != pieceColor)) {
			board[y][x].valid = true;
			if (mark)
				board[y][x].display();
			if (board[y][x].type != Empty && board[y][x].pieceColor != pieceColor)
			{
				if (board[y][x].type == Rook || board[y][x].type == Queen)
					ret = true;
				break;
			}
		}

		x = position.x;
		y = position.y;
		while (validSelect(y, --x) && (board[y][x].type == Empty || board[y][x].pieceColor != pieceColor)) {
			board[y][x].valid = true;
			if (mark)
				board[y][x].display();
			if (board[y][x].type != Empty && board[y][x].pieceColor != pieceColor)
			{
				if (board[y][x].type == Rook || board[y][x].type == Queen)
					ret = true;
				break;
			}
		}

		x = position.x;
		y = position.y;
		while (validSelect(y, ++x) && (board[y][x].type == Empty || board[y][x].pieceColor != pieceColor)) {
			board[y][x].valid = true;
			if (mark)
				board[y][x].display();
			if (board[y][x].type != Empty && board[y][x].pieceColor != pieceColor)
			{
				if (board[y][x].type == Rook || board[y][x].type == Queen)
					ret = true;
				break;
			}
		}
		return ret;
	}

	bool markDiagonals(piece**& board, bool mark = true) {

		bool ret = false;

		int x = position.x;
		int y = position.y;
		while (validSelect(--y, --x) && (board[y][x].type == Empty || board[y][x].pieceColor != pieceColor)) {
			board[y][x].valid = true;
			if (mark)
				board[y][x].display();
			if (board[y][x].type != Empty && board[y][x].pieceColor != pieceColor)
			{
				if (board[y][x].type == Bishop || board[y][x].type == Queen)
					ret = true;
				break;
			}
		}

		x = position.x;
		y = position.y;
		while (validSelect(++y, --x) && (board[y][x].type == Empty || board[y][x].pieceColor != pieceColor)) {
			board[y][x].valid = true;
			if (mark)
				board[y][x].display();
			if (board[y][x].type != Empty && board[y][x].pieceColor != pieceColor)
			{
				if (board[y][x].type == Bishop || board[y][x].type == Queen)
					ret = true;
				break;
			}
		}

		x = position.x;
		y = position.y;
		while (validSelect(--y, ++x) && (board[y][x].type == Empty || board[y][x].pieceColor != pieceColor)) {
			board[y][x].valid = true;
			if (mark)
				board[y][x].display();
			if (board[y][x].type != Empty && board[y][x].pieceColor != pieceColor)
			{
				if (board[y][x].type == Bishop || board[y][x].type == Queen)
					ret = true;
				break;
			}
		}

		x = position.x;
		y = position.y;
		while (validSelect(++y, ++x) && (board[y][x].type == Empty || board[y][x].pieceColor != pieceColor)) {
			board[y][x].valid = true;
			if (mark)
				board[y][x].display();
			if (board[y][x].type != Empty && board[y][x].pieceColor != pieceColor)
			{
				if (board[y][x].type == Bishop || board[y][x].type == Queen)
					ret = true;
				break;
			}
		}
		return ret;
	}

	bool markKnight(piece**& board, bool mark = true) {

		bool ret = false;

		int x, y;
		for (int i = -2; i <= 2; i++) {
			x = position.x;
			y = position.y;
			if (i == 0)
				continue;
			if (validSelect(y, x + i)) {
				x += i;
				for (int n = -1; n <= 1; n += 2) {
					y = position.y;
					if (validSelect(y + n * (3 - abs(i)), x)) {
						y += n * (3 - abs(i));
						if (board[y][x].type == Empty || board[y][x].pieceColor != pieceColor) {
							board[y][x].valid = true;
							if (mark)
								board[y][x].display();
							if (board[y][x].type == Knight)
								ret = true;
						}
					}
				}
			}
		}
		return ret;
	}

	bool markPawn(piece**& board, bool mark = true) {
		bool ret = false;

		int dy;
		int x, y;
		int turns = 1;
		if (pieceColor == White) {
			dy = -1;
			if (position.y == 6)
				turns = 2;
		}
		else {
			dy = 1;
			if (position.y == 1)
				turns = 2;
		}

		x = position.x;
		y = position.y;
		for (int checkY = 1; checkY <= turns; checkY++) {
			if (!validSelect(y + dy, x) || board[y += dy][x].type != Empty)
				break;
			board[y][x].valid = true;
			if (mark) {
				if ((pieceColor == Black && y == 7) || (pieceColor == White && y == 0))
					board[y][x].printBox(White);
				else
					board[y][x].display();
			}
		}

		for (int n = -1; n <= 1; n += 2) {
			y = position.y;
			x = position.x;
			if (!validSelect(y + dy, x + n) || board[y += dy][x += n].type == Empty || board[y][x].pieceColor == pieceColor)
				continue;
			board[y][x].valid = true;
			if (board[y][x].type == Pawn)
				ret = true;
			if (mark) {
				if ((pieceColor == Black && y == 7) || (pieceColor == White && y == 0))
					board[y][x].printBox(White);
				else
					board[y][x].display();
			}
		}

		return ret;
	}

	bool markKing(piece**& board, bool mark = true) {
		bool ret = false;
		int x, y;
		for (int dx = -1; dx <= 1; dx++) {
			for (int dy = -1; dy <= 1; dy++) {
				if (dx == 0 && dy == 0)
					continue;
				x = position.x;
				y = position.y;
				if (!validSelect(y + dy, x + dx) || (board[y += dy][x += dx].type != Empty && board[y][x].pieceColor == pieceColor))
					continue;
				board[y][x].valid = true;
				if (board[y][x].type == King)
					ret = true;
				if (mark)
					board[y][x].display();
			}
		}
		return ret;
	}

	void markValids(piece**& board, bool mark = true) {
		if (mark) {
			printBox(Yellow);
		}

		switch (type)
		{
		case Rook:
			markHorizontalVertical(board, mark);
			break;
		case Knight:
			markKnight(board, mark);
			break;
		case Bishop:
			markDiagonals(board, mark);
			break;
		case Queen:
			markDiagonals(board, mark);
			markHorizontalVertical(board, mark);
			break;
		case King:
			markKing(board, mark);
			int x, y;
			if (castling) {
				for (int dx = -1; dx <= 1; dx += 2) {
					x = position.x;
					y = position.y;
					while (validSelect(y, x += dx) && board[y][x].type == Empty);
					if ((x == 0 || x == 7) && board[y][x].type == Rook && board[y][x].pieceColor == pieceColor) {
						board[position.y][position.x + (dx * 2)].valid = true;
						board[position.y][position.x + (dx * 2)].printBox(White);
					}
				}
			}
			break;
		case Pawn:
			markPawn(board, mark);
			break;
		}
	}

	void removeValidMarks(piece**& board, bool mark = true) {
		if (mark)
			board[position.y][position.x].display();
		for (int ri = 0; ri < 8; ri++) {
			for (int ci = 0; ci < 8; ci++) {
				if (board[ci][ri].valid) {
					board[ci][ri].valid = false;
					if (mark)
						board[ci][ri].display();
				}
			}
		}
	}


	//133 lines lol
	void display() {
		int x = position.x;
		int y = position.y;
		SetConsoleTextAttribute(hConsole, backColor);
		for (int ri = 0; ri < Dimension; ri++) {
			for (int ci = 0; ci < Dimension; ci++) {
				gotoRowCol(y * Dimension + 3 + ri, x * Dimension + 3 + ci);
				cout << block;
			}
		}
		if (valid)
			printBox(Green);

		if (type != Empty) {
			SetConsoleTextAttribute(hConsole, pieceColor);
			switch (type)
			{
			case Rook:
				gotoRowCol(y * Dimension + 3 + 1, x * Dimension + 3 + 1);
				cout << block << block;
				gotoRowCol(y * Dimension + 3 + 1, x * Dimension + 3 + 4);
				cout << block << block;
				gotoRowCol(y * Dimension + 3 + 1, x * Dimension + 3 + 7);
				cout << block << block;
				gotoRowCol(y * Dimension + 3 + 2, x * Dimension + 3 + 1);
				cout << block << block << block << block << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 3, x * Dimension + 3 + 1);
				cout << block << block << block << block << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 4, x * Dimension + 3 + 3);
				cout << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 5, x * Dimension + 3 + 4);
				cout << block << block;
				gotoRowCol(y * Dimension + 3 + 6, x * Dimension + 3 + 4);
				cout << block << block;
				gotoRowCol(y * Dimension + 3 + 7, x * Dimension + 3 + 3);
				cout << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 8, x * Dimension + 3 + 2);
				cout << block << block << block << block << block << block;
				break;
			case Knight:
				gotoRowCol(y * Dimension + 3 + 1, x * Dimension + 3 + 2);
				cout << block;
				gotoRowCol(y * Dimension + 3 + 2, x * Dimension + 3 + 2);
				cout << block << block << block;
				gotoRowCol(y * Dimension + 3 + 3, x * Dimension + 3 + 1);
				cout << block << block << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 4, x * Dimension + 3 + 1);
				cout << block << block << block << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 5, x * Dimension + 3 + 1);
				cout << block << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 6, x * Dimension + 3 + 1);
				cout << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 7, x * Dimension + 3 + 2);
				cout << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 8, x * Dimension + 3 + 1);
				cout << block << block << block << block << block << block;
				break;
			case Bishop:
				gotoRowCol(y * Dimension + 3 + 1, x * Dimension + 3 + 4);
				cout << block << block;
				gotoRowCol(y * Dimension + 3 + 2, x * Dimension + 3 + 3);
				cout << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 3, x * Dimension + 3 + 2);
				cout << block << block << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 4, x * Dimension + 3 + 2);
				cout << block << block << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 5, x * Dimension + 3 + 3);
				cout << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 6, x * Dimension + 3 + 4);
				cout << block << block;
				gotoRowCol(y * Dimension + 3 + 7, x * Dimension + 3 + 4);
				cout << block << block;
				gotoRowCol(y * Dimension + 3 + 8, x * Dimension + 3 + 3);
				cout << block << block << block << block;
				break;
			case Queen:
				gotoRowCol(y * Dimension + 3 + 1, x * Dimension + 3 + 4);
				cout << block << block;
				gotoRowCol(y * Dimension + 3 + 2, x * Dimension + 3 + 2);
				cout << block << block << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 3, x * Dimension + 3 + 2);
				cout << block << block << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 4, x * Dimension + 3 + 4);
				cout << block << block;
				gotoRowCol(y * Dimension + 3 + 5, x * Dimension + 3 + 4);
				cout << block << block;
				gotoRowCol(y * Dimension + 3 + 6, x * Dimension + 3 + 4);
				cout << block << block;
				gotoRowCol(y * Dimension + 3 + 7, x * Dimension + 3 + 3);
				cout << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 8, x * Dimension + 3 + 2);
				cout << block << block << block << block << block << block;
				break;
			case King:
				gotoRowCol(y * Dimension + 3 + 1, x * Dimension + 3 + 4);
				cout << block << block;
				gotoRowCol(y * Dimension + 3 + 2, x * Dimension + 3 + 3);
				cout << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 3, x * Dimension + 3 + 2);
				cout << block << block;
				gotoRowCol(y * Dimension + 3 + 3, x * Dimension + 3 + 6);
				cout << block << block;
				gotoRowCol(y * Dimension + 3 + 4, x * Dimension + 3 + 2);
				cout << block << block;
				gotoRowCol(y * Dimension + 3 + 4, x * Dimension + 3 + 6);
				cout << block << block;
				gotoRowCol(y * Dimension + 3 + 5, x * Dimension + 3 + 3);
				cout << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 6, x * Dimension + 3 + 4);
				cout << block << block;
				gotoRowCol(y * Dimension + 3 + 7, x * Dimension + 3 + 3);
				cout << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 8, x * Dimension + 3 + 2);
				cout << block << block << block << block << block << block;
				break;
			case Pawn:
				gotoRowCol(y * Dimension + 3 + 2, x * Dimension + 3 + 3);
				cout << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 3, x * Dimension + 3 + 3);
				cout << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 4, x * Dimension + 3 + 3);
				cout << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 5, x * Dimension + 3 + 3);
				cout << block << block << block << block;
				gotoRowCol(y * Dimension + 3 + 6, x * Dimension + 3 + 4);
				cout << block << block;
				gotoRowCol(y * Dimension + 3 + 7, x * Dimension + 3 + 4);
				cout << block << block;
				gotoRowCol(y * Dimension + 3 + 8, x * Dimension + 3 + 3);
				cout << block << block << block << block;
				break;
			}
		}
	}

};

void Init(piece**& board, bool load, color& Turn) {
	SetConsoleTextAttribute(hConsole, White);
	char c = 'A';
	for (int ci = 0; ci < 8; ci++) {
		gotoRowCol(1, ci * Dimension + 3 + 4);
		cout << c;
		gotoRowCol(ci * Dimension + 3 + 4, 1);
		cout << 8 - ci;
		gotoRowCol(8 * Dimension + 3 + 2, ci * Dimension + 3 + 4);
		cout << c++;
		gotoRowCol(ci * Dimension + 3 + 4, 8 * Dimension + 3 + 2);
		cout << 8 - ci;
	}


	SetConsoleTextAttribute(hConsole, 7);
	string path;
	if (load)
		path = "Load.txt";
	else
		path = "New.txt";

	ifstream reader(path);

	color C = Grey;
	board = new piece * [8];
	for (int ri = 0; ri < 8; ri++) {
		board[ri] = new piece[8];
		for (int ci = 0; ci < 8; ci++) {
			if (ri % 2 == ci % 2)
				board[ri][ci].backColor = board[ri][ci].pieceColor = Grey;
			else
				board[ri][ci].backColor = board[ri][ci].pieceColor = Silver;
			board[ri][ci].position.x = ci;
			board[ri][ci].position.y = ri;
			board[ri][ci].type = Empty;
			board[ri][ci].valid = false;
		}
	}

	char in;
	pieces P;
	for (int ri = 0; ri < 8; ri++) {
		for (int ci = 0; ci < 8; ci++) {
			reader >> in;
			switch (in) {
			case 'R':
			case 'r':
				P = Rook; break;
			case 'N':
			case 'n':
				P = Knight; break;
			case 'B':
			case 'b':
				P = Bishop; break;
			case 'Q':
			case 'q':
				P = Queen; break;
			case 'K':
			case 'k':
				P = King;
				if (ci == 4) {
					if (in == 'K' && ri == 0)
						board[ri][ci].castling = true;
					else if (in == 'k' && ri == 7)
						board[ri][ci].castling = true;
				}
				break;
			case 'P':
			case 'p':
				P = Pawn; break;
			}
			if (in >= 'A' && in <= 'Z') {
				C = Black;
				board[ri][ci].type = P;
				board[ri][ci].pieceColor = C;
			}
			else if (in >= 'a' && in <= 'z') {
				C = White;
				board[ri][ci].type = P;
				board[ri][ci].pieceColor = C;
			}
			board[ri][ci].display();
		}
	}

	if (load) {
		reader >> in;
		if (in == 'B')
			Turn = Black;
		else
			Turn = White;
	}
	else {
		if (rand() % 2 == 0)
			Turn = White;
		else
			Turn = Black;
	}
}

void getTurnClick(piece**& board, int& ri, int& ci, bool& exit, bool& save, bool& undo) {
	do {
		getRowColbyLeftClick(ri, ci);
		if (ri > Dimension * 8 && ri <= Dimension * 8 + 5) {
			if (ci >= 8 * Dimension + 10 && ci <= 8 * Dimension + 10 + 10) {
				exit = true;
				return;
			}
			if (ci >= 8 * Dimension + 10 + 15 && ci <= 8 * Dimension + 10 + 25) {
				save = true;
				return;
			}
			if (ci >= 8 * Dimension + 10 + 30 && ci <= 8 * Dimension + 10 + 10 + 40) {
				undo = true;
				return;
			}


		}





		ri -= 3;
		ri /= Dimension;
		ci -= 3;
		ci /= Dimension;
	} while (ri >= 8 || ci >= 8);
}

piece findKing(piece**& board, color myColor) {
	for (int ri = 0; ri < 8; ri++)
		for (int ci = 0; ci < 8; ci++)
			if (board[ri][ci].type == King && board[ri][ci].pieceColor == myColor)
				return board[ri][ci];
}

bool check(piece**& board, color myColor) {
	piece** tempBoard = new piece * [8];
	for (int ri = 0; ri < 8; ri++) {
		tempBoard[ri] = new piece[8];
		for (int ci = 0; ci < 8; ci++) {
			tempBoard[ri][ci] = board[ri][ci];
		}
	}

	piece myKing = findKing(tempBoard, myColor);

	bool ret = myKing.markDiagonals(tempBoard, false) || myKing.markHorizontalVertical(tempBoard, false) || myKing.markKing(tempBoard, false)
		|| myKing.markKnight(tempBoard, false) || myKing.markPawn(tempBoard, false);

	for (int ri = 0; ri < 8; ri++)
		delete[] tempBoard[ri];
	delete[] tempBoard;

	return ret;
}

struct chessMove {
	color myColor, enemyColor;
	int selectR, selectC, moveR, moveC;
	pieces myPiece, enemyPiece;
	bool promotion = false;
	Castle castling = No;
};

bool makeMove(piece**& board, int selectR, int selectC, int moveR, int moveC, pieces& tempPiece, bool enemyCheck, bool mark = true) {
	color tempColor = board[moveR][moveC].pieceColor;
	tempPiece = board[moveR][moveC].type;
	board[moveR][moveC].type = board[selectR][selectC].type;
	board[moveR][moveC].pieceColor = board[selectR][selectC].pieceColor;
	board[moveR][moveC].castling = board[selectR][selectC].castling;
	board[selectR][selectC].type = Empty;


	if (check(board, board[moveR][moveC].pieceColor) || (board[moveR][moveC].castling && (moveC == 2 || moveC == 6) && enemyCheck)) {
		board[selectR][selectC].castling = board[moveR][moveC].castling;
		board[selectR][selectC].type = board[moveR][moveC].type;
		board[moveR][moveC].type = tempPiece;
		board[moveR][moveC].pieceColor = tempColor;
		return false;
	}


	if (board[moveR][moveC].castling && (moveC == 2 || moveC == 6)) {
		board[moveR][4 - ((4 - moveC) / 2)].type = King;
		board[moveR][4 - ((4 - moveC) / 2)].pieceColor = board[moveR][moveC].pieceColor;
		board[moveR][moveC].type = Empty;
		if (check(board, board[moveR][moveC].pieceColor)) {
			board[moveR][4 - ((4 - moveC) / 2)].type = Empty;
			board[moveR][moveC].type = King;
			board[selectR][selectC].castling = board[moveR][moveC].castling;
			board[selectR][selectC].type = board[moveR][moveC].type;
			board[moveR][moveC].type = tempPiece;
			board[moveR][moveC].pieceColor = tempColor;
			return false;
		}
		board[moveR][4 - ((4 - moveC) / 2)].type = Empty;
		board[moveR][moveC].type = King;

	}

	if (board[moveR][moveC].castling) {
		if (moveC == 2)
			makeMove(board, moveR, 0, moveR, 3, tempPiece, enemyCheck);
		if (moveC == 6)
			makeMove(board, moveR, 7, moveR, 5, tempPiece, enemyCheck);
	}

	if (mark) {
		board[moveR][moveC].display();
		board[selectR][selectC].display();
	}


	return true;
}

bool possibleMove(piece**& board, color checkColor) {
	for (int selectR = 0; selectR < 8; selectR++) {
		for (int selectC = 0; selectC < 8; selectC++) {
			if (board[selectR][selectC].type != Empty && board[selectR][selectC].pieceColor == checkColor) {
				board[selectR][selectC].markValids(board, false);
				for (int moveR = 0; moveR < 8; moveR++) {
					for (int moveC = 0; moveC < 8; moveC++) {
						if (board[moveR][moveC].valid) {
							color tempColor = board[moveR][moveC].pieceColor;
							pieces tempPiece;
							if (!(board[selectR][selectC].castling && (moveC == 2 || moveC == 6)) &&
								makeMove(board, selectR, selectC, moveR, moveC, tempPiece, false, false)) {
								board[selectR][selectC].type = board[moveR][moveC].type;
								board[moveR][moveC].type = tempPiece;
								board[moveR][moveC].pieceColor = tempColor;
								return true;
							}
						}
					}
				}
				board[selectR][selectC].removeValidMarks(board, false);
			}
		}
	}
	board[0][0].removeValidMarks(board);
	return false;
}

void Save(piece**& board, color Turn) {
	char out;
	ofstream writer("Load.txt");

	for (int ri = 0; ri < 8; ri++) {
		for (int ci = 0; ci < 8; ci++) {
			switch (board[ri][ci].type) {
			case Rook:
				out = 'R'; break;
			case Bishop:
				out = 'B'; break;
			case Knight:
				out = 'N'; break;
			case Queen:
				out = 'Q'; break;
			case King:
				out = 'K'; break;
			case Pawn:
				out = 'P'; break;
			default:
				out = '-';
			}
			if (board[ri][ci].type != Empty && board[ri][ci].pieceColor == White)
				out += 32;

			writer << out;
		}
		writer << '\n';
	}
	if (Turn == White)
		out = 'W';
	else
		out = 'B';
	writer << out;
}

bool printTimes(int Wtime, int Btime, int gameTime) {
	SetConsoleTextAttribute(hConsole, 7);
	int minutes = 0, sec = 0;
	bool ret = true;
	string msg;
	gotoRowCol(7 * Dimension + 6, 8 * Dimension + 10);
	minutes = Wtime / 60;
	if (minutes >= gameTime)
		ret = false;
	sec = Wtime % 60;
	cout << setw(2) << minutes << " : " << setw(2) << sec;
	cout << "\t\t";
	minutes = Btime / 60;
	if (minutes >= gameTime)
		ret = false;
	sec = Btime % 60;
	cout << setw(2) << minutes << " : " << setw(2) << sec;
	return ret;
}

void printMove(chessMove*& moves, int moveN) {
	for (int i = 1; i <= 5; i++) {
		string msg;
		string c;
		if (moveN - i < 0) {
			printMsg("                                         ", (6 - i) * 2 + 9);
			continue;
		}
		msg = to_string(moveN - i + 1) + ") ";
		if (moves[moveN - i].myColor == White) {
			msg += "White";
		}
		else {
			msg += "Black";
		}
		if (moves[moveN - i].castling != No) {
			msg += "King Castles";
			if (moves[moveN - i].castling == Right)
				msg += " King Side";
			else
				msg += " Queen side";
			printMsg(msg, (6 - i) * 2 + 9);
			continue;
		}

		switch (moves[moveN - i].myPiece) {
		case Rook:
			msg += " Rook"; break;
		case Bishop:
			msg += " Bishop"; break;
		case Knight:
			msg += " Knight"; break;
		case Queen:
			msg += " Queen"; break;
		case King:
			msg += " King"; break;
		case Pawn:
			msg += " Pawn"; break;
		}
		c = 'A' + moves[moveN - i].selectC;
		msg += " from " + c;
		c = '8' - moves[moveN - i].selectR;
		msg += c;
		c = 'A' + moves[moveN - i].moveC;
		msg += " to " + c;
		c = '8' - moves[moveN - i].moveR;
		msg += c;

		if (moves[moveN - i].enemyPiece != Empty) {
			msg += " captures";
			switch (moves[moveN - i].enemyPiece) {
			case Rook:
				msg += " Rook"; break;
			case Bishop:
				msg += " Bishop"; break;
			case Knight:
				msg += " Knight"; break;
			case Queen:
				msg += " Queen"; break;
			case King:
				msg += " King"; break;
			case Pawn:
				msg += " Pawn"; break;
			}
		}

		if (moves[moveN - i].promotion)
			msg += " promotes";

		SetConsoleTextAttribute(hConsole, 7);
		printMsg(msg, (6 - i) * 2 + 9);
	}
}

void Undo(piece**& board, chessMove*& moves, int& moveN, color& Turn) {
	if (moveN == 0)
		return;

	board[moves[moveN - 1].selectR][moves[moveN - 1].selectC].type = moves[moveN - 1].myPiece;
	board[moves[moveN - 1].selectR][moves[moveN - 1].selectC].pieceColor = moves[moveN - 1].myColor;

	board[moves[moveN - 1].moveR][moves[moveN - 1].moveC].type = moves[moveN - 1].enemyPiece;
	board[moves[moveN - 1].moveR][moves[moveN - 1].moveC].pieceColor = moves[moveN - 1].enemyColor;

	board[moves[moveN - 1].moveR][moves[moveN - 1].moveC].display();
	board[moves[moveN - 1].selectR][moves[moveN - 1].selectC].display();

	if (moves[moveN - 1].castling != No) {
		pieces temp = Empty;
		if (moves[moveN - 1].castling == Right) {
			makeMove(board, moves[moveN - 1].moveR, 5, moves[moveN - 1].moveR, 7, temp, false);
		}
		else {
			makeMove(board, moves[moveN - 1].moveR, 3, moves[moveN - 1].moveR, 0, temp, false);
		}
	}


	moveN--;
	printMove(moves, moveN);

	if (Turn == White)
		Turn = Black;
	else
		Turn = White;

	board[0][0].removeValidMarks(board);

}

void addMove(int& moveN, chessMove*& moves, int selectC, int selectR, int moveR, int moveC, piece**& board, bool& promotion, pieces tempPiece) {
	moveN++;
	chessMove* tempMoves = new chessMove[moveN];
	for (int i = 0; i < moveN - 1; i++)
		tempMoves[i] = moves[i];

	delete[] moves;

	tempMoves[moveN - 1].selectR = selectR;
	tempMoves[moveN - 1].selectC = selectC;
	tempMoves[moveN - 1].moveR = moveR;
	tempMoves[moveN - 1].moveC = moveC;
	tempMoves[moveN - 1].myColor = board[moveR][moveC].pieceColor;
	tempMoves[moveN - 1].myPiece = board[moveR][moveC].type;
	tempMoves[moveN - 1].enemyPiece = tempPiece;
	tempMoves[moveN - 1].promotion = false;

	if (board[moveR][moveC].pieceColor == White)
		tempMoves[moveN - 1].enemyColor = Black;
	else
		tempMoves[moveN - 1].enemyColor = White;

	if (promotion) {
		board[moveR][moveC].promote(board);
		tempMoves[moveN - 1].promotion = true;
		promotion = false;
	}

	if (board[moveR][moveC].castling) {
		if (moveC == 2)
			tempMoves[moveN - 1].castling = Left;
		if (moveC == 6)
			tempMoves[moveN - 1].castling = Right;

		board[moveR][moveC].castling = false;
	}

	moves = tempMoves;

}

int main() {
	time_t start, end;
	srand(time(0));
	piece** board = nullptr;
	int selectR, selectC, moveR, moveC;
	gotoRowCol(Rows + 2, Cols);
	bool valid, same, restart;
	color Turn;
	bool selfCheck = false, enemyCheck = false;
	piece checkKing;
	pieces tempPiece;
	int moveN = 0;
	int saveMoves = 15;
	bool exit, save, undo, promotion;
	chessMove* moves = nullptr;
	int gameTime = 300;
	bool timeRemains;

	int Wtime = 0, Btime = 0;
	bool BlackTime, CH = false;


	char option;
	do {
		timeRemains = true;
		exit = false;
		system("CLS");
		do {
			printMsg("Welcome to Chess Version 1.0", 5, 5);
			printMsg("Made by Ahsan Abdur Rehman", 7, 5);
			printMsg("Enter Full Screen To Play", 10, 5);
			printMsg("How Would You Like To Start the Game? ", 14, 5);
			printMsg("(L)oad Game or (N)ew Game: ", 16, 5);
			option = _getch();
		} while (!(option == 'l' || option == 'L' || option == 'N' || option == 'n'));

		system("CLS");

		if (option == 'l' || option == 'L')
			Init(board, true, Turn);
		else {
			Init(board, false, Turn);
			if (rand() % 2 == 0)
				Turn = White;
			else
				Turn = Black;
		}
		selfCheck = enemyCheck = restart = false;
		moveN = 0;
		SetConsoleTextAttribute(hConsole, White);
		printMsg("Moves: ", 9);

		printMsg("**********     **********     **********", Dimension * 8 + 1);
		printMsg("*        *     *        *     *        *", Dimension * 8 + 2);
		printMsg("*  Exit  *     *  Save  *     *  Undo  *", Dimension * 8 + 3);
		printMsg("*        *     *        *     *        *", Dimension * 8 + 4);
		printMsg("**********     **********     **********", Dimension * 8 + 5);

		string msg;
		msg = "You Will get each get " + to_string(gameTime) + " minutes each";
		printMsg(msg, Dimension * 7);
		msg = "White\t\tBlack";
		printMsg(msg, Dimension * 7 + 4);
		enemyCheck = false;
		CH = false;
		time(&start);
		time(&end);
		timeRemains = false;
		if (Turn == White)
			BlackTime = false;
		else
			BlackTime = true;

		while (true) {

			time(&end);
			if (BlackTime)
				Btime += difftime(end, start);
			else
				Wtime += difftime(end, start);
			if (!(timeRemains = printTimes(Wtime, Btime, gameTime)))
				break;
			if (CH)
				BlackTime = !BlackTime;
			CH = false;
			time(&start);

			promotion = false;
			save = undo = exit = false;
			SetConsoleTextAttribute(hConsole, 7);
			if (!selfCheck) {
				if (!possibleMove(board, Turn)) {
					string winner;
					SetConsoleTextAttribute(hConsole, Red);
					if (Turn == White)
						winner = "Black";
					else
						winner = "White";

					if (enemyCheck) {
						printMsg("CHECK MATE!!  " + winner + " wins!!", 5);
						checkKing = findKing(board, Turn);
						checkKing.printBox(Red);
					}
					else {
						printMsg("STALE MATE!! It's a draw.", 5);
						checkKing = findKing(board, Black);
						checkKing.printBox(Red);
						checkKing = findKing(board, White);
						checkKing.printBox(Red);
					}
					break;
				}
			}

			if (Turn == White)
				printMsg("White's Turn");
			else
				printMsg("Black's Turn");
			same = false;

			do {
				save = false, undo = false;
				getTurnClick(board, selectR, selectC, exit, save, undo);
				if (save) {
					printMsg("Game Saved!!", 8 * Dimension - 2);
					Save(board, Turn);
				}if (undo) {
					printMsg("Undo !!", 8 * Dimension - 2);
					Undo(board, moves, moveN, Turn);
					time(&end);
					if (BlackTime)
						Btime += difftime(end, start);
					else
						Wtime += difftime(end, start);
					BlackTime = !BlackTime;
					if (!(timeRemains = printTimes(Wtime, Btime, gameTime)))
						break;
					time(&start);
					SetConsoleTextAttribute(hConsole, 7);
					board[0][0].removeValidMarks(board, false);
					printMsg("                     ", 5);
					if (Turn == White)
						printMsg("White's Turn");
					else
						printMsg("Black's Turn");
					same = false;
					continue;
				}
				if (exit)
					break;
			} while (save || undo || !(board[selectR][selectC].type != Empty && board[selectR][selectC].pieceColor == Turn));

			board[0][0].removeValidMarks(board, false);


			if (exit || !timeRemains)
				break;

			if ((board[selectR][selectC].type == Pawn) && ((board[selectR][selectC].pieceColor == White && selectR == 1)
				|| (board[selectR][selectC].pieceColor == Black && selectR == 6)))
				promotion = true;

			printMsg("                               ", 8 * Dimension - 2);

			board[selectR][selectC].markValids(board);

			do {
				exit = save = undo = false;
				getTurnClick(board, moveR, moveC, exit, save, undo);
				same = (moveR == selectR && moveC == selectC);
				if (exit)
					break;
			} while (save || undo || (!(board[moveR][moveC].valid || same)));

			if (exit)
				break;


			board[selectR][selectC].removeValidMarks(board);

			selfCheck = false;

			if (!same) {
				SetConsoleTextAttribute(hConsole, 7);
				if (!(selfCheck = !makeMove(board, selectR, selectC, moveR, moveC, tempPiece, enemyCheck))) {

					enemyCheck = false;
					addMove(moveN, moves, selectC, selectR, moveR, moveC, board, promotion, tempPiece);
					printMove(moves, moveN);

					if (Turn == Black)
						Turn = White;
					else
						Turn = Black;

					CH = true;

					SetConsoleTextAttribute(hConsole, Red);
					if (enemyCheck = check(board, Turn))
						printMsg("CHECK!!!\a", 5);
					else
						printMsg("                  ", 5);
				}
				else {
					SetConsoleTextAttribute(hConsole, Red);
					printMsg("SELF-CHECK!!\a", 5);
				}
				SetConsoleTextAttribute(hConsole, 7);
				if (selfCheck || enemyCheck) {
					checkKing = findKing(board, Turn);
					checkKing.printBox(Red);
					Sleep(1000);
					checkKing.display();
				}
			}
		}
		SetConsoleTextAttribute(hConsole, 7);
		if (!timeRemains) {
			string msg = "Time Ended!!";
			if (BlackTime)
				msg += " White";
			else
				msg += " Black";
			msg += " wins!!";
			printMsg(msg, Dimension * 7);
		}
		printMsg("Game ended. Do you want to (R)estart??  ");
		char opt = _getch();
		if (opt == 'R' || opt == 'r')
			restart = true;
		if (!restart)
			break;
	} while (true);
	return 0;
}