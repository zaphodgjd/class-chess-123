#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <thread>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <atomic>
#include <chrono>
#include <ctime>
#include <future>

#ifdef _MSC_VER
#include <intrin.h>
#endif

#include "Player.h"
#include "Turn.h"
#include "Bit.h"
#include "BitHolder.h"

class GameTable;

struct GameOptions
{
	bool AIPlaying;
	int numberOfPlayers;
	int AIPlayer;
	int rowX;
	int rowY;
	int gameNumber;
	unsigned int currentTurnNo;
	int score;
	int AIDepthSearches;
	int AIMAXDepth;
	bool AIvsAI;
};

class Game
{
public:
	Game();
	~Game();

	void startGame();

	virtual void setUpBoard() = 0;

	virtual void drawFrame();

	// end the current game turn
	virtual void endTurn();

	// Should return true if it is legal for the given bit to be moved from its current holder.
	// Default implementation always returns true.
	virtual bool canBitMoveFrom(Bit &bit, BitHolder &src) = 0;

	// /** Should return true if it is legal for the given Bit to move from src to dst.
	// Default implementation always returns true.
	virtual bool canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) = 0;

	// can we do something with an empty holder?  do it here
	virtual bool actionForEmptyHolder(BitHolder &holder);

	// Should handle any side effects of a Bit's movement, such as captures or scoring.
	// Does not need to do the actual movement! That's already happened.
	// It should end by calling endTurn, if the player's turn is over.
	// Default implementation just calls endTurn.
	virtual void bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst);

	// Called instead of the above if a Bit is simply clicked, not dragged.
	// Should return NO if the click is illegal (i.e. clicking an empty draw pile in a card game.)
	// Default implementation always returns true.
	virtual bool clickedBit(Bit &bit);

	// clear any board highlights we've setup for legal moves
	virtual void clearBoardHighlights();

	// Called on mouse-down/touch of an *empty* BitHolder. Should return a Bit if
	// it's OK to place a new Bit there; else nil.
	virtual Bit *bitToPlaceInHolder(BitHolder &holder);

	virtual Player *checkForWinner() = 0;
	virtual bool checkForDraw() = 0;
	virtual bool animateAndPlaceBitFromTo(Bit &bit, BitHolder &src, BitHolder &dst);

	virtual void stopGame() = 0;
	virtual bool gameHasAI();
	virtual void updateAI();
	virtual void pieceTaken(Bit *bit){};

	virtual std::string initialStateString() = 0;
	virtual std::string stateString() = 0;
	virtual void setStateString(const std::string &s) = 0;

	void setNumberOfPlayers(unsigned int playerCount);
	void setAIPlayer(unsigned int playerNumber);
	virtual int getAIDepathSearches() { return _gameOptions.AIDepthSearches; };
	virtual int getAIMAXDepth() { return _gameOptions.AIMAXDepth; };

	// mouse functions
	void scanForMouse();
	// function to return pointer to the [][] array of bitholders
	virtual BitHolder &getHolderAt(const int x, const int y) = 0;

	const unsigned int getCurrentTurnNo() { return _gameOptions.currentTurnNo; };
	const int getScore() { return _gameOptions.score; };
	void setScore(int score) { _gameOptions.score = score; };
	// this code below limits class to two players at the most, but it ensures player 0 is white and player 1 is black
	Player *getCurrentPlayer() { return (_players.size() != 0) ? _players.at(_gameOptions.currentTurnNo & 1) : nullptr; };
	Player *getPlayerAt(unsigned int playerNumber) { return (_players.size() != 0) ? _players.at(playerNumber) : nullptr;};
	const int getAIPlayer() { return _gameOptions.AIPlayer; };
	const int getHumanPlayer()
	{
		if (_gameOptions.AIPlayer == 1)
			return 0;
		else
			return 1;
	};
	GameTable *_table;
	Player *_winner;

	std::vector<Player *> _players;
	std::vector<Turn *> _turns;

	std::string _lastMove;

	GameOptions _gameOptions;

protected:
	void mouseDown(ImVec2 &location, Entity *bit);
	void mouseMoved(ImVec2 &location, Entity *bit);
	void mouseUp(ImVec2 &location, Entity *bit);
	void findDropTarget(ImVec2 &pos);

	ImVec2 _dragStartPos;
	ImVec2 _dragOffset;
	ImVec2 _oldPos;
	Bit *_dragBit;
	BitHolder *_dragHolder;
	BitHolder *_dropTarget;
	BitHolder *_oldHolder;
	bool _dragMoved;
};
