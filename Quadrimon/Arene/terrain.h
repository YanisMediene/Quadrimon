#ifndef TERRAIN_H
#define TERRAIN_H

#include "quadrimon.h"
#include "capsuleterrain.h"

class Terrain {
public:
    Terrain();
    Terrain(Quadrimon quadrimon);

    Quadrimon& getQuadrimon();
    void setQuadrimon(Quadrimon quadrimon);

    bool hasCapsuleTerrain();
    CapsuleTerrain& getCapsuleTerrain();
    void setCapsuleTerrain(CapsuleTerrain capsuleTerrain);

private:
    Quadrimon m_quadrimon;
    CapsuleTerrain m_capsuleTerrain;
};

#endif // TERRAIN_H
