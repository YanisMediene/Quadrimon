#include "capsuleterrain.h"

CapsuleTerrain::CapsuleTerrain(int id) {
    m_id = id;
    switch (id) {
    case 10:
        m_nom = "Ouragan";
        m_tempsActivite = 3;
        m_effetInfo = "+100 PV";
        m_effets.push_back(std::make_pair(PlusPointsVie, 100));
        break;
    case 11:
        m_nom = "Marais";
        m_tempsActivite = 5;
        m_effetInfo = "-50 PV";
        m_effets.push_back(std::make_pair(MoinsPointsVie, 50));
        break;
    case 12:
        m_nom = "ChatGPT";
        m_tempsActivite = 2;
        m_effetInfo = "Copie";
        break;
    case 13:
        m_nom = "Toiles d'araignée";
        m_tempsActivite = 3;
        m_effetInfo = "-100 ATQ";
        m_effets.push_back(std::make_pair(MoinsPointsAttaque, 100));
        break;
    case 14:
        m_nom = "Grotte";
        m_tempsActivite = 2;
        m_effetInfo = "Bloqué";
        m_effets.push_back(std::make_pair(EmpecheChangerQuadrimon, 1));
        break;
    case 15:
        m_nom = "Infirmerie";
        m_tempsActivite = 3;
        m_effetInfo = "+1000 PV";
        m_effets.push_back(std::make_pair(PlusPointsVie, 1000));
        break;
    case 16:
        m_nom = "Volcan";
        m_tempsActivite = 100;
        m_effetInfo = "Type Feu, +100 ATQ";
        m_effets.push_back(std::make_pair(PlusPointsAttaque, 100));
        break;
    case 17:
        m_nom = "Forêt";
        m_tempsActivite = 100;
        m_effetInfo = "Type Plante, +50 PV";
        m_effets.push_back(std::make_pair(PlusPointsVie, 50));
        break;
    case 18:
        m_nom = "Plage";
        m_tempsActivite = 100;
        m_effetInfo = "Type Eau, Bonus Type x2.5";
        m_effets.push_back(std::make_pair(AvantageTypeEau, 100));
        break;
    default:
        m_nom = "Aucun";
        m_tempsActivite = 0;
        m_effetInfo = "Aucun";
        break;
    }
}

CapsuleTerrain::CapsuleTerrain(int id, QString nom, int tempsActivite)
    : m_id(id), m_nom(nom), m_tempsActivite(tempsActivite) {}

void CapsuleTerrain::ajouterEffet(Effet effet, int valeur) {
    m_effets.push_back(std::make_pair(effet, valeur));
}

int& CapsuleTerrain::getId() {
    return m_id;
}

QString& CapsuleTerrain::getNom() {
    return m_nom;
}

int& CapsuleTerrain::getTempsActivite() {
    return m_tempsActivite;
}

QString& CapsuleTerrain::getEffetInfo() {
    return m_effetInfo;
}

void CapsuleTerrain::setTempsActivite(int tempsActivite) {
    m_tempsActivite = tempsActivite;
}

std::vector<std::pair<CapsuleTerrain::Effet, int>>& CapsuleTerrain::getEffets() {
    return m_effets;
}
