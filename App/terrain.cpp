#include "terrain.h"

Terrain::Terrain() : m_quadrimon(Quadrimon()),m_capsuleTerrain(CapsuleTerrain(0)) {}

Terrain::Terrain(Quadrimon quadrimon)
    : m_quadrimon(quadrimon),m_capsuleTerrain(CapsuleTerrain(0)) {}

Quadrimon& Terrain::getQuadrimon() {
    return m_quadrimon;
}

void Terrain::setQuadrimon(Quadrimon quadrimon) {
    m_quadrimon = quadrimon;
}

bool Terrain::hasCapsuleTerrain() {
    return m_capsuleTerrain.getId() != 0;
}

CapsuleTerrain& Terrain::getCapsuleTerrain() {
    return m_capsuleTerrain;
}

void Terrain::setCapsuleTerrain(CapsuleTerrain capsuleTerrain) {
    m_capsuleTerrain = capsuleTerrain;
}
