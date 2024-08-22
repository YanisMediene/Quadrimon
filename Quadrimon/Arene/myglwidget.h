#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QOpenGLWidget>

// Classe dediee a l'affichage d'une scene OpenGL
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
    void drawTerrainGauche(int terrainIndex);
    void drawTerrainDroite(int terrainIndex);
    int indexAnimationGauche = 0;
    int indexAnimationDroite = 0;


private:
    void idleAnimation(float originalX, float originalY, float originalZ, int id);
    void attackAnimation(float originalX, float originalY, float originalZ, int id, int direction);
    void damageAnimation(float originalX, float originalY, float originalZ, int id, int direction);
    void deathAnimation(float originalX, float originalY, float originalZ, int id);
    void winningAnimation(float originalX, float originalY, float originalZ, int id, int direction);
    float animationTime = 0.0f; // Elapsed time for animation
    float animationSpeed = 0.1f; // Speed of animation progression
    void drawTerrainGaucheForeground(int terrainIndex);
    void drawTerrainGaucheBackground(int terrainIndex);
    void drawTerrainDroiteForeground(int terrainIndex);
    void drawTerrainDroiteBackground(int terrainIndex);
    void drawQuadrimon(int id);
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
