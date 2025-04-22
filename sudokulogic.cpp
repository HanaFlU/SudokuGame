#include "sudokulogic.h"

SudokuLogic::SudokuLogic() {
    // Constructor
}

bool SudokuLogic::isValid(int board[SIZE][SIZE], int row, int col, int num) {
    // Check row and column
    for (int i = 0; i < SIZE; i++) {
        if (board[row][i] == num && i != col) return false;
        if (board[i][col] == num && i != row) return false;
    }

    // Check 3x3 box
    int startRow = (row / 3) * 3, startCol = (col / 3) * 3;
    for (int i = startRow; i < startRow + 3; i++) {
        for (int j = startCol; j < startCol + 3; j++) {
            if (i == row && j == col) continue;
            if (board[i][j] == num) return false;
        }
    }
    return true;
}

bool SudokuLogic::generateFullBoard(int board[SIZE][SIZE], int row, int col) {
    if (row == SIZE) return true; // end of board

    int nextRow = row, nextCol = col + 1;
    if (nextCol == SIZE) {
        nextRow = row + 1;
        nextCol = 0;
    }

    if (board[row][col] != 0) return generateFullBoard(board, nextRow, nextCol);

    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(numbers.begin(), numbers.end(), g);

    for (int num : numbers) {
        if (isValid(board, row, col, num)) {
            board[row][col] = num;
            if (generateFullBoard(board, nextRow, nextCol)) {
                return true;
            }
            board[row][col] = 0; // Backtrack
        }
    }
    return false;
}

bool SudokuLogic::solveSudoku(int currentBoard[SIZE][SIZE], int row, int col, int& solutionCount) {
    while (row < SIZE && currentBoard[row][col] != 0) {
        col++;
        if (col == SIZE) {
            row++;
            col = 0;
        }
    }

    // no empty cell, a solution is reached
    if (row == SIZE) {
        solutionCount++;
        return solutionCount <= 1;
    }

    for (int num = 1; num <= SIZE; num++) {
        int tempBoardCheck[SIZE][SIZE];
        std::copy(&currentBoard[0][0], &currentBoard[0][0] + SIZE * SIZE, &tempBoardCheck[0][0]);
        tempBoardCheck[row][col] = num;

        bool placement_valid = true;
        // Check row
        for (int k = 0; k < SIZE; ++k) if (k != col && tempBoardCheck[row][k] == num) placement_valid = false;
        // Check col
        if (placement_valid) for (int k = 0; k < SIZE; ++k) if (k != row && tempBoardCheck[k][col] == num) placement_valid = false;
        // Check box
        if (placement_valid) {
            int startRow = (row / 3) * 3;
            int startCol = (col / 3) * 3;
            for (int i = startRow; i < startRow + 3; ++i) {
                for (int j = startCol; j < startCol + 3; ++j) {
                    if ((i != row || j != col) && tempBoardCheck[i][j] == num) placement_valid = false;
                }
            }
        }

        if (placement_valid) {
            currentBoard[row][col] = num;

            if (!solveSudoku(currentBoard, row, col, solutionCount)) {
                // false (> 1 solution), stop
                currentBoard[row][col] = 0; // Backtrack
                return false;
            }
            // stop early
            if (solutionCount > 1) {
                currentBoard[row][col] = 0; // Backtrack
                return false;
            }
        }
    }

    currentBoard[row][col] = 0; // backtrack
    
    return solutionCount <= 1;
}

void SudokuLogic::removeNumbers(int currentBoard[SIZE][SIZE], int difficulty) {
    int cellsToRemove;
    switch (difficulty) {
    case 1: cellsToRemove = 35; break; // Easy
    case 2: cellsToRemove = 45; break; // Medium
    case 3: cellsToRemove = 55; break; // Hard
    default: cellsToRemove = 45; break;
    }

    std::random_device rd;
    std::mt19937 g(rd());
    int removedCount = 0;
    int attempts = 0;

    std::vector<std::pair<int, int>> cellsList;
    for (int r = 0; r < SIZE; ++r) {
        for (int c = 0; c < SIZE; ++c) {
            cellsList.push_back({ r, c });
        }
    }
    std::shuffle(cellsList.begin(), cellsList.end(), g);

    for (const auto& cell : cellsList) {
        if (removedCount >= cellsToRemove) break;
        if (attempts > SIZE * SIZE * 2) break;

        int row = cell.first;
        int col = cell.second;

        if (currentBoard[row][col] != 0) {
            int tempVal = currentBoard[row][col];
            currentBoard[row][col] = 0; 
            attempts++;

            // Check uniqueness
            int tempBoard[SIZE][SIZE];
            std::copy(&currentBoard[0][0], &currentBoard[0][0] + SIZE * SIZE, &tempBoard[0][0]);
            int solutionCount = 0;
            solveSudoku(tempBoard, 0, 0, solutionCount);

            if (solutionCount != 1) {
                currentBoard[row][col] = tempVal;
            }
            else {
                removedCount++;
            }
        }
    }
    qDebug() << "Removed" << removedCount << "cells for difficulty" << difficulty;
}

//void SudokuLogic::printBoard(int pBoard[SIZE][SIZE]) {
//    qDebug() << "Current Board State:";
//    for (int i = 0; i < SIZE; i++) {
//        QString rowStr = "";
//        for (int j = 0; j < SIZE; j++) {
//            rowStr += QString::number(pBoard[i][j]) + " ";
//        }
//        qDebug() << rowStr;
//    }
//    qDebug() << "======================";
//}

bool SudokuLogic::hasUniqueSolution(int board[SIZE][SIZE], int solution[SIZE][SIZE]) {
    int tempBoard[SIZE][SIZE];
    std::copy(&board[0][0], &board[0][0] + SIZE * SIZE, &tempBoard[0][0]);

    int solutionCount = 0;
    solveSudoku(tempBoard, 0, 0, solutionCount);

    if (solutionCount == 1) {
        std::copy(&tempBoard[0][0], &tempBoard[0][0] + SIZE * SIZE, &solution[0][0]);
    }

    qDebug() << "Found" << solutionCount << "solutions.";
    return solutionCount == 1;
}

bool SudokuLogic::isBoardCompleteAndCorrect(int board[SIZE][SIZE], int solution[SIZE][SIZE], const QVector<QVector<QString>>& cellTexts) {
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            QString text = cellTexts[row][col];
            if (text.isEmpty()) {
                return false;
            }
            bool ok;
            int val = text.toInt(&ok);
            if (!ok || val != solution[row][col]) {
                return false;
            }
        }
    }
    return true;
}
