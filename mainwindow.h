#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "calibrationprocess.h"
#include <QButtonGroup>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void resimKaydetmeIslemi(QStringList resimList);

private slots:
    void on_selectImages_clicked();

    void on_startCalibration_clicked();



private:
    Ui::MainWindow *ui;

     calibrationProcess cameraCalib;
     int flag=0;
     Size boardSize;
     QButtonGroup group;
     Mat frame;
     Mat drawToFrame;
     Mat distanceCoefficients;
     vector<Mat> savedImages;
     vector<vector<Point2f>> markerCorners,rejectedCandidates;
     bool koseleriGoster,sonucuGoster;
     int status=0;

};

#endif // MAINWINDOW_H
