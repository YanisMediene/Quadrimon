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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    //void on_captureButton_clicked();
    void captureButtonPushed();
    //void on_validateButton_clicked();
    void validateButtonPushed();
    //void on_endTurnButton_clicked();
    void endTurnButtonPushed();
    void updateFrame();
    void updateArena();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::MainWindow *ui;
    cv::VideoCapture cap;
    QTimer timer;
    bool captureEnabled;
    Combat battle;
    int best_index = -1;
    int phaseCounter = 1;

    void startVideoCapture();
    void stopVideoCapture();
    void updateWebcamLabel(const cv::Mat& frame);
    int getMatchedCard(cv::VideoCapture& cap);
    void displayCardImages(int best_match_index, QLabel* cardLabel, QLabel* cardInfoLabel, MyGLWidget* openGLWidget);
    void displayQuadrimonAndTerrain(MyGLWidget* openGLWidget);
    void pauseExecution(int milliseconds);

};

#endif // MAINWINDOW_H
