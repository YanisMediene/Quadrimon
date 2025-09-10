#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QOpenGLWidget>
//Réalisée par Souley MOHAMAN BELLO
//Classe gère l'affichage de l'arène, des quadrimons et des terrains.
class MyGLWidget : public QOpenGLWidget
{
    Q_OBJECT

public:

    // Constructeur
    MyGLWidget(QWidget * parent = nullptr);
    virtual ~MyGLWidget();
    int indexTerrainGauche = 0;
    int indexTerrainDroite = 0;
    int indexQuadrimonGauche = 0;
    int indexQuadrimonDroite = 0;
    void resetShow();
    void drawTerrainGauche(int terrainIndex);//Dessine la zone de gauche de l'arène
    void drawTerrainDroite(int terrainIndex);//Dessine la zone de droite de l'arène
    int indexAnimationGauche = 0;//Indique quelle animation jouée pour le quadrimon dans la zone de gauche
    int indexAnimationDroite = 0;//Indique quelle animation jouée pour le quadrimon dans la zone de droite


private:
    void idleAnimation(float originalX, float originalY, float originalZ, int id);//Réalise l'animation du quadrimon au repos
    void attackAnimation(float originalX, float originalY, float originalZ, int id, int direction);//Réalise l'animation du quadrimon durant l'attaque
    void damageAnimation(float originalX, float originalY, float originalZ, int id, int direction);//Réalise l'animation du quadrimon recevant des dégâts
    void deathAnimation(float originalX, float originalY, float originalZ, int id);//Réalise l'animation du quadrimon lorsqu'il est vaincu
    void winningAnimation(float originalX, float originalY, float originalZ, int id, int direction); //Réalise l'animation du quadrimon lorsque la partie est gagnée
    float animationTime = 0.0f; //Permet de gérer à quel moment de l'animation on se trouve
    float animationSpeed = 0.1f; // Vitesse de progression de l'animation
    void drawTerrainGaucheForeground(int terrainIndex);//Dessine le premier plan de la zone de gauche
    void drawTerrainGaucheBackground(int terrainIndex);//Dessine le background de la zone de gauche
    void drawTerrainDroiteForeground(int terrainIndex);//Dessine le premier plan de la zone de droite
    void drawTerrainDroiteBackground(int terrainIndex);//Dessine le background de la zone de droite
    void drawQuadrimon(int id);//Dessine le quadrimon n° id, à l'aide des méthodes du bas
    //Ces méthodes permettent de dessiner des quadrimons spécifiques
    void drawSoleil();
    void drawKhone();
    void drawPyramide();
    void drawDonut();
    void drawFlamby();
    void drawSaladier();
    void drawCylindrus();
    void drawGlace();
    void drawOlaf();

private slots:

protected:


    // Fonction d’initialisation appelée lors de la création du widget OpenGL
    void initializeGL();

    // Fonction d'affichage destinée au rendu de la scène OpenGL
    void paintGL();

    // Fonction de redimensionnement appelée lorsque la taille de la fenêtre est modifiée
    void resizeGL(int width, int height);

    //Fonction de gestion des évènements clavier appelée à chaque appui sur une touche du clavier
    //void keyPressEvent(QKeyEvent * event);
};

#endif // MYGLWIDGET_H
