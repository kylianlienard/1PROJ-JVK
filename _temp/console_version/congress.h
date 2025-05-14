#include "katarenga.h" 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void initCongress(short** pawns) {
    // initialiser tout à 0
    for (short i = 0; i < 8; i++)
        for (short j = 0; j < 8; j++)
            pawns[i][j] = 0;

    // pions noirs
    pawns[0][1] = 2;
    pawns[0][4] = 2;
    pawns[1][7] = 2;
    pawns[3][0] = 2;
    pawns[4][7] = 2;
    pawns[6][0] = 2;
    pawns[7][3] = 2;
    pawns[7][6] = 2;

    // pions blancs
    pawns[0][3] = 1;
    pawns[0][6] = 1;
    pawns[1][0] = 1;
    pawns[3][7] = 1;
    pawns[4][0] = 1;
    pawns[6][7] = 1;
    pawns[7][1] = 1;
    pawns[7][4] = 1;
}


short switchTurn(short turn);
short correctCoord(short x, short y);
short selectPawn(short** pawns, short turn, short x, short y, short printError);
short canPlace(short** pawns, short turn, short x, short y, short round);
void setMovable(short** pawns, short x, short y);
short possibleMove(short** pawns, short x, short y);
short setMoves(short** board, short** pawns, short turn, short x, short y, short directReturn, short round);
void clearMovable(short** pawns);


void dfs_congress(int x, int y, bool visited[8][8], char board[8][8], char player) {
    if (x < 0 || x >= 8 || y < 0 || y >= 8) return;
    if (visited[x][y] || board[x][y] != player) return;

    visited[x][y] = true;

    dfs_congress(x + 1, y, visited, board, player); // bas
    dfs_congress(x - 1, y, visited, board, player); // haut
    dfs_congress(x, y + 1, visited, board, player); // droite
    dfs_congress(x, y - 1, visited, board, player); // gauche
}


void countcongress(int x, int y, bool visited[8][8], char board[8][8], char player) {
    if (x < 0 || x >= 8 || y < 0 || y >= 8) return;
    if (visited[x][y] || board[x][y] != player) return;

    visited[x][y] = true;

    countcongress(x + 1, y, visited, board, player);
    countcongress(x - 1, y, visited, board, player);
    countcongress(x, y + 1, visited, board, player);
    countcongress(x, y - 1, visited, board, player);
}

bool congress_are_pawns_connected(char board[8][8], char player) {
    bool visited[8][8] = {false};
    int total_pawns = 0;
    int connected_pawns = 0;
    int start_x = -1, start_y = -1;

    // trouver un pion initial
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] == player) {
                total_pawns++;
                if (start_x == -1) {
                    start_x = i;
                    start_y = j;
                }
            }
        }
    }

    if (total_pawns == 0) return false;

    countcongress(start_x, start_y, visited, board, player);

    // cmbien de pions connectés
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (visited[i][j]) connected_pawns++;
        }
    }

    // tout les pions sont connectés
    return connected_pawns == total_pawns && total_pawns == 8;
}

if (congress_are_pawns_connected(board, current_player)) {
    printf("Le joueur %c a gagné !\n", current_player);
    break; 
}