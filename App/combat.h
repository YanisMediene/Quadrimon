#ifndef COMBAT_H
#define COMBAT_H

#include "joueur.h"
//Réalisée par Yanis MEDIENE
//Cette classe gère l'état des joueurs et du tour(à qui est la main).
class Combat {
public:
    //Constructeurs
    Combat();
    Combat(Joueur joueur1, Joueur joueur2);

    Joueur& getJoueur1();//Renvoie le joueur 1
    Joueur& getJoueur2();//Renvoie le joueur 2
    int& getTurn();//Renvoie le nombre de tours joués
    void setTurn(int turn);// Modifie le nombre de tours joués
    int& getCurrentPlayer();//Renvoie le numéro du joueur qui à la main
    void setCurrentPlayer(int currentPlayer);//Modifie le numéro du joueur qui à la main
    void endTurn();//Met à jour le nombre de tours joués et le numéro du current player à la fin d'un tour.

private:
    Joueur m_joueur1;//Joueur 1
    Joueur m_joueur2;//Joueur 2
    int m_turn;//Nombre de tours joués
    int m_currentPlayer;//Numéro du joueur qui à la main
};

#endif // COMBAT_H
