#ifndef JOUEUR_H
#define JOUEUR_H

#include <QString>
#include "quadrimon.h"
#include "terrain.h"

class Joueur {
public:
    Joueur();
    Joueur(Quadrimon quadrimon1, Quadrimon quadrimon2, Terrain terrain);

    Quadrimon& getQuadrimon1();
    void setQuadrimon1(Quadrimon quadrimon1);
    Quadrimon& getQuadrimon2();
    void setQuadrimon2(Quadrimon quadrimon2);
    Terrain& getTerrain();

private:
    Quadrimon m_quadrimon1;
    Quadrimon m_quadrimon2;
    Terrain m_terrain;
};

#endif // JOUEUR_H
