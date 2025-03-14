void initPawns(pawns) {
    for (short i; i < 8; i++) {
        pawns[0][i] = 2;
        pawns[7][i] = 1;
    }
}

short selectPawn(board, pawns, player, x, y) {
    return pawns[x][y] == player;
}

short end(board, pawns, player) {
    short i, j;
    for (i=0; i < 8; i++) {
        for (j=0; j < 8, j++) {
            
        } 
    }
}

void katarenga(board, pawns) {
    short pl = 1;
    short x, y;

    initPawns();
    while (end(board, pawns)) {
        do {
            x = inputInt("Enter x");
            y = inputInt("enter y")
        } while(selectPawn(board, pawns, player))

        do {
            x = inputInt("Enter x");
            y = inputInt("enter y")
        } while(selectPawn(board, pawns, player))
    }
    printf("_ucuc"); 
    return 0;
}