#include "combat.h"

Combat::Combat()
    : m_joueur1(Joueur()), m_joueur2(Joueur()), m_turn(1),m_currentPlayer(0) {}

Combat::Combat(Joueur joueur1, Joueur joueur2)
    : m_joueur1(joueur1), m_joueur2(joueur2), m_turn(1),m_currentPlayer(0) {}

Joueur& Combat::getJoueur1() {
    return m_joueur1;
}

Joueur& Combat::getJoueur2() {
    return m_joueur2;
}

int& Combat::getTurn() {
    return m_turn;
}

void Combat::setTurn(int turn) {
    m_turn = turn;
}

int& Combat::getCurrentPlayer() {
    return m_currentPlayer;
}

void Combat::setCurrentPlayer(int currentPlayer) {
    m_currentPlayer = currentPlayer;
}

void Combat::endTurn() {
    m_currentPlayer = (m_currentPlayer+1) % 2;
    m_turn++;
}
