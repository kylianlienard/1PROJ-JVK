// La gestion de la fenêtre + style de title bar

short endWindow(struct Player* players, short** pawns, short** board) {
    free(players);

    for (int i = 0; i < 8; i++) {
        free(pawns[i]);
        free(board[i]);
    }
    free(pawns);
    free(board);

    return 0;
};
