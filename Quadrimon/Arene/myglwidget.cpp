#define _USE_MATH_DEFINES

#include "myglwidget.h"
#include <QApplication>
#include <QImage>

// Pour mac OS,
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
// Pour Windows
//#include <GL/glu.h>
//#include <GL/gl.h>


// Declarations des constantes
const unsigned int WIN_WIDTH_HEIGHT = 900;

// Declare variables to control camera movement
float cameraX = -2.5f; // Initial position of the camera
float cameraSpeed = 0.05f; // Adjust the speed of the camera movement


// Constructeur
MyGLWidget::MyGLWidget(QWidget * parent) : QOpenGLWidget(parent)
{
    resize(WIN_WIDTH_HEIGHT, WIN_WIDTH_HEIGHT);
}

MyGLWidget::~MyGLWidget()
{
}

// Fonction d'initialisation
void MyGLWidget::initializeGL()
{
    // Reglage de la couleur de fond
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // Activation de l'éclairage
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
}

// Fonction de redimensionnement
void MyGLWidget::resizeGL(int width, int height)
{
    // Definition du viewport (zone d'affichage)
    glViewport(0, 0, width, height);
}

// Fonction d'affichage
void MyGLWidget::paintGL()
{
    cameraX += cameraSpeed;
    if (cameraX >= 2.5f || cameraX <= -2.5f) {
        cameraSpeed = -cameraSpeed; // Reverse direction at the end points
    }

    // Effacement du tampon de couleur et du tampon de profondeur
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    // Définition de la projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0f, (double)width() / height(), 2.0f, 20.0f);

    // Définition de la vue
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraX, 8.0f, 8.0f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);

    // Activation de l'éclairage
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);


    // Définition de la couleur et de la direction de la lumière
    GLfloat lightPos[] = {0.0f, 5.0f, 0.0f, 1.0f}; // Direction de la lumière
    GLfloat lightColor[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Couleur de la lumière

    // Activation de la lumière et configuration de ses propriétés
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);

    // Terrain de gauche
    drawTerrainGauche(indexTerrainGauche);

    // Terrain de droite
    drawTerrainDroite(indexTerrainDroite);

    // Affichage des Quadrimon sur le terrain gauche (joueur 1)
    if (indexQuadrimonGauche != 0) {
        glPushMatrix();
        glTranslatef(-2.5f, 0.5f, 0.0f);
        if (indexAnimationGauche == 0) {
            // Idle animation
            idleAnimation(0.0f, 0.0f, 0.0f, indexQuadrimonGauche);
        } else if (indexAnimationGauche == 1) {
            // Attack animation
            attackAnimation(0.0f, 0.0f, 0.0f, indexQuadrimonGauche, 1);
        } else if (indexAnimationGauche == 2) {
            // Damage animation
            damageAnimation(0.0f, 0.0f, 0.0f, indexQuadrimonGauche, 1);
        } else if (indexAnimationGauche == 3) {
            // Death animation
            deathAnimation(-2.0f, 0.5f, 0.0f, indexQuadrimonGauche);
        } else if (indexAnimationGauche == 4) {
            // Winning animation
            winningAnimation(0.0f, 0.0f, 0.0f, indexQuadrimonGauche, 1);
        }
        glPopMatrix();
    }

    // Affichage des Quadrimon sur le terrain droit (joueur 2)
    if (indexQuadrimonDroite != 0) {
        glPushMatrix();
        glTranslatef(2.5f, 0.5f, 0.0f);
        if (indexAnimationDroite == 0) {
            // Idle animation
            idleAnimation(0.0f, 0.0f, 0.0f, indexQuadrimonDroite);
        } else if (indexAnimationDroite == 1) {
            // Attack animation
            attackAnimation(0.0f, 0.0f, 0.0f, indexQuadrimonDroite, -1);
        } else if (indexAnimationDroite == 2) {
            // Damage animation
            damageAnimation(0.0f, 0.0f, 0.0f, indexQuadrimonDroite, -1);
        } else if (indexAnimationDroite == 3) {
            // Death animation
            deathAnimation(0.0f, 0.0f, 0.0f, indexQuadrimonDroite);
        } else if (indexAnimationDroite == 4) {
            // Winning animation
            winningAnimation(0.0f, 0.0f, 0.0f, indexQuadrimonDroite, -1);
        }
        glPopMatrix();
    }

    // Désactivation de l'éclairage à la fin du dessin
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    update();
}

void MyGLWidget::resetShow() {
    indexTerrainGauche = 0;
    indexTerrainDroite = 0;
    indexQuadrimonGauche = 0;
    indexQuadrimonDroite = 0;
}

void MyGLWidget::idleAnimation(float originalX, float originalY, float originalZ, int id) {
    float scaleFactor = 1.0f + 0.05f * sin(animationTime * 1.0f); // Adjust frequency and amplitude as needed

    glPushMatrix();
    glTranslatef(originalX, originalY, originalZ);
    glScalef(scaleFactor, scaleFactor, scaleFactor); // Apply scale transformation
    drawQuadrimon(id); // Draw the Quadrimon at the specified position
    glPopMatrix();

    // Increment animation time for the next frame
    animationTime += animationSpeed;
}

void MyGLWidget::attackAnimation(float originalX, float originalY, float originalZ, int id, int direction) {
    // Calculate animation parameters
    float jumpHeight = 2.0f; // Adjust as needed
    float jumpDistance = 6.0f; // Adjust as needed
    float jumpDuration = 10.0f; // Adjust as needed
    float currentTime = fmod(animationTime, jumpDuration); // Calculate current time within one jump cycle
    float jumpProgress = currentTime / jumpDuration; // Calculate jump progress from 0 to 1

    // Calculate vertical position based on jump progress
    float yPosition = originalY + jumpHeight * (4 * jumpProgress - 4 * jumpProgress * jumpProgress);

    // Calculate horizontal position based on jump progress
    float xPosition;
    if (currentTime < jumpDuration / 2.0f) {
        // Quadrimon is moving forward
        xPosition = originalX + direction * jumpDistance * jumpProgress;
    } else {
        // Quadrimon is moving backward
        xPosition = originalX + direction * jumpDistance * jumpProgress - direction * jumpDistance * (2.0f * jumpProgress - 1.0f);
    }

    glPushMatrix();
    glTranslatef(xPosition, yPosition, originalZ);

    // If Quadrimon is jumping towards the enemy
    if (currentTime < jumpDuration / 2.0f) {
        // Scale Quadrimon to simulate jumping forward
        float scaleFactor = 1.0f + jumpProgress * 0.5; // Increase scale as it jumps forward
        glScalef(scaleFactor, scaleFactor, scaleFactor);
    } else {
        // Scale Quadrimon to simulate jumping back to original size
        float scaleFactor = 1.5f - jumpProgress * 0.5f; // Decrease scale as it jumps back
        glScalef(scaleFactor, scaleFactor, scaleFactor);
    }

    drawQuadrimon(id); // Draw the Quadrimon at the specified position
    glPopMatrix();

    // Increment animation time for the next frame
    animationTime += animationSpeed;
}

void MyGLWidget::damageAnimation(float originalX, float originalY, float originalZ, int id, int direction) {
    // Calculate animation parameters
    float stepBackDistance = 2.0f; // Adjust as needed
    float rotationAngle = 45.0f; // Adjust as needed
    float damageDuration = 10.0f; // Adjust as needed
    float currentTime = fmod(animationTime, damageDuration); // Calculate current time within the damage animation
    float damageProgress = currentTime / damageDuration; // Calculate damage progress from 0 to 1

    glPushMatrix();

    // If Quadrimon is struck by the opponent
    if (currentTime < 2 *damageDuration / 4) {
        glTranslatef(originalX, originalY, originalZ);
    }
    if (currentTime > 2 *damageDuration / 4 && currentTime < 3 *damageDuration / 4) {
        // Step back animation
        float xPosition = originalX -  direction * stepBackDistance * 2 * (damageProgress - 0.5f); // Move back gradually
        float yRotation = direction * rotationAngle * 2 * (damageProgress - 0.5f); // Rotate gradually by 45 degrees

        glTranslatef(xPosition, originalY, originalZ);
        glRotatef(yRotation, 0.0f, 0.0f, 1.0f); // Rotate around X-axis
    }
    if (currentTime > 3 * damageDuration / 4 && currentTime < damageDuration) {
        float xPosition = originalX - direction * stepBackDistance * (1 - 2 * (damageProgress - 0.5f)); // Move back gradually
        float yRotation = direction * rotationAngle * (1 - 2 * (damageProgress - 0.5f)); // Rotate gradually by 45 degrees

        glTranslatef(xPosition, originalY, originalZ);
        glRotatef(yRotation, 0.0f, 0.0f, 1.0f); // Rotate around X-axis
    }

    drawQuadrimon(id); // Draw the Quadrimon at the specified position

    glPopMatrix();

    // Increment animation time for the next frame
    animationTime += animationSpeed;
}

void MyGLWidget::deathAnimation(float originalX, float originalY, float originalZ, int id) {
    // Calculate animation parameters
    float scaleDownDuration = 10.0f; // Adjust as needed
    float spinSpeed = 10.0f; // Adjust as needed
    float currentTime = fmod(animationTime, scaleDownDuration); // Calculate current time within the scale down duration
    float scaleProgress = currentTime / scaleDownDuration; // Calculate scale down progress from 0 to 1
    float rotationAngle = spinSpeed * currentTime; // Calculate rotation angle based on spin speed

    glPushMatrix();

    // If Quadrimon is in the process of dying
    if (currentTime < scaleDownDuration) {
        // Scale down animation
        float scaleFactor = 1.0f - scaleProgress; // Decrease scale gradually from 100% to 0%
        glTranslatef(originalX, originalY, originalZ);
        glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f); // Rotate around Z-axis
        glScalef(scaleFactor, scaleFactor, scaleFactor); // Scale down gradually
    }

    drawQuadrimon(id); // Draw the Quadrimon at the specified position

    glPopMatrix();

    // Increment animation time for the next frame
    animationTime += animationSpeed;
}

void MyGLWidget::winningAnimation(float originalX, float originalY, float originalZ, int id, int direction) {
    // Calculate animation parameters
    float jumpHeight = 2.0f; // Adjust as needed
    float jumpDuration = 10.0f; // Adjust as needed
    float currentTime = fmod(animationTime, jumpDuration); // Calculate current time within one jump cycle
    float jumpProgress = currentTime / jumpDuration; // Calculate jump progress from 0 to 1

    // Calculate vertical position based on jump progress
    float yPosition = originalY + jumpHeight * (4 * jumpProgress - 4 * jumpProgress * jumpProgress);

    // Keep horizontal position constant
    float xPosition = originalX;

    glPushMatrix();
    glTranslatef(xPosition, yPosition, originalZ);

    // If Quadrimon is jumping, adjust scale
    float scaleFactor;
    if (currentTime < jumpDuration / 2.0f) {
        // Scale Quadrimon to simulate jumping forward
        scaleFactor = 1.0f + jumpProgress * 0.5f; // Increase scale as it jumps up
    } else {
        // Scale Quadrimon to simulate returning to original size
        scaleFactor = 1.5f - jumpProgress * 0.5f; // Decrease scale as it comes down
    }
    glScalef(scaleFactor, scaleFactor, scaleFactor);

    drawQuadrimon(id); // Draw the Quadrimon at the specified position
    glPopMatrix();

    // Increment animation time for the next frame
    animationTime += animationSpeed;
}

void MyGLWidget::drawTerrainGauche(int terrainIndex) {
    drawTerrainGaucheBackground(terrainIndex);
    drawTerrainGaucheForeground(terrainIndex);
}

void MyGLWidget::drawTerrainGaucheForeground(int terrainIndex) {
    // Charger la texture
    QString imagePathToCard = QString("../Textures/texture%1.jpeg").arg(terrainIndex);
    QImage textureImage(imagePathToCard);
    if (textureImage.isNull()) {
        qDebug() << "Impossible de charger l'image de texture.";
        return;
    }
    textureImage = textureImage.rgbSwapped();

    // Désactiver l'éclairage pour le terrain
    glDisable(GL_LIGHTING);

    // Activer le mode de texture OpenGL
    glEnable(GL_TEXTURE_2D);

    // Générer et lier la texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Paramètres de texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Charger les données de texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureImage.width(), textureImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage.bits());

    // Définir les coordonnées de texture pour chaque sommet
    glBegin(GL_QUADS);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-5.0f, 0.0f, 5.0f);  // Bas gauche
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 5.0f);    // Bas droit
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, -5.0f);    // Haut droit
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-5.0f, 0.0f, -5.0f);   // Haut gauche
    glEnd();

    // Désactiver le mode de texture OpenGL
    glDisable(GL_TEXTURE_2D);

    // Réactiver l'éclairage
    glEnable(GL_LIGHTING);
}

void MyGLWidget::drawTerrainGaucheBackground(int terrainIndex) {
    // Charger la texture
    QString imagePathToCard = QString("../Textures/backgroundtexture%1.jpeg").arg(terrainIndex);
    QImage textureImage(imagePathToCard);
    if (textureImage.isNull()) {
        qDebug() << "Impossible de charger l'image de texture.";
        return;
    }
    textureImage = textureImage.rgbSwapped();

    // Désactiver l'éclairage pour le terrain
    glDisable(GL_LIGHTING);

    // Activer le mode de texture OpenGL
    glEnable(GL_TEXTURE_2D);

    // Générer et lier la texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Paramètres de texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Charger les données de texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureImage.width(), textureImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage.bits());

    // Définir les coordonnées de texture pour chaque sommet
    glBegin(GL_QUADS);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-5.0f, 0.0f, -5.0f);  // Bas gauche
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, -5.0f);    // Bas droit
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 5.0f, -5.0f);    // Haut droit
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-5.0f, 5.0f, -5.0f);   // Haut gauche
    glEnd();

    // Désactiver le mode de texture OpenGL
    glDisable(GL_TEXTURE_2D);

    // Réactiver l'éclairage
    glEnable(GL_LIGHTING);
}

void MyGLWidget::drawTerrainDroite(int terrainIndex) {
    drawTerrainDroiteBackground(terrainIndex);
    drawTerrainDroiteForeground(terrainIndex);
}

void MyGLWidget::drawTerrainDroiteForeground(int terrainIndex) {
    // Charger la texture
    QString imagePathToCard = QString("../Textures/texture%1.jpeg").arg(terrainIndex);
    QImage textureImage(imagePathToCard);
    if (textureImage.isNull()) {
        qDebug() << "Impossible de charger l'image de texture.";
        return;
    }
    textureImage = textureImage.rgbSwapped();

    // Désactiver l'éclairage pour le terrain
    glDisable(GL_LIGHTING);

    // Activer le mode de texture OpenGL
    glEnable(GL_TEXTURE_2D);

    // Générer et lier la texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Paramètres de texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Charger les données de texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureImage.width(), textureImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage.bits());

    // Définir les coordonnées de texture pour chaque sommet
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 5.0f);  // Bas gauche
    glTexCoord2f(1.0f, 0.0f); glVertex3f(5.0f, 0.0f, 5.0f);    // Bas droit
    glTexCoord2f(1.0f, 1.0f); glVertex3f(5.0f, 0.0f, -5.0f);    // Haut droit
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, -5.0f);   // Haut gauche
    glEnd();

    // Désactiver le mode de texture OpenGL
    glDisable(GL_TEXTURE_2D);

    // Réactiver l'éclairage
    glEnable(GL_LIGHTING);
}

void MyGLWidget::drawTerrainDroiteBackground(int terrainIndex) {
    // Charger la texture
    QString imagePathToCard = QString("../Textures/backgroundtexture%1.jpeg").arg(terrainIndex);
    QImage textureImage(imagePathToCard);
    if (textureImage.isNull()) {
        qDebug() << "Impossible de charger l'image de texture.";
        return;
    }
    textureImage = textureImage.rgbSwapped();

    // Désactiver l'éclairage pour le terrain
    glDisable(GL_LIGHTING);

    // Activer le mode de texture OpenGL
    glEnable(GL_TEXTURE_2D);

    // Générer et lier la texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Paramètres de texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Charger les données de texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureImage.width(), textureImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage.bits());

    // Définir les coordonnées de texture pour chaque sommet
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, -5.0f);  // Bas gauche
    glTexCoord2f(1.0f, 0.0f); glVertex3f(5.0f, 0.0f, -5.0f);    // Bas droit
    glTexCoord2f(1.0f, 1.0f); glVertex3f(5.0f, 5.0f, -5.0f);    // Haut droit
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 5.0f, -5.0f);   // Haut gauche
    glEnd();

    // Désactiver le mode de texture OpenGL
    glDisable(GL_TEXTURE_2D);

    // Réactiver l'éclairage
    glEnable(GL_LIGHTING);
}

void MyGLWidget::drawQuadrimon(int id) {
    switch (id) {
    case 1:
        drawCylindrus();
        break;
    case 2:
        drawGlace();
        break;
    case 3:
        drawOlaf();
        break;
    case 4:
        drawSoleil();
        break;
    case 5:
        drawPyramide();
        break;
    case 6:
        drawKhone();
        break;
    case 7:
        drawFlamby();
        break;
    case 8:
        drawDonut();
        break;
    case 9:
        drawSaladier();
        break;
    default:
        break;
    }
}

void MyGLWidget::drawCylindrus() {
    // Charger la texture
    QString imagePathToCard("../Textures/water.jpeg");
    QImage textureImage(imagePathToCard);
    if (textureImage.isNull()) {
        qDebug() << "Impossible de charger l'image de texture.";
        return;
    }
    // Inversion des couleurs de l'image
    textureImage = textureImage.rgbSwapped();

    // Activer le mode de texture OpenGL
    glEnable(GL_TEXTURE_2D);

    // Générer et lier la texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Spécifier le format de la texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureImage.width(), textureImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage.bits());

    // Paramètres de filtrage de la texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Cylindrus le texturé
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureID); // Lier la texture
    glEnable(GL_TEXTURE_2D); // Activer le mode de mappage de texture

    glColor3f(1.0f, 1.0f, 1.0f); // Réinitialiser la couleur à blanc

    GLfloat materialDiffuseColorCylindrus[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Utiliser la couleur blanche pour la texture
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuseColorCylindrus);

    GLUquadric* quadricCyl = gluNewQuadric();
    gluQuadricDrawStyle(quadricCyl, GLU_FILL);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

    // Définir les coordonnées de texture pour chaque sommet du cylindre
    gluQuadricTexture(quadricCyl, GL_TRUE);

    gluDisk(quadricCyl, 0, 1, 20, 1);
    gluCylinder(quadricCyl, 1, 1, 2, 20, 20);
    glTranslatef(0.0f, 0.0f, 2.0f);
    gluDisk(quadricCyl, 0, 1, 20, 1);

    gluDeleteQuadric(quadricCyl);
    glPopMatrix();
}

void MyGLWidget::drawGlace() {
    // Charger la texture
    QString imagePathToTexture("../Textures/water.jpeg");
    QImage textureImage(imagePathToTexture);
    if (textureImage.isNull()) {
        qDebug() << "Impossible de charger l'image de texture.";
        return;
    }
    // Inversion des couleurs de l'image
    textureImage = textureImage.rgbSwapped();

    // Activer le mode de texture OpenGL
    glEnable(GL_TEXTURE_2D);

    // Générer et lier la texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Spécifier le format de la texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureImage.width(), textureImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage.bits());

    // Paramètres de filtrage de la texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Glace à la fraise
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureID); // Lier la texture
    glEnable(GL_TEXTURE_2D); // Activer le mode de mappage de texture

    glColor3f(1.0f, 1.0f, 1.0f); // Réinitialiser la couleur à blanc

    GLfloat materialDiffuseColorGlace[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Utiliser la couleur blanche pour la texture
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuseColorGlace);

    GLUquadric* quadricGlace = gluNewQuadric();
    gluQuadricDrawStyle(quadricGlace, GLU_FILL);
    glTranslatef(0.0f, 2.0f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

    // Définir les coordonnées de texture pour chaque sommet de la sphère
    gluQuadricTexture(quadricGlace, GL_TRUE);

    gluCylinder(quadricGlace, 0.5, 0, 2, 20, 20);
    gluSphere(quadricGlace, 0.5, 20, 20);

    gluDeleteQuadric(quadricGlace);
    glPopMatrix();
}

void MyGLWidget::drawOlaf() {
    // Charger la texture
    QString imagePathToTexture("../Textures/water.jpeg");
    QImage textureImage(imagePathToTexture);
    if (textureImage.isNull()) {
        qDebug() << "Impossible de charger l'image de texture.";
        return;
    }
    // Inversion des couleurs de l'image
    textureImage = textureImage.rgbSwapped();

    // Activer le mode de texture OpenGL
    glEnable(GL_TEXTURE_2D);

    // Générer et lier la texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Spécifier le format de la texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureImage.width(), textureImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage.bits());

    // Paramètres de filtrage de la texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Olaf
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureID); // Lier la texture
    glEnable(GL_TEXTURE_2D); // Activer le mode de mappage de texture

    glColor3f(1.0f, 1.0f, 1.0f); // Réinitialiser la couleur à blanc

    GLfloat materialDiffuseColorOlaf[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Utiliser la couleur blanche pour la texture
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuseColorOlaf);

    GLUquadric* quadricOlaf = gluNewQuadric();
    gluQuadricDrawStyle(quadricOlaf, GLU_FILL);
    glTranslatef(0.0f, 0.5f, 0.0f);
    gluQuadricTexture(quadricOlaf, GL_TRUE); // Activer la génération des coordonnées de texture
    gluSphere(quadricOlaf, 0.5, 20, 20);
    glTranslatef(0.0f, 0.9f, 0.0f);
    gluSphere(quadricOlaf, 0.4, 20, 20);
    glTranslatef(0.0f, 0.7f, 0.0f);
    gluSphere(quadricOlaf, 0.3, 20, 20);

    gluDeleteQuadric(quadricOlaf);
    glPopMatrix();
}

void MyGLWidget::drawSoleil() {
    // Charger la texture
    QString imagePathToTexture("../Textures/fire.jpeg");
    QImage textureImage(imagePathToTexture);
    if (textureImage.isNull()) {
        qDebug() << "Impossible de charger l'image de texture.";
        return;
    }
    // Inversion des couleurs de l'image
    textureImage = textureImage.rgbSwapped();

    // Activer le mode de texture OpenGL
    glEnable(GL_TEXTURE_2D);

    // Générer et lier la texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Spécifier le format de la texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureImage.width(), textureImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage.bits());

    // Paramètres de filtrage de la texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Soleil
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureID); // Lier la texture
    glEnable(GL_TEXTURE_2D); // Activer le mode de mappage de texture

    glColor3f(1.0f, 1.0f, 1.0f); // Réinitialiser la couleur à blanc

    GLfloat materialDiffuseColorSoleil[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Utiliser la couleur blanche pour la texture
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuseColorSoleil);

    GLUquadric* quadricSoleil = gluNewQuadric();
    gluQuadricDrawStyle(quadricSoleil, GLU_FILL);
    glTranslatef(0.0f, 1.0f, 0.0f);
    gluQuadricTexture(quadricSoleil, GL_TRUE); // Activer la génération des coordonnées de texture
    gluSphere(quadricSoleil, 1, 20, 20);

    gluDeleteQuadric(quadricSoleil);
    glPopMatrix();
}

void MyGLWidget::drawPyramide() {
    // Charger la texture pour la pyramide
    QString imagePathToPyramidTexture("../Textures/fire.jpeg");
    QImage pyramidTextureImage(imagePathToPyramidTexture);
    if (pyramidTextureImage.isNull()) {
        qDebug() << "Impossible de charger l'image de texture pour la pyramide.";
        return;
    }
    pyramidTextureImage = pyramidTextureImage.rgbSwapped();

    // Activer le mode de texture OpenGL
    glEnable(GL_TEXTURE_2D);

    // Générer et lier la texture pour la pyramide
    GLuint pyramidTextureID;
    glGenTextures(1, &pyramidTextureID);
    glBindTexture(GL_TEXTURE_2D, pyramidTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pyramidTextureImage.width(), pyramidTextureImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pyramidTextureImage.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Méca-Pyramide Gizeh
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, pyramidTextureID); // Lier la texture
    glEnable(GL_TEXTURE_2D); // Activer le mode de mappage de texture

    glColor3f(1.0f, 1.0f, 1.0f); // Réinitialiser la couleur à blanc

    GLfloat materialDiffuseColorPyra[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Utiliser la couleur blanche pour la texture
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuseColorPyra);

    GLUquadric* quadricPyra = gluNewQuadric();
    gluQuadricDrawStyle(quadricPyra, GLU_FILL);

    // Enable texture coordinates for the quadric
    gluQuadricTexture(quadricPyra, GL_TRUE);

    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluDisk(quadricPyra, 0, 1, 4, 1);
    gluCylinder(quadricPyra, 1, 0, 2, 4, 20);

    gluDeleteQuadric(quadricPyra);
    glPopMatrix();

    // Disable texture mapping and delete the texture
    glDisable(GL_TEXTURE_2D);
    glDeleteTextures(1, &pyramidTextureID);
}

void MyGLWidget::drawKhone() {
    // Charger la texture
    QString imagePathToTexture("../Textures/fire.jpeg");
    QImage textureImage(imagePathToTexture);
    if (textureImage.isNull()) {
        qDebug() << "Impossible de charger l'image de texture.";
        return;
    }
    // Inversion des couleurs de l'image
    textureImage = textureImage.rgbSwapped();

    // Activer le mode de texture OpenGL
    glEnable(GL_TEXTURE_2D);

    // Générer et lier la texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Spécifier le format de la texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureImage.width(), textureImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage.bits());

    // Paramètres de filtrage de la texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Khone
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureID); // Lier la texture
    glEnable(GL_TEXTURE_2D); // Activer le mode de mappage de texture

    glColor3f(1.0f, 1.0f, 1.0f); // Réinitialiser la couleur à blanc

    GLfloat materialDiffuseColorKhone[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Utiliser la couleur blanche pour la texture
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuseColorKhone);

    GLUquadric* quadricKhone = gluNewQuadric();
    gluQuadricDrawStyle(quadricKhone, GLU_FILL);

    // Enable texture coordinates for the quadric
    gluQuadricTexture(quadricKhone, GL_TRUE);

    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluDisk(quadricKhone, 0, 0.5, 20, 1);
    gluCylinder(quadricKhone, 0.5, 0, 2, 20, 20);

    gluDeleteQuadric(quadricKhone);
    glPopMatrix();

    // Disable texture mapping and delete the texture
    glDisable(GL_TEXTURE_2D);
    glDeleteTextures(1, &textureID);
}

void MyGLWidget::drawFlamby() {
    // Charger la texture pour Flamby
    QString imagePathToFlambyTexture("../Textures/plant.jpeg");
    QImage flambyTextureImage(imagePathToFlambyTexture);
    if (flambyTextureImage.isNull()) {
        qDebug() << "Impossible de charger l'image de texture pour Flamby.";
        return;
    }
    flambyTextureImage = flambyTextureImage.rgbSwapped();

    // Activer le mode de texture OpenGL
    glEnable(GL_TEXTURE_2D);

    // Générer et lier la texture pour Flamby
    GLuint flambyTextureID;
    glGenTextures(1, &flambyTextureID);
    glBindTexture(GL_TEXTURE_2D, flambyTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, flambyTextureImage.width(), flambyTextureImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, flambyTextureImage.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Flamby
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, flambyTextureID); // Lier la texture
    glEnable(GL_TEXTURE_2D); // Activer le mode de mappage de texture

    glColor3f(1.0f, 1.0f, 1.0f); // Réinitialiser la couleur à blanc

    GLfloat materialDiffuseColorFlamby[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Utiliser la couleur blanche pour la texture
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuseColorFlamby);

    glTranslatef(0.0f, 1.1f, 0.0f); // Adjust Y position to be above the terrain

    glBegin(GL_QUADS);
    // Front face
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);

    // Back face
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);

    // Left face
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);

    // Right face
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);

    // Top face
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);

    // Bottom face
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);

    glEnd();

    glPopMatrix();

    // Désactiver le mode de texture OpenGL
    glDisable(GL_TEXTURE_2D);
    glDeleteTextures(1, &flambyTextureID);
}

void MyGLWidget::drawDonut() {
    // Charger la texture pour le donut
    QString imagePathToDonutTexture("../Textures/plant.jpeg");
    QImage donutTextureImage(imagePathToDonutTexture);
    if (donutTextureImage.isNull()) {
        qDebug() << "Impossible de charger l'image de texture pour le donut.";
        return;
    }
    donutTextureImage = donutTextureImage.rgbSwapped();

    // Activer le mode de texture OpenGL
    glEnable(GL_TEXTURE_2D);

    // Générer et lier la texture pour le donut
    GLuint donutTextureID;
    glGenTextures(1, &donutTextureID);
    glBindTexture(GL_TEXTURE_2D, donutTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, donutTextureImage.width(), donutTextureImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, donutTextureImage.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Menu Donut Vegan
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, donutTextureID); // Lier la texture
    glEnable(GL_TEXTURE_2D); // Activer le mode de mappage de texture

    glColor3f(1.0f, 1.0f, 1.0f); // Réinitialiser la couleur à blanc

    GLfloat materialDiffuseColorDonut[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Utiliser la couleur blanche pour la texture
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuseColorDonut);

    const float innerRadius = 0.5f;
    const float outerRadius = 1.0f;
    const int sides = 50;
    const int rings = 50;
    glTranslatef(0.0f, 1.0f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    for (int i = 0; i < sides; ++i) {
        const float theta = 2.0f * M_PI * i / sides;
        const float nextTheta = 2.0f * M_PI * (i + 1) / sides;

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= rings; ++j) {
            const float phi = 2.0f * M_PI * j / rings;

            const float x = (outerRadius + innerRadius * cos(phi)) * cos(theta);
            const float y = (outerRadius + innerRadius * cos(phi)) * sin(theta);
            const float z = innerRadius * sin(phi);

            glTexCoord2f((float)i / sides, (float)j / rings);
            glVertex3f(x, y, z);

            const float nextX = (outerRadius + innerRadius * cos(phi)) * cos(nextTheta);
            const float nextY = (outerRadius + innerRadius * cos(phi)) * sin(nextTheta);

            glTexCoord2f((float)(i + 1) / sides, (float)j / rings);
            glVertex3f(nextX, nextY, z);
        }
        glEnd();
    }
    glPopMatrix();
}

void MyGLWidget::drawSaladier() {
    // Charger la texture pour le saladier
    QString imagePathToSaladTexture("../Textures/plant.jpeg");
    QImage saladTextureImage(imagePathToSaladTexture);
    if (saladTextureImage.isNull()) {
        qDebug() << "Impossible de charger l'image de texture pour le saladier.";
        return;
    }
    saladTextureImage = saladTextureImage.rgbSwapped();

    // Activer le mode de texture OpenGL
    glEnable(GL_TEXTURE_2D);

    // Générer et lier la texture pour le saladier
    GLuint saladTextureID;
    glGenTextures(1, &saladTextureID);
    glBindTexture(GL_TEXTURE_2D, saladTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, saladTextureImage.width(), saladTextureImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, saladTextureImage.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Saladier mystique
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, saladTextureID); // Lier la texture
    glEnable(GL_TEXTURE_2D); // Activer le mode de mappage de texture

    glColor3f(1.0f, 1.0f, 1.0f); // Réinitialiser la couleur à blanc

    GLfloat materialDiffuseColorSaladier[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Utiliser la couleur blanche pour la texture
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuseColorSaladier);

    glTranslatef(2.0f, 0.0f, 0.0f);

    int scaley = 40;
    int scalex = 40;
    GLfloat r = 2;
    int i, j;
    GLfloat v[scalex * scaley][3];
    GLfloat texCoords[scalex * scaley][2];

    // Adjust the range to exclude the top part
    int start_i = 0;
    int end_i = scalex; // For example, exclude the top 10% (4 layers out of 40)

    for (i = start_i; i < end_i; ++i) {
        for (j = 0; j < scaley; ++j) {
            v[i * scaley + j][0] = r * cos(j * 2 * M_PI / scaley) * cos(i * M_PI / (2 * scalex)) - 2;
            v[i * scaley + j][1] = -r * sin(i * M_PI / (2 * scalex)) + 2;
            v[i * scaley + j][2] = r * sin(j * 2 * M_PI / scaley) * cos(i * M_PI / (2 * scalex));

            texCoords[i * scaley + j][0] = static_cast<float>(j) / scaley;
            texCoords[i * scaley + j][1] = static_cast<float>(i) / scalex;
        }
    }

    glBegin(GL_QUADS);
    for (i = start_i; i < end_i - 1; ++i) {
        for (j = 0; j < scaley; ++j) {
            glTexCoord2fv(texCoords[i * scaley + j]);
            glVertex3fv(v[i * scaley + j]);

            glTexCoord2fv(texCoords[i * scaley + (j + 1) % scaley]);
            glVertex3fv(v[i * scaley + (j + 1) % scaley]);

            glTexCoord2fv(texCoords[(i + 1) * scaley + (j + 1) % scaley]);
            glVertex3fv(v[(i + 1) * scaley + (j + 1) % scaley]);

            glTexCoord2fv(texCoords[(i + 1) * scaley + j]);
            glVertex3fv(v[(i + 1) * scaley + j]);
        }
    }
    glEnd();

    glPopMatrix();

    // Disable texture mapping and delete the texture
    glDisable(GL_TEXTURE_2D);
    glDeleteTextures(1, &saladTextureID);
}
