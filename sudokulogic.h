#pragma once
#ifndef SUDOKULOGIC_H
#define SUDOKULOGIC_H

#include <QDebug>
#include <random>
#include <vector>
#include <algorithm>

const int SIZE = 9;

class SudokuLogic {
public:
    SudokuLogic();

    // Core Sudoku algorithms
    bool isValid(int board[SIZE][SIZE], int row, int col, int num);
    bool generateFullBoard(int board[SIZE][SIZE], int row = 0, int col = 0);
    bool solveSudoku(int board[SIZE][SIZE], int row, int col, int& solutionCount);
    void removeNumbers(int board[SIZE][SIZE], int difficulty);

    // Helper functions
    void printBoard(int board[SIZE][SIZE]);
    bool hasUniqueSolution(int board[SIZE][SIZE], int solution[SIZE][SIZE]);
    bool isBoardCompleteAndCorrect(int board[SIZE][SIZE], int solution[SIZE][SIZE],
        const QVector<QVector<QString>>& cellTexts);
};

#endif // SUDOKULOGIC_H
