#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "QtWidgets/qlabel.h"
#include "combat.h"
#include "myglwidget.h"
#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QTimer>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
//Réalisée par Yanis MEDIENE
//Cette classe gère la fenêtre principale et le déroulement d'une partie.
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);//Constructeur
    ~MainWindow();//Destructeur


private slots:
    //void on_captureButton_clicked();
    void captureButtonPushed();//Evenement de click du bouton capture
    //void on_validateButton_clicked();
    void validateButtonPushed();//Evenement de click du bouton validate
    //void on_endTurnButton_clicked();
    void endTurnButtonPushed();//Evenement de click du bouton skip
    void updateFrame();//Met à jour la frame de la sortie de la caméra
    void updateArena();//Met à jour l'arène

protected:
    void keyPressEvent(QKeyEvent *event) override;//Pour créer des raccourcis clavier comme alternatives des boutons

private:
    Ui::MainWindow *ui;//Interface de la fen^tre principale
    cv::VideoCapture cap;//C'est la "caméra"
    QTimer timer;//Un timer pour rafraîchir la sortie de la caméra en parallèle du jeu
    bool captureEnabled;//Pour indiquer que prendre une photo est autorisée
    Combat battle;//La gestion des joueurs et du tour
    int best_index = -1;//Meiilleur index pour la reconnaissance d'une carte
    int phaseCounter = 1;//Phase dut tour à laquelle on se trouve

    void startVideoCapture();//Lance la caméra
    void stopVideoCapture();//Arrête la caméra
    void updateWebcamLabel(const cv::Mat& frame);//Update la sortie de la caméra
    int getMatchedCard(cv::VideoCapture& cap);//détermine la carte qui a été photographiée
    void displayCardImages(int best_match_index, QLabel* cardLabel, QLabel* cardInfoLabel, MyGLWidget* openGLWidget);//affiche à l'écran la carte qui a été reconnue
    void displayQuadrimonAndTerrain(MyGLWidget* openGLWidget);//Affiche l'arène complète avec les quadrimons et les terrains
    void pauseExecution(int milliseconds);//Pour poser l'éxécution

};

#endif // MAINWINDOW_H
