#ifndef CAPSULETERRAIN_H
#define CAPSULETERRAIN_H

#include <QString>
#include <vector>

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
    };

    CapsuleTerrain(int id);
    CapsuleTerrain(int id, QString nom, int tempsActivite);
    void ajouterEffet(Effet effet, int valeur = 0);

    int& getId();
    QString& getNom();
    int& getTempsActivite();
    QString& getEffetInfo();
    void setTempsActivite(int tempsActivite);
    std::vector<std::pair<Effet, int>>& getEffets();

private:
    int m_id;
    QString m_nom;
    int m_tempsActivite;
    QString m_effetInfo;
    std::vector<std::pair<Effet, int>> m_effets; // Vecteur de paires (effet, valeur)
};

#endif // CAPSULETERRAIN_H
