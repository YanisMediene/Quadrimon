#ifndef COMBAT_H
#define COMBAT_H

#include "joueur.h"

class Combat {
public:
    Combat();
    Combat(Joueur joueur1, Joueur joueur2);

    Joueur& getJoueur1();
    Joueur& getJoueur2();
    int& getTurn();
    void setTurn(int turn);
    int& getCurrentPlayer();
    void setCurrentPlayer(int currentPlayer);
    void endTurn();

private:
    Joueur m_joueur1;
    Joueur m_joueur2;
    int m_turn;
    int m_currentPlayer;
};

#endif // COMBAT_H
