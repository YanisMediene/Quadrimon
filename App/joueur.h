#ifndef JOUEUR_H
#define JOUEUR_H

#include <QString>
#include "quadrimon.h"
#include "terrain.h"
//Réalisée par Yanis MEDIENE
//Cette classe permet de gérer les joueurs, c'est-à-dire de savoir quels quadrimons et l'état de leur zonee de combat(quel quadrimon est déployé avec quelle capsule).
class Joueur {
public:
    //Constructeurs de la classe
    Joueur();
    Joueur(Quadrimon quadrimon1, Quadrimon quadrimon2, Terrain terrain);

    Quadrimon& getQuadrimon1();//Renvoie le 1er quadrimon
    void setQuadrimon1(Quadrimon quadrimon1);//Modifie le 1er quadrimon
    Quadrimon& getQuadrimon2();//Renvoie le 2nd quadrimon
    void setQuadrimon2(Quadrimon quadrimon2);//Modifie le 2nd quadrimon
    Terrain& getTerrain();//Renvoie le terrain

private:
    Quadrimon m_quadrimon1;//1er quadrimon du joueur
    Quadrimon m_quadrimon2;//2nd quadrimon
    Terrain m_terrain;//Etat de la zone de combat du joueur
};

#endif // JOUEUR_H
