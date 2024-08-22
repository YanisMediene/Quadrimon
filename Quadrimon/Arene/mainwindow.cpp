#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <chrono>
#include <thread>

using namespace cv;
using std::cout;
using std::endl;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    captureEnabled(false),
    battle()
{
    ui->setupUi(this);

    connect(ui->captureButton, &QPushButton::clicked, this, &MainWindow::captureButtonPushed);
    connect(ui->validateButton, &QPushButton::clicked, this, &MainWindow::validateButtonPushed);
    connect(ui->endTurnButton, &QPushButton::clicked, this, &MainWindow::endTurnButtonPushed);

    // Démarrer la capture vidéo dès l'ouverture de la fenêtre principale
    startVideoCapture();

    ui->P1->setText("Joueur 1 choix Q1");

    // exemple d'initialisation

    Quadrimon j1q1(0);
    Quadrimon j1q2(0);
    Quadrimon j2q1(0);
    Quadrimon j2q2(0);

    CapsuleTerrain c1(0);
    CapsuleTerrain c2(0);

    Terrain t1(j1q1);
    t1.setCapsuleTerrain(c1);
    Terrain t2(j2q1);
    t2.setCapsuleTerrain(c2);

    Joueur j1(j1q1,j1q2,t1);
    Joueur j2(j2q1,j2q2,t2);

    battle = Combat(j1,j2);

    displayQuadrimonAndTerrain(ui->arenaOpenGLWidget);

    ui->turnInfo->setText("Tour : 1");
    //phaseCounter = 7;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startVideoCapture()
{
    cap.open(0); // Ouvre la capture vidéo de la webcam
    if (!cap.isOpened()) {
        // Gérer l'erreur
        return;
    }

    // Utiliser un minuteur pour récupérer les images de la webcam en continu
    connect(&timer, &QTimer::timeout, this, &MainWindow::updateFrame);
    connect(&timer, &QTimer::timeout, this, &MainWindow::updateArena);

    timer.start(30); // Mettre à jour toutes les 30 ms (environ 30 FPS)
}

void MainWindow::stopVideoCapture()
{
    timer.stop(); // Arrêter le minuteur
    cap.release(); // Libérer les ressources de capture vidéo
}

void MainWindow::updateFrame()
{
    cv::Mat frame;
    cap >> frame; // Capturer un frame de la webcam

    if (!frame.empty()) {
        // Mettre à jour le QLabel avec le frame capturé
        updateWebcamLabel(frame);
    }

}

void MainWindow::updateArena()
{
    displayQuadrimonAndTerrain(ui->arenaOpenGLWidget);
}

void MainWindow::updateWebcamLabel(const cv::Mat& frame)
{
    // Convertir le frame OpenCV en QImage
    QImage img = QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888).rgbSwapped();

    // Mettre à jour la taille de l'image pour s'adapter au QLabel tout en conservant le rapport d'aspect
    img = img.scaled(ui->webcam->size(), Qt::KeepAspectRatio);

    // Définir le QImage en tant que pixmap du QLabel
    ui->webcam->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::captureButtonPushed()
{
    best_index = getMatchedCard(cap);;

    if (best_index != -1) {
        displayCardImages(best_index, ui->cardImage, ui->cardInfo, ui->arenaOpenGLWidget);
    } else {
        displayCardImages(0, ui->cardImage, ui->cardInfo, ui->arenaOpenGLWidget);
    }
}

int MainWindow::getMatchedCard(cv::VideoCapture& cap) {
    //-- Step 1: Load the template images (cards)
    cv::Mat reference_images[18];
    for (int i = 0; i < 18; ++i) {
        std::string filename = "/Users/mediene/Informatique/BiblioDevMulti/Projet_Quadrimon/Cards/card" + std::to_string(i + 1) + ".png";
        reference_images[i] = cv::imread(filename, cv::IMREAD_GRAYSCALE);
        if (reference_images[i].empty()) {
            cout << "Error: Failed to load reference image: " << filename << endl;
        }
    }

    cv::Ptr<cv::SIFT> detector = cv::SIFT::create();
    std::vector<std::vector<cv::KeyPoint>> keypoints_reference;
    std::vector<cv::Mat> descriptors_reference;
    for (int i = 0; i < 18; ++i) {
        std::vector<cv::KeyPoint> keypoints;
        cv::Mat descriptors;
        detector->detectAndCompute(reference_images[i], cv::noArray(), keypoints, descriptors);
        keypoints_reference.push_back(keypoints);
        descriptors_reference.push_back(descriptors);
    }

    //-- Step 2: Camera capture
    cv::Mat frame;
    std::vector<cv::KeyPoint> keypoints_frame;
    cv::Mat descriptors_frame;
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);

    cap >> frame;
    if (frame.empty())
    {
        cout << "Error: Failed to capture frame" << endl;
        return -1;
    }

    //-- Step 3: Detect keypoints and compute descriptors for the current frame
    detector->detectAndCompute(frame, cv::noArray(), keypoints_frame, descriptors_frame);

    //-- Step 4: Match descriptors between the current frame and template images
    //-- Filter matches using the Lowe's ratio test
    int max_matches = 20;
    int best_match_index = -1;
    for (int i = 0; i < 18; ++i) {
        std::vector< std::vector<cv::DMatch> > knn_matches;
        matcher->knnMatch(descriptors_frame, descriptors_reference[i], knn_matches, 2);

        const float ratio_thresh = 0.8f;
        std::vector<cv::DMatch> good_matches;
        for (size_t j = 0; j < knn_matches.size(); ++j)
        {
            if (knn_matches[j][0].distance < ratio_thresh * knn_matches[j][1].distance)
            {
                good_matches.push_back(knn_matches[j][0]);
            }
        }

        if (good_matches.size() > max_matches) {
            max_matches = good_matches.size();
            best_match_index = i+1;
        }

    }
    return best_match_index;
}

void MainWindow::displayCardImages(int best_match_index, QLabel* cardLabel, QLabel* cardInfoLabel, MyGLWidget* openGLWidget) {
    QString imagePathToCard = QString("/Users/mediene/Informatique/BiblioDevMulti/Projet_Quadrimon/Cards/card%1.png").arg(best_match_index);
    QString imagePathToCardInfo = QString("/Users/mediene/Informatique/BiblioDevMulti/Projet_Quadrimon/Cards/infocard%1.png").arg(best_match_index);

    QImage imgCard(imagePathToCard); // Load the image from the file
    QImage imgCardInfo(imagePathToCardInfo); // Load the image from the file

    if (imgCard.isNull() || imgCardInfo.isNull()) {
        qDebug() << "Failed to load image from" << imagePathToCard << "or" << imagePathToCardInfo;
        return; // Exit or handle the error accordingly
    }

    // If you want to resize the images to fit the QLabel while maintaining aspect ratio
    imgCard = imgCard.scaled(cardLabel->size(), Qt::KeepAspectRatio);
    imgCardInfo = imgCardInfo.scaled(cardInfoLabel->size(), Qt::KeepAspectRatio);

    // Set the QImage as the pixmap of the QLabel
    cardLabel->setPixmap(QPixmap::fromImage(imgCard));
    cardInfoLabel->setPixmap(QPixmap::fromImage(imgCardInfo));
}

void MainWindow::validateButtonPushed()
{
    switch (phaseCounter) {
    case 1: // Avant Combat P1Q1
        if (best_index >= 1 && best_index <= 9) {
            battle.getJoueur1().setQuadrimon1(Quadrimon(best_index));
            battle.getJoueur1().getTerrain().setQuadrimon(battle.getJoueur1().getQuadrimon1());
            ui->P1->setText("Joueur 1  a la main : choix Q2");
            phaseCounter++;
        }
        break;
    case 2: // Avant Combat P1Q2
        if (best_index >= 1 && best_index <= 9) {
            battle.getJoueur1().setQuadrimon2(Quadrimon(best_index));
            ui->P1->setText("Joueur 1");
            ui->P2->setText("Joueur 2  a la main : choix Q1");
            phaseCounter++;
        }
        break;
    case 3: // Avant Combat P2Q1
        if (best_index >= 1 && best_index <= 9) {
            battle.getJoueur2().setQuadrimon1(Quadrimon(best_index));
            battle.getJoueur2().getTerrain().setQuadrimon(battle.getJoueur2().getQuadrimon1());
            ui->P2->setText("Joueur 2  a la main : choix Q2");
            phaseCounter++;
        }
        break;
    case 4: // Avant Combat P2Q2
        if (best_index >= 1 && best_index <= 9) {
            battle.getJoueur2().setQuadrimon2(Quadrimon(best_index));

            // Avant Combat Choix Aleatoire du premier joueur
            int randomChoice = rand() % 2;
            battle.setCurrentPlayer(randomChoice);

            phaseCounter = 7; // On skip les effet de terrain et l'anim de mort au debut car il n'y a pas de terrain ni de quadrimon mort au debut

            ui->P2->setText("Joueur 2");

            if (battle.getCurrentPlayer() == 0) {
                ui->P1->setText("Joueur 1");
                ui->P2->setText("Joueur 2 a la main : interuption");
            }
            else {
                ui->P1->setText("Joueur 1 a la main : interuption");
                ui->P2->setText("Joueur 2");
            }
        }
        break;
    case 5: // Phase d'appliquation des effets des terrains
        ui->P1->setText("Joueur 1");
        ui->P2->setText("Joueur 2");
        if (battle.getJoueur1().getTerrain().hasCapsuleTerrain()) {
            if (battle.getJoueur1().getTerrain().getCapsuleTerrain().getEffetInfo() == "+100 PV") {
                battle.getJoueur1().getTerrain().getQuadrimon().setPointsVie(battle.getJoueur1().getTerrain().getQuadrimon().getPointsVie()+100);
                if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon1().getId()) {
                    battle.getJoueur1().getQuadrimon1().setPointsVie(battle.getJoueur1().getQuadrimon1().getPointsVie()+100);
                }
                if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon2().getId()) {
                    battle.getJoueur1().getQuadrimon2().setPointsVie(battle.getJoueur1().getQuadrimon2().getPointsVie()+100);
                }
            }
            if (battle.getJoueur1().getTerrain().getCapsuleTerrain().getEffetInfo() == "-50 PV") {
                battle.getJoueur1().getTerrain().getQuadrimon().setPointsVie(battle.getJoueur1().getTerrain().getQuadrimon().getPointsVie()-50);
                if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon1().getId()) {
                    battle.getJoueur1().getQuadrimon1().setPointsVie(battle.getJoueur1().getQuadrimon1().getPointsVie()-50);
                }
                if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon2().getId()) {
                    battle.getJoueur1().getQuadrimon2().setPointsVie(battle.getJoueur1().getQuadrimon2().getPointsVie()-50);
                }
            }
            if (battle.getJoueur1().getTerrain().getCapsuleTerrain().getEffetInfo() == "-100 ATQ") {
                battle.getJoueur1().getTerrain().getQuadrimon().setPointsAttaque(Quadrimon(battle.getJoueur1().getTerrain().getQuadrimon().getId()).getPointsAttaque()-100);
                if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon1().getId()) {
                    battle.getJoueur1().getQuadrimon1().setPointsVie(Quadrimon(battle.getJoueur1().getQuadrimon1().getId()).getPointsAttaque()-100);
                }
                if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon2().getId()) {
                    battle.getJoueur1().getQuadrimon2().setPointsVie(Quadrimon(battle.getJoueur1().getQuadrimon2().getId()).getPointsAttaque()-100);
                }
            }
            if (battle.getJoueur1().getTerrain().getCapsuleTerrain().getEffetInfo() == "+1000 PV") {
                battle.getJoueur1().getTerrain().getQuadrimon().setPointsVie(battle.getJoueur1().getTerrain().getQuadrimon().getPointsVie()+1000);
                if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon1().getId()) {
                    battle.getJoueur1().getQuadrimon1().setPointsVie(battle.getJoueur1().getQuadrimon1().getPointsVie()+1000);
                }
                if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon2().getId()) {
                    battle.getJoueur1().getQuadrimon2().setPointsVie(battle.getJoueur1().getQuadrimon2().getPointsVie()+1000);
                }
            }

            battle.getJoueur1().getTerrain().getCapsuleTerrain().setTempsActivite(battle.getJoueur1().getTerrain().getCapsuleTerrain().getTempsActivite()-1);
            if (battle.getJoueur1().getTerrain().getCapsuleTerrain().getTempsActivite()<=0) {
                battle.getJoueur1().getTerrain().setCapsuleTerrain(CapsuleTerrain(0));
            }

            if (battle.getJoueur1().getTerrain().getQuadrimon().getPointsVie() <= 0) {
                battle.getJoueur1().getTerrain().getQuadrimon().setPointsVie(0);
                if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon1().getId()) {
                    battle.getJoueur1().getQuadrimon1().setPointsVie(0);
                    ui->arenaOpenGLWidget->indexAnimationGauche = 3;
                    ui->P1->setText("Joueur 1 : Quadrimon Mort");
                }
                if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon2().getId()) {
                    battle.getJoueur1().getQuadrimon2().setPointsVie(0);
                    ui->arenaOpenGLWidget->indexAnimationGauche = 3;
                    ui->P1->setText("Joueur 1 : Quadrimon Mort");
                }
            }
        }
        if (battle.getJoueur2().getTerrain().hasCapsuleTerrain()) {
            if (battle.getJoueur2().getTerrain().getCapsuleTerrain().getEffetInfo() == "+100 PV") {
                battle.getJoueur2().getTerrain().getQuadrimon().setPointsVie(battle.getJoueur2().getTerrain().getQuadrimon().getPointsVie()+100);
                if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon1().getId()) {
                    battle.getJoueur2().getQuadrimon1().setPointsVie(battle.getJoueur2().getQuadrimon1().getPointsVie()+100);
                }
                if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon2().getId()) {
                    battle.getJoueur2().getQuadrimon2().setPointsVie(battle.getJoueur2().getQuadrimon2().getPointsVie()+100);
                }
            }
            if (battle.getJoueur2().getTerrain().getCapsuleTerrain().getEffetInfo() == "-50 PV") {
                battle.getJoueur2().getTerrain().getQuadrimon().setPointsVie(battle.getJoueur2().getTerrain().getQuadrimon().getPointsVie()-50);
                if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon1().getId()) {
                    battle.getJoueur2().getQuadrimon1().setPointsVie(battle.getJoueur2().getQuadrimon1().getPointsVie()-50);
                }
                if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon2().getId()) {
                    battle.getJoueur2().getQuadrimon2().setPointsVie(battle.getJoueur2().getQuadrimon2().getPointsVie()-50);
                }
            }
            if (battle.getJoueur2().getTerrain().getCapsuleTerrain().getEffetInfo() == "-100 ATQ") {
                battle.getJoueur2().getTerrain().getQuadrimon().setPointsAttaque(Quadrimon(battle.getJoueur2().getTerrain().getQuadrimon().getId()).getPointsAttaque()-100);
                if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon1().getId()) {
                    battle.getJoueur2().getQuadrimon1().setPointsVie(Quadrimon(battle.getJoueur2().getQuadrimon1().getId()).getPointsAttaque()-100);
                }
                if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon2().getId()) {
                    battle.getJoueur2().getQuadrimon2().setPointsVie(Quadrimon(battle.getJoueur2().getQuadrimon2().getId()).getPointsAttaque()-100);
                }
            }
            if (battle.getJoueur2().getTerrain().getCapsuleTerrain().getEffetInfo() == "+1000 PV") {
                battle.getJoueur2().getTerrain().getQuadrimon().setPointsVie(battle.getJoueur2().getTerrain().getQuadrimon().getPointsVie()+1000);
                if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon1().getId()) {
                    battle.getJoueur2().getQuadrimon1().setPointsVie(battle.getJoueur2().getQuadrimon1().getPointsVie()+1000);
                }
                if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon2().getId()) {
                    battle.getJoueur2().getQuadrimon2().setPointsVie(battle.getJoueur2().getQuadrimon2().getPointsVie()+1000);
                }
            }

            battle.getJoueur2().getTerrain().getCapsuleTerrain().setTempsActivite(battle.getJoueur2().getTerrain().getCapsuleTerrain().getTempsActivite()-1);
            if (battle.getJoueur2().getTerrain().getCapsuleTerrain().getTempsActivite()<=0) {
                battle.getJoueur2().getTerrain().setCapsuleTerrain(CapsuleTerrain(0));
            }

            if (battle.getJoueur2().getTerrain().getQuadrimon().getPointsVie() <= 0) {
                battle.getJoueur2().getTerrain().getQuadrimon().setPointsVie(0);
                if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon1().getId()) {
                    battle.getJoueur2().getQuadrimon1().setPointsVie(0);
                    ui->arenaOpenGLWidget->indexAnimationDroite = 3;
                    ui->P2->setText("Joueur 2 : Quadrimon Mort");
                }
                if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon2().getId()) {
                    battle.getJoueur2().getQuadrimon2().setPointsVie(0);
                    ui->arenaOpenGLWidget->indexAnimationDroite = 3;
                    ui->P2->setText("Joueur 2 : Quadrimon Mort");
                }
            }
        }      
        if (battle.getJoueur1().getTerrain().getQuadrimon().getPointsVie() > 0 && battle.getJoueur2().getTerrain().getQuadrimon().getPointsVie() > 0) {
            phaseCounter++;// On skip la phase de mort car aucun des deux est mort
            if (battle.getCurrentPlayer() == 0) {
                ui->P1->setText("Joueur 1");
                ui->P2->setText("Joueur 2 : Interuption");
            }
            else {
                ui->P1->setText("Joueur 1 : Interuption");
                ui->P2->setText("Joueur 2");
            }
        }
        phaseCounter++;
        break;
    case 6: // Phase d'animation de mort
        if (battle.getJoueur1().getTerrain().getQuadrimon().getPointsVie() <= 0) {
            if (battle.getJoueur1().getQuadrimon1().getPointsVie() > 0 && battle.getJoueur1().getQuadrimon1().getId() != battle.getJoueur1().getTerrain().getQuadrimon().getId()) {
                battle.getJoueur1().getTerrain().setQuadrimon(battle.getJoueur1().getQuadrimon1());
            }
            if (battle.getJoueur1().getQuadrimon2().getPointsVie() > 0 && battle.getJoueur1().getQuadrimon2().getId() != battle.getJoueur1().getTerrain().getQuadrimon().getId()) {
                battle.getJoueur1().getTerrain().setQuadrimon(battle.getJoueur1().getQuadrimon2());
            }
        }
        if (battle.getJoueur2().getTerrain().getQuadrimon().getPointsVie() <= 0) {
            if (battle.getJoueur2().getQuadrimon1().getPointsVie() > 0 && battle.getJoueur2().getQuadrimon1().getId() != battle.getJoueur2().getTerrain().getQuadrimon().getId()) {
                battle.getJoueur2().getTerrain().setQuadrimon(battle.getJoueur2().getQuadrimon1());
            }
            if (battle.getJoueur2().getQuadrimon2().getPointsVie() > 0 && battle.getJoueur2().getQuadrimon2().getId() != battle.getJoueur2().getTerrain().getQuadrimon().getId()) {
                battle.getJoueur2().getTerrain().setQuadrimon(battle.getJoueur2().getQuadrimon2());
            }
        }
        if (battle.getCurrentPlayer() == 0) {
            ui->P1->setText("Joueur 1");
            ui->P2->setText("Joueur 2 a la main interuption");
        }
        if (battle.getCurrentPlayer() == 1) {
            ui->P1->setText("Joueur 1 a la main interuption");
            ui->P2->setText("Joueur 2");
        }
        ui->arenaOpenGLWidget->indexAnimationGauche = 0;
        ui->arenaOpenGLWidget->indexAnimationDroite = 0;
        phaseCounter++;
        if (battle.getJoueur1().getQuadrimon1().getPointsVie() <= 0 && battle.getJoueur1().getQuadrimon2().getPointsVie() <= 0) {
            ui->P1->setText("Joueur 1 a perdu");
            ui->P2->setText("Joueur 2 a gagner");
            ui->arenaOpenGLWidget->indexAnimationGauche = 3;
            ui->arenaOpenGLWidget->indexAnimationDroite = 4;
            phaseCounter = 16;
        }
        if (battle.getJoueur2().getQuadrimon1().getPointsVie() <= 0 && battle.getJoueur2().getQuadrimon2().getPointsVie() <= 0) {
            ui->P1->setText("Joueur 1 a gagner");
            ui->P2->setText("Joueur 2 a perdu");
            ui->arenaOpenGLWidget->indexAnimationGauche = 4;
            ui->arenaOpenGLWidget->indexAnimationDroite = 3;
            phaseCounter = 16;
        }
        break;
    case 7: // Phase d'interuption
        if (battle.getCurrentPlayer() == 0) {

            if (best_index == -1) {
                phaseCounter++;
                ui->P1->setText("Joueur 1 a la main : prep chgt Q");
                ui->P2->setText("Joueur 2");
            }
            if (best_index >= 10 && best_index <= 18) {
                switch (best_index) {
                case 10: case 12: case 15: case 16: case 17: case 18:
                    if (best_index == 12) {
                        battle.getJoueur2().getTerrain().setCapsuleTerrain(CapsuleTerrain(battle.getJoueur1().getTerrain().getCapsuleTerrain().getId()));
                    }
                    else {
                        battle.getJoueur2().getTerrain().setCapsuleTerrain(CapsuleTerrain(best_index));
                    }
                    phaseCounter++;
                    break;
                case 11: case 13: case 14:
                    battle.getJoueur1().getTerrain().setCapsuleTerrain(CapsuleTerrain(best_index));
                    phaseCounter++;
                    break;
                default:
                    break;
                }
                ui->P1->setText("Joueur 1 a la main : prep chgt Q");
                ui->P2->setText("Joueur 2");
            }
        }
        if (battle.getCurrentPlayer() == 1) {
            if (best_index == -1) {
                phaseCounter++;
                ui->P1->setText("Joueur 1");
                ui->P2->setText("Joueur 2 a la main : prep chgt Q");
            }
            if (best_index >= 10 && best_index <= 18) {                
                switch (best_index) {
                case 10: case 12: case 15: case 16: case 17: case 18:
                    if (best_index == 12) {
                        battle.getJoueur1().getTerrain().setCapsuleTerrain(CapsuleTerrain(battle.getJoueur2().getTerrain().getCapsuleTerrain().getId()));
                    }
                    else {
                        battle.getJoueur1().getTerrain().setCapsuleTerrain(CapsuleTerrain(best_index));
                    }
                    phaseCounter++;
                    break;
                case 11: case 13: case 14:
                    battle.getJoueur2().getTerrain().setCapsuleTerrain(CapsuleTerrain(best_index));
                    phaseCounter++;
                    break;
                default:
                    break;
                }
                ui->P1->setText("Joueur 1");
                ui->P2->setText("Joueur 2 a la main : prep chgt Q");
            }
        }
        break;
    case 8: // Phase de preparation changement de quadrimon
        if (battle.getCurrentPlayer() == 0) {
            if (best_index == -1) {
                phaseCounter++;
                ui->P1->setText("Joueur 1");
                ui->P2->setText("Joueur 2 a la main : interuption");
            }
            if (best_index >= 1 && best_index <= 9) {
                if (best_index != battle.getJoueur1().getTerrain().getQuadrimon().getId()) {
                    if (best_index != battle.getJoueur1().getQuadrimon1().getId() && battle.getJoueur1().getQuadrimon2().getPointsVie() > 0) {
                        battle.getJoueur1().getTerrain().setQuadrimon(battle.getJoueur1().getQuadrimon2());
                        phaseCounter++;
                        ui->P1->setText("Joueur 1");
                        ui->P2->setText("Joueur 2 a la main : interuption");
                    }
                    if (best_index != battle.getJoueur1().getQuadrimon2().getId() && battle.getJoueur1().getQuadrimon1().getPointsVie() > 0) {
                        battle.getJoueur1().getTerrain().setQuadrimon(battle.getJoueur1().getQuadrimon1());
                        phaseCounter++;
                        ui->P1->setText("Joueur 1");
                        ui->P2->setText("Joueur 2 a la main : interuption");
                    }
                }
            }
        }
        else {
            if (best_index == -1) {
                phaseCounter++;
                ui->P1->setText("Joueur 1 a la main : interuption");
                ui->P2->setText("Joueur 2");
            }
            if (best_index >= 1 && best_index <= 9) {
                if (battle.getJoueur2().getTerrain().getCapsuleTerrain().getEffetInfo() == "Bloqué") {
                    phaseCounter++;
                    ui->P1->setText("Joueur 1");
                    ui->P2->setText("Joueur 2 a la main : prep chgt T");
                }
                else {
                    if (best_index != battle.getJoueur2().getTerrain().getQuadrimon().getId() && battle.getJoueur2().getQuadrimon2().getPointsVie() > 0) {
                        if (best_index != battle.getJoueur2().getQuadrimon1().getId()) {
                            battle.getJoueur2().getTerrain().setQuadrimon(battle.getJoueur2().getQuadrimon2());
                            phaseCounter++;
                            ui->P1->setText("Joueur 1 a la main : interuption");
                            ui->P2->setText("Joueur 2");
                        }
                        if (best_index != battle.getJoueur2().getQuadrimon2().getId() && battle.getJoueur2().getQuadrimon1().getPointsVie() > 0) {
                            battle.getJoueur2().getTerrain().setQuadrimon(battle.getJoueur2().getQuadrimon1());
                            phaseCounter++;
                            ui->P1->setText("Joueur 1 a la main : interuption");
                            ui->P2->setText("Joueur 2");
                        }
                    }
                }
            }
        }
        break;
    case 9: // Phase d'interuption
        if (battle.getCurrentPlayer() == 0) {
            if (best_index == -1) {
                phaseCounter++;
                ui->P1->setText("Joueur 1 a la main : prep chgt T");
                ui->P2->setText("Joueur 2");
            }
            if (best_index >= 10 && best_index <= 18) {
                switch (best_index) {
                case 10: case 12: case 15: case 16: case 17: case 18:
                    if (best_index == 12) {
                        battle.getJoueur2().getTerrain().setCapsuleTerrain(CapsuleTerrain(battle.getJoueur1().getTerrain().getCapsuleTerrain().getId()));
                    }
                    else {
                        battle.getJoueur2().getTerrain().setCapsuleTerrain(CapsuleTerrain(best_index));
                    }
                    phaseCounter++;
                    break;
                case 11: case 13: case 14:
                    battle.getJoueur1().getTerrain().setCapsuleTerrain(CapsuleTerrain(best_index));
                    phaseCounter++;
                    break;
                default:
                    break;
                }
                ui->P1->setText("Joueur 1 a la main : prep chgt T");
                ui->P2->setText("Joueur 2");
            }
        }
        if (battle.getCurrentPlayer() == 1) {
            if (best_index == -1) {
                phaseCounter++;
                ui->P1->setText("Joueur 1");
                ui->P2->setText("Joueur 2 a la main : prep chgt T");
            }
            if (best_index >= 10 && best_index <= 18) {
                switch (best_index) {
                case 10: case 12: case 15: case 16: case 17: case 18:
                    if (best_index == 12) {
                        battle.getJoueur1().getTerrain().setCapsuleTerrain(CapsuleTerrain(battle.getJoueur2().getTerrain().getCapsuleTerrain().getId()));
                    }
                    else {
                        battle.getJoueur1().getTerrain().setCapsuleTerrain(CapsuleTerrain(best_index));
                    }
                    phaseCounter++;
                    break;
                case 11: case 13: case 14:
                    battle.getJoueur2().getTerrain().setCapsuleTerrain(CapsuleTerrain(best_index));
                    phaseCounter++;
                    break;
                default:
                    break;
                }
                ui->P1->setText("Joueur 1");
                ui->P2->setText("Joueur 2 a la main : prep chgt T");
            }
        }
        break;
    case 10: // Phase de preparation changement terrain
        if (battle.getCurrentPlayer() == 0) {
            if (battle.getJoueur1().getTerrain().getCapsuleTerrain().getEffetInfo() == "Bloqué") {
                phaseCounter++;
                ui->P1->setText("Joueur 1");
                ui->P2->setText("Joueur 2 a la main : interuption");
            }
            else {
                if (best_index == -1) {
                    phaseCounter++;
                    ui->P1->setText("Joueur 1");
                    ui->P2->setText("Joueur 2 a la main : interuption");
                }
                if (best_index >= 10 && best_index <= 18) {
                    switch (best_index) {
                    case 10: case 12: case 15: case 16: case 17: case 18:
                        if (best_index == 12) {
                            battle.getJoueur1().getTerrain().setCapsuleTerrain(CapsuleTerrain(battle.getJoueur2().getTerrain().getCapsuleTerrain().getId()));
                        }
                        else {
                            battle.getJoueur1().getTerrain().setCapsuleTerrain(CapsuleTerrain(best_index));
                        }
                        phaseCounter++;
                        break;
                    case 11: case 13: case 14:
                        battle.getJoueur2().getTerrain().setCapsuleTerrain(CapsuleTerrain(best_index));
                        phaseCounter++;
                        break;
                    default:
                        break;
                    }
                    phaseCounter++;
                    ui->P1->setText("Joueur 1");
                    ui->P2->setText("Joueur 2 a la main : interuption");
                }
            }
        }
        else {
            if (battle.getJoueur2().getTerrain().getCapsuleTerrain().getEffetInfo() == "Bloqué") {
                phaseCounter++;
                ui->P1->setText("Joueur 1 a la main : interuption");
                ui->P2->setText("Joueur 2");
            }
            else {
                if (best_index == -1) {
                    phaseCounter++;
                    ui->P1->setText("Joueur 1 a la main : interuption");
                    ui->P2->setText("Joueur 2");
                }
                if (best_index >= 10 && best_index <= 18) {
                    switch (best_index) {
                    case 10: case 12: case 15: case 16: case 17: case 18:
                        if (best_index == 12) {
                            battle.getJoueur2().getTerrain().setCapsuleTerrain(CapsuleTerrain(battle.getJoueur1().getTerrain().getCapsuleTerrain().getId()));
                        }
                        else {
                            battle.getJoueur2().getTerrain().setCapsuleTerrain(CapsuleTerrain(best_index));
                        }
                        phaseCounter++;
                        break;
                    case 11: case 13: case 14:
                        battle.getJoueur1().getTerrain().setCapsuleTerrain(CapsuleTerrain(best_index));
                        phaseCounter++;
                        break;
                    default:
                        break;
                    }
                    phaseCounter++;
                    ui->P1->setText("Joueur 1 a la main : interuption");
                    ui->P2->setText("Joueur 2");
                }
            }
        }
        break;
    case 11: // Phase d'interuption
        if (battle.getCurrentPlayer() == 0) {
            if (best_index == -1) {
                ui->arenaOpenGLWidget->indexAnimationGauche = 1;
                ui->arenaOpenGLWidget->indexAnimationDroite = 2;
                phaseCounter++;
                ui->P1->setText("Joueur 1 a la main : atk");
                ui->P2->setText("Joueur 2");
            }
            if (best_index >= 10 && best_index <= 18) {
                switch (best_index) {
                case 10: case 12: case 15: case 16: case 17: case 18:
                    if (best_index == 12) {
                        battle.getJoueur2().getTerrain().setCapsuleTerrain(CapsuleTerrain(battle.getJoueur1().getTerrain().getCapsuleTerrain().getId()));
                    }
                    else {
                        battle.getJoueur2().getTerrain().setCapsuleTerrain(CapsuleTerrain(best_index));
                    }
                    phaseCounter++;
                    break;
                case 11: case 13: case 14:
                    battle.getJoueur1().getTerrain().setCapsuleTerrain(CapsuleTerrain(best_index));
                    phaseCounter++;
                    break;
                default:
                    break;
                }
                ui->arenaOpenGLWidget->indexAnimationGauche = 1;
                ui->arenaOpenGLWidget->indexAnimationDroite = 2;
                ui->P1->setText("Joueur 1 a la main : atk");
                ui->P2->setText("Joueur 2");
            }
        }
        if (battle.getCurrentPlayer() == 1) {
            if (best_index == -1) {
                ui->arenaOpenGLWidget->indexAnimationGauche = 2;
                ui->arenaOpenGLWidget->indexAnimationDroite = 1;
                phaseCounter++;
                ui->P1->setText("Joueur 1");
                ui->P2->setText("Joueur 2 a la main : atk");
            }
            if (best_index >= 10 && best_index <= 18) {
                switch (best_index) {
                case 10: case 12: case 15: case 16: case 17: case 18:
                    if (best_index == 12) {
                        battle.getJoueur1().getTerrain().setCapsuleTerrain(CapsuleTerrain(battle.getJoueur2().getTerrain().getCapsuleTerrain().getId()));
                    }
                    else {
                        battle.getJoueur1().getTerrain().setCapsuleTerrain(CapsuleTerrain(best_index));
                    }
                    phaseCounter++;
                    break;
                case 11: case 13: case 14:
                    battle.getJoueur2().getTerrain().setCapsuleTerrain(CapsuleTerrain(best_index));
                    phaseCounter++;
                    break;
                default:
                    break;
                }
                ui->arenaOpenGLWidget->indexAnimationGauche = 2;
                ui->arenaOpenGLWidget->indexAnimationDroite = 1;
                ui->P1->setText("Joueur 1");
                ui->P2->setText("Joueur 2 a la main : atk");
            }
        }
        break;
    case 12: // Phase de combat
        if (battle.getCurrentPlayer() == 0) {
            float bonus = 0;
            if (battle.getJoueur1().getTerrain().hasCapsuleTerrain()) {
                if (battle.getJoueur1().getTerrain().getCapsuleTerrain().getEffetInfo() == "Type Feu, +100 ATQ") {
                    battle.getJoueur1().getTerrain().getQuadrimon().setType(Quadrimon::Feu);
                    if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon1().getId()) {
                        battle.getJoueur1().getQuadrimon1().setType(Quadrimon::Feu);
                    }
                    if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon2().getId()) {
                        battle.getJoueur1().getQuadrimon2().setType(Quadrimon::Feu);
                    }

                    if (battle.getJoueur1().getTerrain().getQuadrimon().getType() == Quadrimon::Feu) {
                        battle.getJoueur1().getTerrain().getQuadrimon().setPointsAttaque(Quadrimon(battle.getJoueur1().getTerrain().getQuadrimon().getId()).getPointsAttaque()+100);
                        if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon1().getId()) {
                            battle.getJoueur1().getQuadrimon1().setPointsVie(Quadrimon(battle.getJoueur1().getQuadrimon1().getId()).getPointsAttaque()+100);
                        }
                        if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon2().getId()) {
                            battle.getJoueur1().getQuadrimon2().setPointsVie(Quadrimon(battle.getJoueur1().getQuadrimon2().getId()).getPointsAttaque()+100);
                        }
                    }

                }
                if (battle.getJoueur1().getTerrain().getCapsuleTerrain().getEffetInfo() == "Type Plante, +50 PV") {
                    battle.getJoueur1().getTerrain().getQuadrimon().setType(Quadrimon::Plante);
                    if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon1().getId()) {
                        battle.getJoueur1().getQuadrimon1().setType(Quadrimon::Plante);
                    }
                    if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon2().getId()) {
                        battle.getJoueur1().getQuadrimon2().setType(Quadrimon::Plante);
                    }

                    if (battle.getJoueur1().getTerrain().getQuadrimon().getType() == Quadrimon::Plante) {
                        battle.getJoueur1().getTerrain().getQuadrimon().setPointsVie(battle.getJoueur1().getTerrain().getQuadrimon().getPointsVie()+50);
                        if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon1().getId()) {
                            battle.getJoueur1().getQuadrimon1().setPointsVie(battle.getJoueur1().getQuadrimon1().getPointsVie()+50);
                        }
                        if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon2().getId()) {
                            battle.getJoueur1().getQuadrimon2().setPointsVie(battle.getJoueur1().getQuadrimon2().getPointsVie()+50);
                        }
                    }
                }
                if (battle.getJoueur1().getTerrain().getCapsuleTerrain().getEffetInfo() == "Type Eau, Bonus Type x2.5") {
                    battle.getJoueur1().getTerrain().getQuadrimon().setType(Quadrimon::Eau);
                    if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon1().getId()) {
                        battle.getJoueur1().getQuadrimon1().setType(Quadrimon::Eau);
                    }
                    if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon2().getId()) {
                        battle.getJoueur1().getQuadrimon2().setType(Quadrimon::Eau);
                    }

                    if (battle.getJoueur1().getTerrain().getQuadrimon().getType() == Quadrimon::Eau) {
                        bonus += 0.5;
                    }
                }
                if (battle.getJoueur1().getTerrain().getCapsuleTerrain().getId() != 16 && battle.getJoueur1().getTerrain().getCapsuleTerrain().getId() != 17 && battle.getJoueur1().getTerrain().getCapsuleTerrain().getId() != 18) {
                    battle.getJoueur1().getTerrain().getQuadrimon().setType(Quadrimon(battle.getJoueur1().getQuadrimon1().getId()).getType());
                    if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon1().getId()) {
                        battle.getJoueur1().getQuadrimon1().setType(Quadrimon(battle.getJoueur1().getQuadrimon1().getId()).getType());
                    }
                    if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon2().getId()) {
                        battle.getJoueur1().getQuadrimon2().setType(Quadrimon(battle.getJoueur1().getQuadrimon1().getId()).getType());
                    }
                }
            }

            if (battle.getJoueur1().getTerrain().getQuadrimon().getPointsVie() <= 0) {
                battle.getJoueur1().getTerrain().getQuadrimon().setPointsVie(0);
                if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon1().getId()) {
                    battle.getJoueur1().getQuadrimon1().setPointsVie(0);
                    ui->arenaOpenGLWidget->indexAnimationGauche = 3;
                }
                if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon2().getId()) {
                    battle.getJoueur1().getQuadrimon2().setPointsVie(0);
                    ui->arenaOpenGLWidget->indexAnimationGauche = 3;
                }
                ui->P1->setText("Joueur 1 : Quadrimon Mort");
                ui->P2->setText("Joueur 2");
            }
            else {
                Quadrimon& Attaquant = battle.getJoueur1().getTerrain().getQuadrimon();
                Quadrimon& Defenseur = battle.getJoueur2().getTerrain().getQuadrimon();

                float avantage = 1;
                if (Attaquant.getType() == Quadrimon::Feu && Defenseur.getType() == Quadrimon::Plante) {
                    avantage = 2;
                }
                if (Attaquant.getType() == Quadrimon::Plante && Defenseur.getType() == Quadrimon::Eau) {
                    avantage = 2;
                }
                if (Attaquant.getType() == Quadrimon::Eau && Defenseur.getType() == Quadrimon::Feu) {
                    avantage = 2;
                }
                avantage += bonus;

                int atq = Attaquant.getPointsAttaque() * avantage;

                battle.getJoueur2().getTerrain().getQuadrimon().setPointsVie(battle.getJoueur2().getTerrain().getQuadrimon().getPointsVie()-atq);

                if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon1().getId()) {
                    battle.getJoueur2().getQuadrimon1().setPointsVie(battle.getJoueur2().getQuadrimon1().getPointsVie()-atq);
                }
                if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon2().getId()) {
                    battle.getJoueur2().getQuadrimon2().setPointsVie(battle.getJoueur2().getQuadrimon2().getPointsVie()-atq);
                }

                if (battle.getJoueur2().getTerrain().getQuadrimon().getPointsVie() <= 0) {
                    battle.getJoueur2().getTerrain().getQuadrimon().setPointsVie(0);
                    if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon1().getId()) {
                        battle.getJoueur2().getQuadrimon1().setPointsVie(0);
                        ui->arenaOpenGLWidget->indexAnimationDroite = 3;
                    }
                    if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon2().getId()) {
                        battle.getJoueur2().getQuadrimon2().setPointsVie(0);
                        ui->arenaOpenGLWidget->indexAnimationDroite = 3;
                    }
                    ui->P1->setText("Joueur 1");
                    ui->P2->setText("Joueur 2 : Quadrimon Mort");
                }
                else {
                    ui->P1->setText("Joueur 1");
                    ui->P2->setText("Joueur 2 a la main interuption");
                    ui->arenaOpenGLWidget->indexAnimationGauche = 0;
                    ui->arenaOpenGLWidget->indexAnimationDroite = 0;
                    phaseCounter++;
                }
            }          
        }
        else {
            float bonus = 0;
            if (battle.getJoueur2().getTerrain().hasCapsuleTerrain()) {
                if (battle.getJoueur2().getTerrain().getCapsuleTerrain().getEffetInfo() == "Type Feu, +100 ATQ") {
                    battle.getJoueur2().getTerrain().getQuadrimon().setType(Quadrimon::Feu);
                    if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon1().getId()) {
                        battle.getJoueur2().getQuadrimon1().setType(Quadrimon::Feu);
                    }
                    if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon2().getId()) {
                        battle.getJoueur2().getQuadrimon2().setType(Quadrimon::Feu);
                    }

                    if (battle.getJoueur2().getTerrain().getQuadrimon().getType() == Quadrimon::Feu) {
                        battle.getJoueur2().getTerrain().getQuadrimon().setPointsAttaque(Quadrimon(battle.getJoueur2().getTerrain().getQuadrimon().getId()).getPointsAttaque()+100);
                        if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon1().getId()) {
                            battle.getJoueur2().getQuadrimon1().setPointsVie(Quadrimon(battle.getJoueur2().getQuadrimon1().getId()).getPointsAttaque()+100);
                        }
                        if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon2().getId()) {
                            battle.getJoueur2().getQuadrimon2().setPointsVie(Quadrimon(battle.getJoueur2().getQuadrimon2().getId()).getPointsAttaque()+100);
                        }
                    }

                }
                if (battle.getJoueur2().getTerrain().getCapsuleTerrain().getEffetInfo() == "Type Plante, +50 PV") {
                    battle.getJoueur2().getTerrain().getQuadrimon().setType(Quadrimon::Plante);
                    if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon1().getId()) {
                        battle.getJoueur2().getQuadrimon1().setType(Quadrimon::Plante);
                    }
                    if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon2().getId()) {
                        battle.getJoueur2().getQuadrimon2().setType(Quadrimon::Plante);
                    }

                    if (battle.getJoueur2().getTerrain().getQuadrimon().getType() == Quadrimon::Plante) {
                        battle.getJoueur2().getTerrain().getQuadrimon().setPointsVie(battle.getJoueur2().getTerrain().getQuadrimon().getPointsVie()+50);
                        if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon1().getId()) {
                            battle.getJoueur2().getQuadrimon1().setPointsVie(battle.getJoueur2().getQuadrimon1().getPointsVie()+50);
                        }
                        if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon2().getId()) {
                            battle.getJoueur2().getQuadrimon2().setPointsVie(battle.getJoueur2().getQuadrimon2().getPointsVie()+50);
                        }
                    }
                }
                if (battle.getJoueur2().getTerrain().getCapsuleTerrain().getEffetInfo() == "Type Eau, Bonus Type x2.5") {
                    battle.getJoueur2().getTerrain().getQuadrimon().setType(Quadrimon::Eau);
                    if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon1().getId()) {
                        battle.getJoueur2().getQuadrimon1().setType(Quadrimon::Eau);
                    }
                    if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon2().getId()) {
                        battle.getJoueur2().getQuadrimon2().setType(Quadrimon::Eau);
                    }

                    if (battle.getJoueur2().getTerrain().getQuadrimon().getType() == Quadrimon::Eau) {
                        bonus += 0.5;
                    }
                }
                if (battle.getJoueur2().getTerrain().getCapsuleTerrain().getId() != 16 && battle.getJoueur2().getTerrain().getCapsuleTerrain().getId() != 17 && battle.getJoueur2().getTerrain().getCapsuleTerrain().getId() != 18) {
                    battle.getJoueur2().getTerrain().getQuadrimon().setType(Quadrimon(battle.getJoueur2().getQuadrimon1().getId()).getType());
                    if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon1().getId()) {
                        battle.getJoueur2().getQuadrimon1().setType(Quadrimon(battle.getJoueur2().getQuadrimon1().getId()).getType());
                    }
                    if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon2().getId()) {
                        battle.getJoueur2().getQuadrimon2().setType(Quadrimon(battle.getJoueur2().getQuadrimon1().getId()).getType());
                    }
                }
            }

            if (battle.getJoueur2().getTerrain().getQuadrimon().getPointsVie() <= 0) {
                battle.getJoueur2().getTerrain().getQuadrimon().setPointsVie(0);
                if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon1().getId()) {
                    battle.getJoueur2().getQuadrimon1().setPointsVie(0);
                    ui->arenaOpenGLWidget->indexAnimationDroite = 3;
                }
                if (battle.getJoueur2().getTerrain().getQuadrimon().getId() == battle.getJoueur2().getQuadrimon2().getId()) {
                    battle.getJoueur2().getQuadrimon2().setPointsVie(0);
                    ui->arenaOpenGLWidget->indexAnimationDroite = 3;
                }
                ui->P1->setText("Joueur 1");
                ui->P2->setText("Joueur 2 : Quadrimon Mort");
            }
            else {
                Quadrimon Attaquant = battle.getJoueur2().getTerrain().getQuadrimon();
                Quadrimon Defenseur = battle.getJoueur1().getTerrain().getQuadrimon();

                float avantage = 1;
                if (Attaquant.getType() == Quadrimon::Feu && Defenseur.getType() == Quadrimon::Plante) {
                    avantage = 2;
                }
                if (Attaquant.getType() == Quadrimon::Plante && Defenseur.getType() == Quadrimon::Eau) {
                    avantage = 2;
                }
                if (Attaquant.getType() == Quadrimon::Eau && Defenseur.getType() == Quadrimon::Feu) {
                    avantage = 2;
                }
                avantage += bonus;

                int atq = Attaquant.getPointsAttaque() * avantage;

                battle.getJoueur1().getTerrain().getQuadrimon().setPointsVie(battle.getJoueur1().getTerrain().getQuadrimon().getPointsVie()-atq);

                if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon1().getId()) {
                    battle.getJoueur1().getQuadrimon1().setPointsVie(battle.getJoueur1().getQuadrimon1().getPointsVie()-atq);
                }
                if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon2().getId()) {
                    battle.getJoueur1().getQuadrimon2().setPointsVie(battle.getJoueur1().getQuadrimon2().getPointsVie()-atq);
                }

                if (battle.getJoueur1().getTerrain().getQuadrimon().getPointsVie() <= 0) {
                    battle.getJoueur1().getTerrain().getQuadrimon().setPointsVie(0);
                    if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon1().getId()) {
                        battle.getJoueur1().getQuadrimon1().setPointsVie(0);
                        ui->arenaOpenGLWidget->indexAnimationGauche = 3;
                    }
                    if (battle.getJoueur1().getTerrain().getQuadrimon().getId() == battle.getJoueur1().getQuadrimon2().getId()) {
                        battle.getJoueur1().getQuadrimon2().setPointsVie(0);
                        ui->arenaOpenGLWidget->indexAnimationGauche = 3;
                    }
                    ui->P1->setText("Joueur 1 : Quadrimon Mort");
                    ui->P2->setText("Joueur 2");
                }
                else {
                    ui->P1->setText("Joueur 1 a la main interuption");
                    ui->P2->setText("Joueur 2");
                    ui->arenaOpenGLWidget->indexAnimationGauche = 0;
                    ui->arenaOpenGLWidget->indexAnimationDroite = 0;
                    phaseCounter++;
                }
            }

        }
        phaseCounter++;
        break;
    case 13: // Phase d'animation de mort
        if (battle.getJoueur1().getTerrain().getQuadrimon().getPointsVie() <= 0) {
            if (battle.getJoueur1().getQuadrimon1().getPointsVie() > 0 && battle.getJoueur1().getQuadrimon1().getId() != battle.getJoueur1().getTerrain().getQuadrimon().getId()) {
                battle.getJoueur1().getTerrain().setQuadrimon(battle.getJoueur1().getQuadrimon1());
            }
            if (battle.getJoueur1().getQuadrimon2().getPointsVie() > 0 && battle.getJoueur1().getQuadrimon2().getId() != battle.getJoueur1().getTerrain().getQuadrimon().getId()) {
                battle.getJoueur1().getTerrain().setQuadrimon(battle.getJoueur1().getQuadrimon2());
            }
        }
        if (battle.getJoueur2().getTerrain().getQuadrimon().getPointsVie() <= 0) {
            if (battle.getJoueur2().getQuadrimon1().getPointsVie() > 0 && battle.getJoueur2().getQuadrimon1().getId() != battle.getJoueur2().getTerrain().getQuadrimon().getId()) {
                battle.getJoueur2().getTerrain().setQuadrimon(battle.getJoueur2().getQuadrimon1());
            }
            if (battle.getJoueur2().getQuadrimon2().getPointsVie() > 0 && battle.getJoueur2().getQuadrimon2().getId() != battle.getJoueur2().getTerrain().getQuadrimon().getId()) {
                battle.getJoueur2().getTerrain().setQuadrimon(battle.getJoueur2().getQuadrimon2());
            }
        }
        if (battle.getCurrentPlayer() == 0) {
            ui->P1->setText("Joueur 1");
            ui->P2->setText("Joueur 2 a la main interuption");
        }
        if (battle.getCurrentPlayer() == 1) {
            ui->P1->setText("Joueur 1 a la main interuption");
            ui->P2->setText("Joueur 2");
        }
        ui->arenaOpenGLWidget->indexAnimationGauche = 0;
        ui->arenaOpenGLWidget->indexAnimationDroite = 0;
        phaseCounter++;
        if (battle.getJoueur1().getQuadrimon1().getPointsVie() <= 0 && battle.getJoueur1().getQuadrimon2().getPointsVie() <= 0) {
            ui->P1->setText("Joueur 1 a perdu");
            ui->P2->setText("Joueur 2 a gagner");
            ui->arenaOpenGLWidget->indexAnimationGauche = 3;
            ui->arenaOpenGLWidget->indexAnimationDroite = 4;
            phaseCounter = 16;
        }
        if (battle.getJoueur2().getQuadrimon1().getPointsVie() <= 0 && battle.getJoueur2().getQuadrimon2().getPointsVie() <= 0) {
            ui->P1->setText("Joueur 1 a gagner");
            ui->P2->setText("Joueur 2 a perdu");
            ui->arenaOpenGLWidget->indexAnimationGauche = 4;
            ui->arenaOpenGLWidget->indexAnimationDroite = 3;
            phaseCounter = 16;
        }
        break;
    case 14: // Phase d'interuption
        if (battle.getCurrentPlayer() == 0) {
            if (best_index == -1) {
                phaseCounter++;
                ui->P1->setText("Joueur 1 a la main : fin de tour");
                ui->P2->setText("Joueur 2");
            }
            if (best_index >= 10 && best_index <= 18) {
                switch (best_index) {
                case 10: case 12: case 15: case 16: case 17: case 18:
                    if (best_index == 12) {
                        battle.getJoueur2().getTerrain().setCapsuleTerrain(CapsuleTerrain(battle.getJoueur1().getTerrain().getCapsuleTerrain().getId()));
                    }
                    else {
                        battle.getJoueur2().getTerrain().setCapsuleTerrain(CapsuleTerrain(best_index));
                    }
                    phaseCounter++;
                    break;
                case 11: case 13: case 14:
                    battle.getJoueur1().getTerrain().setCapsuleTerrain(CapsuleTerrain(best_index));
                    phaseCounter++;
                    break;
                default:
                    break;
                }
                ui->P1->setText("Joueur 1 a la main : fin de tour");
                ui->P2->setText("Joueur 2");
            }
        }
        if (battle.getCurrentPlayer() == 1) {
            if (best_index == -1) {
                phaseCounter++;
                ui->P1->setText("Joueur 1");
                ui->P2->setText("Joueur 2 a la main : fin de tour");
            }
            if (best_index >= 10 && best_index <= 18) {
                switch (best_index) {
                case 10: case 12: case 15: case 16: case 17: case 18:
                    if (best_index == 12) {
                        battle.getJoueur1().getTerrain().setCapsuleTerrain(CapsuleTerrain(battle.getJoueur2().getTerrain().getCapsuleTerrain().getId()));
                    }
                    else {
                        battle.getJoueur1().getTerrain().setCapsuleTerrain(CapsuleTerrain(best_index));
                    }
                    phaseCounter++;
                    break;
                case 11: case 13: case 14:
                    battle.getJoueur2().getTerrain().setCapsuleTerrain(CapsuleTerrain(best_index));
                    phaseCounter++;
                    break;
                default:
                    break;
                }
                ui->P1->setText("Joueur 1");
                ui->P2->setText("Joueur 2 a la main : fin de tour");
            }
        }
        break;
    case 15: // Phase de fin de tour
        if (battle.getJoueur1().getQuadrimon1().getPointsVie() <= 0 && battle.getJoueur1().getQuadrimon2().getPointsVie() <= 0) {
            ui->P1->setText("Joueur 1 a perdu");
            ui->P2->setText("Joueur 2 a gagner");
            ui->arenaOpenGLWidget->indexAnimationGauche = 3;
            ui->arenaOpenGLWidget->indexAnimationDroite = 4;
            phaseCounter++;
        }
        if (battle.getJoueur2().getQuadrimon1().getPointsVie() <= 0 && battle.getJoueur2().getQuadrimon2().getPointsVie() <= 0) {
            ui->P1->setText("Joueur 1 a gagner");
            ui->P2->setText("Joueur 2 a perdu");
            ui->arenaOpenGLWidget->indexAnimationGauche = 4;
            ui->arenaOpenGLWidget->indexAnimationDroite = 3;
            phaseCounter++;
        }
        if ((battle.getJoueur1().getQuadrimon1().getPointsVie() > 0 || battle.getJoueur1().getQuadrimon2().getPointsVie() > 0) && (battle.getJoueur2().getQuadrimon1().getPointsVie() > 0 || battle.getJoueur2().getQuadrimon2().getPointsVie() > 0)) {
            battle.endTurn();
            ui->turnInfo->setText("Tour : " + QString::number(battle.getTurn()));
            ui->P1->setText("Joueur 1 : effet des terrains");
            ui->P2->setText("Joueur 2 : effet des terrains");
            phaseCounter = 5;
        }
        break;
    default:
        break;
    }
    displayQuadrimonAndTerrain(ui->arenaOpenGLWidget);
}

void MainWindow::displayQuadrimonAndTerrain(MyGLWidget* openGLWidget) {
    qDebug() << "---------------------------------------------------------";
    qDebug() << "Resetting OpenGL widget...";
    openGLWidget->resetShow();
    qDebug() << "";

    qDebug() << "Retrieving information for joueur1...";
    Quadrimon& joueur1Quadrimon = battle.getJoueur1().getTerrain().getQuadrimon();
    CapsuleTerrain& joueur1Terrain = battle.getJoueur1().getTerrain().getCapsuleTerrain();
    qDebug() << "    Joueur1 Quadrimon: Name=" << joueur1Quadrimon.getNom().toStdString() << ", PV=" << joueur1Quadrimon.getPointsVie() << ", ATQ=" << joueur1Quadrimon.getPointsAttaque();
    qDebug() << "    Joueur1 Terrain: Name=" << joueur1Terrain.getNom().toStdString() << ", EffetInfo=" << joueur1Terrain.getEffetInfo().toStdString() << ", TempsActivite=" << joueur1Terrain.getTempsActivite();
    qDebug() << "";

    qDebug() << "Retrieving information for joueur2...";
    Quadrimon& joueur2Quadrimon = battle.getJoueur2().getTerrain().getQuadrimon();
    CapsuleTerrain& joueur2Terrain = battle.getJoueur2().getTerrain().getCapsuleTerrain();
    qDebug() << "    Joueur2 Quadrimon: Name=" << joueur2Quadrimon.getNom().toStdString() << ", PV=" << joueur2Quadrimon.getPointsVie() << ", ATQ=" << joueur2Quadrimon.getPointsAttaque();
    qDebug() << "    Joueur2 Terrain: Name=" << joueur2Terrain.getNom().toStdString() << ", EffetInfo=" << joueur2Terrain.getEffetInfo().toStdString() << ", TempsActivite=" << joueur2Terrain.getTempsActivite();
    qDebug() << "";

    qDebug() << "Updating joueur1 information on UI...";
    openGLWidget->indexQuadrimonGauche = joueur1Quadrimon.getId();
    ui->P1_ATQ->setText(QString::number(joueur1Quadrimon.getPointsAttaque()));
    ui->P1_Name->setText(joueur1Quadrimon.getNom());
    ui->P1_PV->setText(QString::number(joueur1Quadrimon.getPointsVie()));

    openGLWidget->indexTerrainGauche = joueur1Terrain.getId();
    ui->P1_Terrain->setText(joueur1Terrain.getNom());
    ui->P1_Terrain_Info->setText(joueur1Terrain.getEffetInfo());
    ui->P1_Terrain_Turn_Left->setText(QString::number(joueur1Terrain.getTempsActivite()));
    qDebug() << "";

    qDebug() << "Updating joueur2 information on UI...";
    openGLWidget->indexQuadrimonDroite = joueur2Quadrimon.getId();
    ui->P2_ATQ->setText(QString::number(joueur2Quadrimon.getPointsAttaque()));
    ui->P2_Name->setText(joueur2Quadrimon.getNom());
    ui->P2_PV->setText(QString::number(joueur2Quadrimon.getPointsVie()));

    openGLWidget->indexTerrainDroite = joueur2Terrain.getId();
    ui->P2_Terrain->setText(joueur2Terrain.getNom());
    ui->P2_Terrain_Info->setText(joueur2Terrain.getEffetInfo());
    ui->P2_Terrain_Turn_Left->setText(QString::number(joueur2Terrain.getTempsActivite()));
    qDebug() << "";

    qDebug() << "Updating individual quadrimon information on UI...";
    qDebug() << "    Updating joueur1 Quadrimon1...";
    Quadrimon& joueur1Quadrimon1 = battle.getJoueur1().getQuadrimon1();
    qDebug() << "        Name=" << joueur1Quadrimon1.getNom().toStdString() << ", PV=" << joueur1Quadrimon1.getPointsVie() << ", ATQ=" << joueur1Quadrimon1.getPointsAttaque();
    ui->P1_Q1_ATQ->setText(QString::number(joueur1Quadrimon1.getPointsAttaque()));
    ui->P1_Q1_Name->setText(joueur1Quadrimon1.getNom());
    ui->P1_Q1_PV->setText(QString::number(joueur1Quadrimon1.getPointsVie()));
    qDebug() << "";

    qDebug() << "    Updating joueur1 Quadrimon2...";
    Quadrimon& joueur1Quadrimon2 = battle.getJoueur1().getQuadrimon2();
    qDebug() << "        Name=" << joueur1Quadrimon2.getNom().toStdString() << ", PV=" << joueur1Quadrimon2.getPointsVie() << ", ATQ=" << joueur1Quadrimon2.getPointsAttaque();
    ui->P1_Q2_ATQ->setText(QString::number(joueur1Quadrimon2.getPointsAttaque()));
    ui->P1_Q2_Name->setText(joueur1Quadrimon2.getNom());
    ui->P1_Q2_PV->setText(QString::number(joueur1Quadrimon2.getPointsVie()));
    qDebug() << "";

    qDebug() << "    Updating joueur2 Quadrimon1...";
    Quadrimon& joueur2Quadrimon1 = battle.getJoueur2().getQuadrimon1();
    qDebug() << "        Name=" << joueur2Quadrimon1.getNom().toStdString() << ", PV=" << joueur2Quadrimon1.getPointsVie() << ", ATQ=" << joueur2Quadrimon1.getPointsAttaque();
    ui->P2_Q1_ATQ->setText(QString::number(joueur2Quadrimon1.getPointsAttaque()));
    ui->P2_Q1_Name->setText(joueur2Quadrimon1.getNom());
    ui->P2_Q1_PV->setText(QString::number(joueur2Quadrimon1.getPointsVie()));
    qDebug() << "";

    qDebug() << "    Updating joueur2 Quadrimon2...";
    Quadrimon& joueur2Quadrimon2 = battle.getJoueur2().getQuadrimon2();
    qDebug() << "        Name=" << joueur2Quadrimon2.getNom().toStdString() << ", PV=" << joueur2Quadrimon2.getPointsVie() << ", ATQ=" << joueur2Quadrimon2.getPointsAttaque();
    ui->P2_Q2_ATQ->setText(QString::number(joueur2Quadrimon2.getPointsAttaque()));
    ui->P2_Q2_Name->setText(joueur2Quadrimon2.getNom());
    ui->P2_Q2_PV->setText(QString::number(joueur2Quadrimon2.getPointsVie()));
    qDebug() << "";


    qDebug() << "Updating OpenGL widget...";
    openGLWidget->update();
    qDebug() << "---------------------------------------------------------";
}

void MainWindow::endTurnButtonPushed() {
    best_index = -1;
    displayCardImages(0, ui->cardImage, ui->cardInfo, ui->arenaOpenGLWidget);
    validateButtonPushed();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_C:
        captureButtonPushed();
        break;
    case Qt::Key_V:
        validateButtonPushed();
        break;
    case Qt::Key_S:
        endTurnButtonPushed();
        break;
    default:
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::pauseExecution(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
