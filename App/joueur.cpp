#include "joueur.h"

Joueur::Joueur()
    : m_quadrimon1(), m_quadrimon2(), m_terrain() {
    // Default constructor initializes member objects using their default constructors
}

Joueur::Joueur(Quadrimon quadrimon1, Quadrimon quadrimon2, Terrain terrain)
    : m_quadrimon1(quadrimon1), m_quadrimon2(quadrimon2), m_terrain(terrain) {}

Quadrimon& Joueur::getQuadrimon1() {
    return m_quadrimon1;
}

void Joueur::setQuadrimon1(Quadrimon quadrimon1) {
    m_quadrimon1 = quadrimon1;
}

Quadrimon& Joueur::getQuadrimon2() {
    return m_quadrimon2;
}

void Joueur::setQuadrimon2(Quadrimon quadrimon2) {
    m_quadrimon2 = quadrimon2;
}

Terrain& Joueur::getTerrain() {
    return m_terrain;
}
