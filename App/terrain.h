#ifndef TERRAIN_H
#define TERRAIN_H

#include "quadrimon.h"
#include "capsuleterrain.h"
//Réalisée par MOHAMAN BELLO Souley
//Cette classe permet de gérer une zone parmi les deux qui constituent l'arène. Plus précisément, elle permet de savoir quel quadrimon et quelle capsule sont présents sur cette zone.
class Terrain {
public:
    Terrain();//Constructeur
    Terrain(Quadrimon quadrimon);

    Quadrimon& getQuadrimon();//Récupère le quadrimon
    void setQuadrimon(Quadrimon quadrimon);//Modifie le quadrimon

    bool hasCapsuleTerrain();//Indique si une capsule est présente
    CapsuleTerrain& getCapsuleTerrain();//Récupère la capsule
    void setCapsuleTerrain(CapsuleTerrain capsuleTerrain);//Modifie la capsule

private:
    Quadrimon m_quadrimon;//Le quadrimon présent sur la zone
    CapsuleTerrain m_capsuleTerrain;//La capsule présente sur la zone
};

#endif // TERRAIN_H
