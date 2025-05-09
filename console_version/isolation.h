#include <stdio.h>
#include <stdlib.h>
#include "katarenga.h"

void initialize_empty_board(char board[8][8]) {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            board[i][j] = '.';
}


short switchTurn(short turn);
short correctCoord(short x, short y);
short selectPawn(short** pawns, short turn, short x, short y, short printError);
short canPlace(short** pawns, short turn, short x, short y, short round);
void setMovable(short** pawns, short x, short y);
short possibleMove(short** pawns, short x, short y);
short setMoves(short** board, short** pawns, short turn, short x, short y, short directReturn, short round);
void clearMovable(short** pawns);

bool estMenacee(Case *caseDest, ListePions *pionsAdverses) {
    for (int i = 0; i < pionsAdverses->taille; i++) {
        Pion *pion = pionsAdverses->pions[i];
        switch (pion->couleur) {
            case BLEU:
                if (deplacementRoi(pion, caseDest)) return true;
                break;
            case VERT:
                if (deplacementCavalier(pion, caseDest)) return true;
                break;
            case JAUNE:
                if (deplacementFou(pion, caseDest)) return true;
                break;
            case ROUGE:
                if (deplacementTour(pion, caseDest)) return true;
                break;
            default:
                break;
        }
    }
    return false; // case pas menacé
}
bool aucunPlacementPossible(Plateau *plateau, ListePions *pionsAdverses) {
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            Case *c = &plateau->grille[x][y];
            if (c->pion == NULL && !estMenacee(c, pionsAdverses)) {
                return false; // un pion peux encore être placé légalement
            }
        }
    }
    return true; // plus de déplacement
}

if (aucunPlacementPossible(plateau, listePionsAdverses)) {
    printf("Le joueur %s a perdu, il ne peut plus poser de pion sans être menacé.\n", joueurActuel->nom);
    finDePartie = true;
}
