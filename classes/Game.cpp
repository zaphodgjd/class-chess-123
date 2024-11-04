#include "Game.h"
#include "Bit.h"
#include "BitHolder.h"
#include "Turn.h"
#include "../Application.h"
#include <cmath>

Game::Game() {
	_gameOps.AIPlayer = false;
	_gameOps.AIPlaying = false;
	_gameOps.currentTurnNo = 0;
	_gameOps.gameNumber = -1;
	_gameOps.numberOfPlayers = 0;
	_gameOps.X = 8;
	_gameOps.Y = 8;
	_gameOps.size = _gameOps.X * _gameOps.Y;
	_gameOps.score = 0;
	_gameOps.AIDepthSearches = 0;
	_gameOps.AIvsAI = false;

	_table = nullptr;
	_winner = nullptr;
	_lastMove = "";
	// everything else
	_dragBit = nullptr;
	_dragMoved = false;
	_dropTarget = nullptr;
	_oldHolder = nullptr;
	_dragStartPos = ImVec2(0, 0);
	_dragOffset = ImVec2(0, 0);
	_oldPos = ImVec2(0, 0);
}

Game::~Game() {
	for (auto &_turn : _turns) {
		delete _turn;
	}
	_turns.clear();
	for (auto &_player : _players) {
		delete _player;
	}
	_players.clear();

	_gameOps.score = 0;
	_table = nullptr;
	_winner = nullptr;
	_gameOps.currentTurnNo = 0;
	_lastMove = "";
}

void Game::setNumberOfPlayers(unsigned int n) {
	_players.clear();
	for (unsigned int i = 1; i <= n; i++) {
		Player *player = Player::initWithGame(this);
		//		player->setName( std::format( "Player-{}", i ) );
		player->setName("Player");
		player->setPlayerNumber(i - 1); // player numbers are zero-based
		_players.push_back(player);
	}
	_winner = nullptr;

	_gameOps.gameNumber = 0;
	_gameOps.numberOfPlayers = n;

	Turn *turn = Turn::initStartOfGame(this);
	_turns.clear();
	_turns.push_back(turn);
}

void Game::setAIPlayer(unsigned int playerNumber) {
	_players.at(playerNumber)->setAIPlayer(true);
	_gameOps.AIPlayer = playerNumber;
	_gameOps.AIPlayer = true;
}

void Game::startGame() {
	std::string startState = stateString();
	Turn *turn = _turns.at(0);
	turn->_boardState = startState;
	turn->_gameNumber = _gameOps.gameNumber;
	_gameOps.currentTurnNo = 0;
}

void Game::endTurn() {
	_gameOps.currentTurnNo++;
	std::string startState = stateString();
	Turn *turn = new Turn;
	turn->_boardState = stateString();
	turn->_date = (int)_gameOps.currentTurnNo;
	turn->_score = _gameOps.score;
	turn->_gameNumber = _gameOps.gameNumber;
	_turns.push_back(turn);
	ClassGame::EndOfTurn();
}

// scan for mouse is temporarily in the actual game class
// this will be moved to a higher up class when the squares have a heirarchy
// we want the event loop to be elsewhere and calling this class, not called by this class
// but this is fine for tic-tac-toe
void Game::scanForMouse() {
	if (gameHasAI() && getCurrentPlayer() && getCurrentPlayer()->isAIPlayer()) {
		return;
	}
#if defined(UCI_INTERFACE)
	return;
#endif
	ImVec2 mousePos = ImGui::GetMousePos();
	mousePos.x -= ImGui::GetWindowPos().x;
	mousePos.y -= ImGui::GetWindowPos().y;

	Entity *entity = nullptr;
	for (int y = 0; y < _gameOps.Y; y++) {
		for (int x = 0; x < _gameOps.X; x++) {
			BitHolder &holder = getHolderAt(x, y);
			Bit *bit = holder.bit();
			if (bit && bit->isMouseOver(mousePos)) {
				entity = bit;
			}
			else if (holder.isMouseOver(mousePos)) {
				entity = &holder;
			}
		}
	}
	if (ImGui::IsMouseClicked(0)) {
		mouseDown(mousePos, entity);
	}
	else if (ImGui::IsMouseReleased(0)) {
		mouseUp(mousePos, entity);
	}
	else {
		mouseMoved(mousePos, entity);
	}
}

void Game::findDropTarget(ImVec2 &pos) {
	for (int y = 0; y < _gameOps.Y; y++) {
		for (int x = 0; x < _gameOps.X; x++) {
			BitHolder &holder = getHolderAt(x, y);
			if (&holder == _oldHolder) {
				continue;
			}
			if (holder.isMouseOver(pos)) {
				if (_dropTarget && &holder != _dropTarget) {
					_dropTarget->willNotDropBit(_dragBit);
					_dropTarget->setHighlighted(false);
					_dropTarget = nullptr;
				}
				if (holder.canDropBitAtPoint(_dragBit, pos) && canBitMoveFromTo(*_dragBit, *_oldHolder, holder)) {
					_dropTarget = &holder;
					_dropTarget->setHighlighted(true);
				}
			}
		}
	}
}

// draw the board and then the pieces
// this will also go somewhere else when the heirarchy is set up
void Game::drawFrame() {
	scanForMouse();

	for (int y = 0; y < _gameOps.Y; y++) {
		for (int x = 0; x < _gameOps.X; x++) {
			BitHolder &holder = getHolderAt(x, y);
			holder.paintSprite();
		}
	}

	// paint the pieces second so they are always on top of the board as we move them
	for (int y = 0; y < _gameOps.Y; y++) {
		for (int x = 0; x < _gameOps.X; x++) {
			BitHolder &holder = getHolderAt(x, y);
			if (holder.bit() && !holder.bit()->getPickedUp() && !holder.bit()->getMoving()) {
				holder.bit()->paintSprite();
			}
		}
	}

	// now paint the moving pieces
	for (int y = 0; y < _gameOps.Y; y++) {
		for (int x = 0; x < _gameOps.X; x++) {
			BitHolder &holder = getHolderAt(x, y);
			if (holder.bit() && holder.bit()->getMoving() && !holder.bit()->getPickedUp()) {
				holder.bit()->update();
				holder.bit()->paintSprite();
			}
		}
	}

	// now paint any picked up pieces
	for (int y = 0; y < _gameOps.Y; y++) {
		for (int x = 0; x < _gameOps.X; x++) {
			BitHolder &holder = getHolderAt(x, y);
			if (holder.bit() && holder.bit()->getPickedUp()) {
				holder.bit()->paintSprite();
			}
		}
	}
}

void Game::bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {
	endTurn();
}

Bit *Game::bitToPlaceInHolder(BitHolder &holder) {
	return nullptr;
}

bool Game::actionForEmptyHolder(BitHolder &holder) {
	return false;
}

bool Game::clickedBit(Bit &bit) {
	return true;
}

bool Game::animateAndPlaceBitFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {
	return false;
}

bool Game::gameHasAI() {
	return false;
}

void Game::updateAI() {

}

void Game::mouseDown(ImVec2 &location, Entity *entity) {
	bool placing = false;
	_dragStartPos = location;
	if (entity && entity->getEntityType() == Entity::EntityBit) {
		_dragBit = (Bit *)entity;
	}

	if (!_dragBit) {
		if (entity && entity->getEntityType() == Entity::EntityBitHolder) {
			BitHolder *holder = (BitHolder *)entity;
			if (holder) {
				_dragBit = bitToPlaceInHolder(*holder);
				if (_dragBit) {
					_dragOffset.x = _dragOffset.y = 0;
					_dragBit->setPosition(_dragStartPos);
					placing = true;
				}
			}
		}
	}

	if (!_dragBit) {
		return;
	}

	// Clicked on a Bit:
	_dragMoved = false;
	_dropTarget = nullptr;
	_oldHolder = _dragBit->getHolder();
	// Ask holder's and game's permission before dragging:
	if (_oldHolder) {
		_dragBit = _oldHolder->canDragBit(_dragBit);
		if (_dragBit && !(canBitMoveFrom(*_dragBit, *_oldHolder))) {
			_oldHolder->cancelDragBit(_dragBit);
			_dragBit = nullptr;
		}
		if (!_dragBit) {
			_oldHolder = nullptr;
			return;
		}
	}
	// Start dragging:
	_oldPos = _dragBit->getPosition();
	if (_dragBit)
		_dragBit->setPickedUp(true);

	if (placing && _dragBit) {
		_dragBit->setCenterPosition(_dragStartPos); // animate Bit to new position
		_dragMoved = true;
		findDropTarget(_dragStartPos);
	}
}

void Game::mouseMoved(ImVec2 &location, Entity *entity) {
	if (_dragBit) {
		// Get the mouse position, and see if we've moved 3 pixels since the mouseDown:
		ImVec2 pos = location;
		if (std::fabs(pos.x - _dragStartPos.x) >= 12 || std::fabs(pos.y - _dragStartPos.y) >= 12)
			_dragMoved = true;

		// Move the _dragBit (without animation -- it's unnecessary and slows down responsiveness):
		pos.x += _dragOffset.x;
		pos.y += _dragOffset.y;

		_dragBit->setCenterPosition(pos);

		// Find what it's over:
		findDropTarget(pos);
	}
}

void Game::mouseUp(ImVec2 &location, Entity *entity) {
	if (!_dragBit) {
		// If no bit was clicked, see if it's a BitHolder the game will let the user add a Bit to:
		if (entity && entity->getEntityType() == Entity::EntityBitHolder) {
			BitHolder *holder = (BitHolder *)entity;
			if (actionForEmptyHolder(*holder)) {
				_dropTarget = nullptr;
				_dragBit = nullptr;
				return;
			}
		}
	}

	if (_dragBit) {
		if (_dragMoved) {
			// Update the drag tracking to the final mouse position:
			mouseMoved(location, entity);
			if (_dropTarget)
				_dropTarget->setHighlighted(false);
			if (_dragBit)
				_dragBit->setPickedUp(false);
			clearBoardHighlights();

			if (_dropTarget && _dropTarget->bit()) {
				pieceTaken(_dropTarget->bit());
			}

			// Is the move legal?
			if (_dropTarget && _dropTarget->dropBitAtPoint(_dragBit, _dragBit->getPosition())) {
				// Yes, notify the interested parties:
				_dragBit->setPickedUp(false);
				_dragBit->setPosition(_dropTarget->getPosition()); // don't animate
				if (_oldHolder)
					_oldHolder->draggedBitTo(_dragBit, _dropTarget);
				bitMovedFromTo(*_dragBit, *_oldHolder, *_dropTarget);
			}
			else {
				// Nope, cancel:
				if (_dropTarget)
					_dropTarget->willNotDropBit(_dragBit);
				_dragBit->moveTo(_oldPos);
			}
		}
		else {
			// Just a click, without a drag:
			if (_dropTarget)
				_dropTarget->setHighlighted(false);
			if (_dragBit)
				_dragBit->setPickedUp(false);
			if (_oldHolder)
				_oldHolder->cancelDragBit(_dragBit);
			_dragBit->setPosition(_oldPos);
			clickedBit(*_dragBit);
		}
		_dropTarget = nullptr;
		_dragBit = nullptr;
	}
}

void Game::clearBoardHighlights() {

}