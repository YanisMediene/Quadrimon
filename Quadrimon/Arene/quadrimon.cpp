#include "quadrimon.h"

Quadrimon::Quadrimon() : m_id(0), m_nom("MissingNo"),m_type(Feu),m_pointsVie(1000), m_pointsAttaque(1000) {}

Quadrimon::Quadrimon(int id) {
    m_id = id;
    switch (id) {
    case 1:
        m_nom = "Cylindrus le texturé";
        m_type = Eau;
        m_pointsAttaque = 50;
        m_pointsVie = 700;
        break;
    case 2:
        m_nom = "Glace à la fraise";
        m_type = Eau;
        m_pointsAttaque = 100;
        m_pointsVie = 600;
        break;
    case 3:
        m_nom = "Olaf le bonhomme de neige";
        m_type = Eau;
        m_pointsAttaque = 150;
        m_pointsVie = 500;
        break;
    case 4:
        m_nom = "Soleil du TD d'OpenGL";
        m_type = Feu;
        m_pointsAttaque = 150;
        m_pointsVie = 400;
        break;
    case 5:
        m_nom = "Méca-Pyramide Gizeh";
        m_type = Feu;
        m_pointsAttaque = 400;
        m_pointsVie = 200;
        break;
    case 6:
        m_nom = "Khone, Roi démon démoniaque suprême";
        m_type = Feu;
        m_pointsAttaque = 350;
        m_pointsVie = 350;
        break;
    case 7:
        m_nom = "Flamby, Déesse ultime de la création";
        m_type = Plante;
        m_pointsAttaque = 0;
        m_pointsVie = 900;
        break;
    case 8:
        m_nom = "Menu Donut Vegan";
        m_type = Plante;
        m_pointsAttaque = 200;
        m_pointsVie = 500;
        break;
    case 9:
        m_nom = "Saladier Mystique";
        m_type = Plante;
        m_pointsAttaque = 100;
        m_pointsVie = 800;
        break;
    default:
        m_nom = "MissingNo";
        m_type = Feu;
        m_pointsAttaque = 1000;
        m_pointsVie = 1000;
        break;
    }
}

Quadrimon::Quadrimon(int id, QString nom, Type type, int pointsVie, int pointsAttaque)
    : m_id(id), m_nom(nom), m_type(type), m_pointsVie(pointsVie), m_pointsAttaque(pointsAttaque) {}

int& Quadrimon::getId() {
    return m_id;
}

void Quadrimon::setId(int id) {
    m_id = id;
}

QString& Quadrimon::getNom() {
    return m_nom;
}

void Quadrimon::setNom(QString nom) {
    m_nom = nom;
}

Quadrimon::Type& Quadrimon::getType() {
    return m_type;
}

void Quadrimon::setType(Type type) {
    m_type = type;
}

int& Quadrimon::getPointsVie() {
    return m_pointsVie;
}

void Quadrimon::setPointsVie(int pointsVie) {
    m_pointsVie = pointsVie;
}

int& Quadrimon::getPointsAttaque() {
    return m_pointsAttaque;
}

void Quadrimon::setPointsAttaque(int pointsAttaque) {
    m_pointsAttaque = pointsAttaque;
}

