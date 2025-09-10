#ifndef CAPSULETERRAIN_H
#define CAPSULETERRAIN_H

#include <QString>
#include <vector>
//Réalisée par Yanis MEDIENE
//Cette classe permet de créer des capsules terrains.
class CapsuleTerrain {
public:
    enum Effet {
        PlusPointsVie,
        MoinsPointsVie,
        PlusPointsAttaque,
        MoinsPointsAttaque,
        EmpecheChangerQuadrimon, // True = 1, False = 0
        ChangeTypeEnFeu, // True = 1, False = 0
        ChangeTypeEnPlante, // True = 1, False = 0
        ChangeTypeEnEau, // True = 1, False = 0
        AvantageTypeEau // True = 1, False = 0
    };//Les différents types d'effets possibles

    //Constructeurs
    CapsuleTerrain(int id);
    CapsuleTerrain(int id, QString nom, int tempsActivite);

    void ajouterEffet(Effet effet, int valeur = 0);//Ajoute un effet et sa durée d'activation

    int& getId();//Renvoie l'ID
    QString& getNom();//Renvoie le nom
    int& getTempsActivite();//Renvoie la durée d'activation
    QString& getEffetInfo();//Renvoir les infos sur les effets
    void setTempsActivite(int tempsActivite);//Modifie la durée d'activation
    std::vector<std::pair<Effet, int>>& getEffets();//Permet d'obtenir les différents effets et leurs durée d'activation

private:
    int m_id;//Numéro de la carte dans notre base d'images.
    QString m_nom;//Nom de la carte
    int m_tempsActivite;//Durée d'activation de la carte
    QString m_effetInfo;//Infos sur les effets de la carte
    std::vector<std::pair<Effet, int>> m_effets; // Vecteur de paires (effet, valeur)
};

#endif // CAPSULETERRAIN_H
