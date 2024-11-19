#include "Application.h"
#include "imgui/imgui.h"
#include "classes/Chess.h"
#include "tools/Logger.h"

namespace ClassGame {
	Chess *game = nullptr;
	bool gameOver = false;
	int gameWinner = -1;

	// game starting point
	// this is called by the main render loop in main.cpp
	void GameStartUp() {
		game = new Chess();
		game->setUpBoard();
	}

	void drawMoveProber() {
		const ImGuiTableFlags flags = ImGuiTableFlags_Borders |
							ImGuiTableFlags_RowBg;

		ImGui::BeginChild("Moves", ImVec2(0, 0), true); 
		if (ImGui::BeginTable("Move Prober", 2, flags)) {
			ImGui::TableSetupColumn("I", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultSort, 12.0f);
			ImGui::TableSetupColumn("Moves", ImGuiTableColumnFlags_WidthFixed  | ImGuiTableColumnFlags_DefaultSort);
			ImGui::TableHeadersRow();

			std::unordered_map<uint8_t, std::vector<Move>> moves = game->getMoves();
			std::vector<std::pair<int, std::vector<Move>>> moveList;
			moveList.reserve(64);
			for (int i = 0; i < 64; i++) {
				moveList.emplace_back(i, moves[i]);
			}

			for(const std::pair<int, std::vector<Move>>& data : moveList) {
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s", std::to_string(data.first).c_str());

				ImGui::TableSetColumnIndex(1);
				std::string moves;
				for (Move i : data.second) {
					moves += std::to_string(i.getTo()) + " ";
				}
				ImGui::Text("%s", moves.c_str());
			}

			ImGui::EndTable();
		}
		ImGui::EndChild();
	}

	void drawState() {
		ImGui::BeginChild("State", ImVec2(300, 200), true);
		GameState state = game->getState();
		ImGui::Text("Game State");
		ImGui::Text("Clock: %d", state.getClock());
		ImGui::Text("Half Clock: %d", state.getHalfClock());
		ImGui::Text("Black's Turn? %d", state.isBlackTurn());
		ImGui::Text("En Passant Square: %s", ChessSquare::indexToPosNotation(state.getEnPassantSquare()).c_str());
		ImGui::Text("Castle Rights %d", state.getCastlingRights());

		std::string stateStr = "";
		const char* t = state.getState();
		for (int i = 0; i < 64; i++) {
			if (i % 8 == 0) {
				stateStr += '\n';
			}
			stateStr += t[i];
		}
		ImGui::Text("%s", stateStr.c_str());
		ImGui::EndChild();
	}

	// game render loop
	// this is called by the main render loop in main.cpp
	void RenderGame() {
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		//ImGui::ShowDemoWindow();

		ImGui::Begin("Settings");
		ImGui::Text("Current Player Number: %d", game->getCurrentPlayer()->playerNumber());
		ImGui::Text("Current Board State: %s", game->stateString().c_str());

		drawState();
		drawMoveProber();

		if (gameOver) {
			ImGui::Text("Game Over!");
			ImGui::Text("Winner: %d", gameWinner);
			if (ImGui::Button("Reset Game")) {
				game->stopGame();
				game->setUpBoard();
				gameOver = false;
				gameWinner = -1;
			}
		}
		ImGui::End();

		ImGui::Begin("GameWindow");
		game->drawFrame();
		ImGui::End();
		Loggy.draw();
	}

	// end turn is called by the game code at the end of each turn
	// this is where we check for a winner
	void EndOfTurn() {
		Player *winner = game->checkForWinner();
		if (winner) {
			gameOver = true;
			gameWinner = winner->playerNumber();
		}
		if (game->checkForDraw()) {
			gameOver = true;
			gameWinner = -1;
		}
	}
}
