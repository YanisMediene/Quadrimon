#ifndef QUADRIMON_H
#define QUADRIMON_H

#include <QString>

class Quadrimon {
public:
    enum Type {
        Feu,
        Eau,
        Plante
    };

    Quadrimon();
    Quadrimon(int id);
    Quadrimon(int id, QString nom, Type type, int pointsVie, int pointsAttaque);

    int& getId();
    void setId(int id);
    QString& getNom();
    void setNom(QString nom);
    Type& getType();
    void setType(Type);
    int& getPointsVie();
    void setPointsVie(int pointsVie);
    int& getPointsAttaque();
    void setPointsAttaque(int pointsAttaque);

private:
    int m_id;
    QString m_nom;
    Type m_type;
    int m_pointsVie;
    int m_pointsAttaque;
};

#endif // QUADRIMON_H
