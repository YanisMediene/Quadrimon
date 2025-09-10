#ifndef QUADRIMON_H
#define QUADRIMON_H

#include <QString>
//Réalisée par Souley MOHAMAN BELLO.
//Cette classe permet de dréer un quadrimon.
class Quadrimon {
public:
    enum Type {
        Feu,
        Eau,
        Plante
    };//Les différents types de quadrimons.

    //Les différents constructeurs nécessaires dans le code
    Quadrimon();
    Quadrimon(int id);
    Quadrimon(int id, QString nom, Type type, int pointsVie, int pointsAttaque);

    int& getId();// Méthode pour récupérer l'ID
    void setId(int id);//Méthode pour modifier l'ID
    QString& getNom();// Méthode pour récupérer le nom
    void setNom(QString nom);//Méthode pour modifier le nom
    Type& getType();// Méthode pour récupérer le type
    void setType(Type);//Méthode pour modifier le type
    int& getPointsVie();// Méthode pour récupérer le nombre de PV
    void setPointsVie(int pointsVie);//Méthode pour modifier le nombre de PV
    int& getPointsAttaque();// Méthode pour récupérer le nombre de points d'attaque
    void setPointsAttaque(int pointsAttaque);//Méthode pour modifier le nombre de points d'attaque

private:
    int m_id;//Le numéro de la carte dans notre dossier de cartes
    QString m_nom;//Le nom du quadrimon
    Type m_type;//Le type du quadrimon
    int m_pointsVie;//Le nombre de points de vie du quadrimon
    int m_pointsAttaque;//Le nombre de points d'attaque du quadrimon
};

#endif // QUADRIMON_H
