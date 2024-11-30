// Author: Pathik Sangani
// Date: November-25-2024
/* Description : This project involves creating a simplified version of the 1024
 or 2048 game, where players slide numbers (which are powers of 2) on a 4x4 grid.
 The goal is to combine matching numbers to form larger numbers, with the ultimate 
 aim of reaching a target number, depending on the difficulty level. */

#include <iostream>
#include <iomanip>
#include <vector>
#include "mersenne-twister.h"

const int BOARD_SIZE = 4; //define size of the board as 4*4

class Board {
private:
    int grid[BOARD_SIZE][BOARD_SIZE]; // 2D array representing the board grid
    int winTarget;  

    // Function to generate a random starting number for the game based on mode (Easy, Medium, or Hard)
    int getRandomStartingNumber(const std::string& mode) {
        int randomValue = chooseRandomNumber(1, 10);
        
        if (mode == "E") return (randomValue <= 5) ? 2 : 4;
        if (mode == "M") return (randomValue <= 7) ? 2 : 4;
        return (randomValue <= 9) ? 2 : 4;
    }

    // Adds a random piece (2 or 4) to an empty position on the board
    void addRandomPiece() {
        std::vector<std::pair<int, int>> emptySquares;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (grid[i][j] == 0) {  //find empty squares
                    emptySquares.push_back({i, j});
                }
            }
        }

        if (emptySquares.empty()) return;

        int randomSquare = chooseRandomNumber(0, emptySquares.size() - 1);
        auto [row, col] = emptySquares[randomSquare];
        grid[row][col] = getRandomStartingNumber(
            winTarget == 256 ? "E" : 
            winTarget == 512 ? "M" : "H"
        );
    }

    // slide the pieces to left and combine them if needed
    bool slidePiecesLeft() {
        bool moved = false;
        for (int row = 0; row < BOARD_SIZE; row++) {
            // Combine and slide
            std::vector<int> newRow;
            for (int col = 0; col < BOARD_SIZE; col++) {
                if (grid[row][col] != 0) {
                    if (!newRow.empty() && newRow.back() == grid[row][col]) {
                        newRow.back() *= 2;
                        moved = true;
                    } else {
                        newRow.push_back(grid[row][col]);
                    }
                }
            }

            // Pad with zeros
            while (newRow.size() < BOARD_SIZE) {
                newRow.push_back(0);
            }

            // Check if row changed
            for (int col = 0; col < BOARD_SIZE; col++) {
                if (grid[row][col] != newRow[col]) {
                    moved = true;
                    grid[row][col] = newRow[col];
                }
            }
        }
        return moved;
    }

    bool slidePiecesRight() {
        // Rotate board to reuse left sliding logic
        rotateBoard();
        rotateBoard();
        bool moved = slidePiecesLeft();
        rotateBoard();
        rotateBoard();
        return moved;
    }

    bool slidePiecesUp() {
        // Rotate board to reuse left sliding logic
        rotateBoard(true);
        bool moved = slidePiecesLeft();
        rotateBoard(false);
        return moved;
    }

    bool slidePiecesDown() {
        // Rotate board to reuse left sliding logic
        rotateBoard(false);
        bool moved = slidePiecesLeft();
        rotateBoard(true);
        return moved;
    }

    // Rotates the board 90 degrees in the specified direction
    void rotateBoard(bool counterClockwise = true) {
        int temp[BOARD_SIZE][BOARD_SIZE];
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (counterClockwise)
                    temp[BOARD_SIZE - 1 - j][i] = grid[i][j];
                else
                    temp[j][BOARD_SIZE - 1 - i] = grid[i][j];
            }
        }
        

        //copy the rotated grid back to original grid
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                grid[i][j] = temp[i][j];
            }
        }
    }

public:
    Board(const std::string& mode) {
        // Initialize grid with zeros
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                grid[i][j] = 0;
            }
        }

        // Set win target based on mode
        winTarget = (mode == "E") ? 256 : (mode == "M") ? 512 : 1024;

        // Add two initial random pieces
        addRandomPiece();
        addRandomPiece();
    }

    // display the current state of the board
    void display() const {
        std::cout << "---------------------\n";
        for (int i = 0; i < BOARD_SIZE; i++) {
            std::cout << "|";
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (grid[i][j] == 0) 
                    std::cout << "    |";
                else 
                    std::cout << std::setw(4) << grid[i][j] << "|";
            }
            std::cout << "\n";
            std::cout << "---------------------\n";
        }
    }

    bool move(char direction) {
        bool moved = false;
        switch(std::toupper(direction)) {
            case 'L': moved = slidePiecesLeft(); break;
            case 'R': moved = slidePiecesRight(); break;
            case 'U': moved = slidePiecesUp(); break;
            case 'D': moved = slidePiecesDown(); break;
            default: return false;
        }

        if (moved) addRandomPiece();
        return moved;
    }

    // check if player has won by reaching the win target
    bool checkWin() const {
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (grid[i][j] == winTarget) return true;
            }
        }
        return false;
    }

    // check if the game is over
    bool isGameOver() const {
        // Check if board is full with no possible moves
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (grid[i][j] == 0) return false;
                
                // Check adjacent cells for possible merges
                if (j < BOARD_SIZE - 1 && grid[i][j] == grid[i][j+1]) return false;
                if (i < BOARD_SIZE - 1 && grid[i][j] == grid[i+1][j]) return false;
            }
        }
        return true;
    }
};


// Main Function
int main() {
    std::cout << "Enter random seed: ";
    int randSeed;
    std::cin >> randSeed;
    seed(randSeed);

    std::string mode;
    do {
        std::cout << "Choose game mode: Easy (E), Medium (M), or Hard (H): ";
        std::cin >> mode;
        
        if (mode != "E" && mode != "M" && mode != "H") {
            std::cout << "Error: Invalid mode.\n";
        }
    } while (mode != "E" && mode != "M" && mode != "H");

    Board gameBoard(mode);
    
    while (!gameBoard.isGameOver()) {
        gameBoard.display();
        
        std::cout << "Enter move: U, D, L, or R. Q to quit: ";
        std::string move;
        std::cin >> move;
        
        if (move == "Q" || move == "q") break;
        
        if (move.length() != 1 || 
            (move[0] != 'U' && move[0] != 'u' && 
             move[0] != 'D' && move[0] != 'd' && 
             move[0] != 'L' && move[0] != 'l' && 
             move[0] != 'R' && move[0] != 'r')) {
            std::cout << "Error: Invalid move.\n";
            continue;
        }
        
        if (!gameBoard.move(move[0])) {
            std::cout << "Invalid move direction.\n";
            continue;
        }
        
        if (gameBoard.checkWin()) {
            gameBoard.display();
            std::cout << "You win!\n";
            break;
        }
    }
    
    if (gameBoard.isGameOver()) {
        gameBoard.display();
        std::cout << "You lose.\n";
    }
    
    return 0;
}
