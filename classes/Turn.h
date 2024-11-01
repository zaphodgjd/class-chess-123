#pragma once
#include <iostream>

class Game;
class Player;

typedef enum {
	kTurnEmpty,             // No action yet
	kTurnPartial,           // Action taken, but more needs to be done
	kTurnComplete,          // Action complete, but player needs to confirm
	kTurnFinished           // Turn is confirmed and finished
} TurnStatus;

class Turn
{
public:
	Turn() : _game(nullptr), _player(nullptr), _status(kTurnEmpty), _move(""), _boardState(""), _date(0), _comment(""), _score(0), _replaying(false), _gameNumber(-1) {};
	~Turn() {};

	static	Turn *initStartOfGame(Game *game) { Turn *turn = new Turn(); turn->_game = game; turn->_status = kTurnFinished; return turn; };
	void	setStateString(std::string board) { _boardState = board; };
	Game		*_game;
	Player		*_player;
	TurnStatus	_status;
	std::string	_move;
	std::string	_boardState;
	int			_date;
	std::string	_comment;
	int			_score;
	bool		_replaying;
	int			_gameNumber;
};

