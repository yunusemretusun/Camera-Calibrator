#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QMessageBox>
#include <QtXml/QtXml>
#include "src/calibrationprocess.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_selectImages_clicked()
{
    boardSize.width=ui->BoardSize_Width->currentIndex();
    boardSize.height=ui->BoardSize_Height->currentIndex();
    cameraCalib.chessboardDimension=boardSize;
    koseleriGoster=ui->koseleriGosterr->checkState();
    cameraCalib.showUndistortedImage=ui->sonucuGoster->checkState();
    savedImages.clear();


    if(boardSize.width==0 || boardSize.height==0){
        QMessageBox::warning(this,tr("Uyarı"),tr("Lütfen öncelikle Checker Board boyut bilgisini giriniz!"));
    }
    else{
        ui->progressBar->setValue(15);
        //code for browse picture
               QStringList files = QFileDialog::getOpenFileNames(this,"Kalibrasyon için gerekli resimleri seçiniz","Resimler (*.png *.jpg *.bmp *.jpeg)");
                QMessageBox::StandardButton reply;

                if( files.size()!=0 && files.size()<10){
               reply = QMessageBox::question(this, "Bilgilendirme", "İyi bir kalibrasyon için en az 10 resim yüklemeniz tavsiye edilir,Devam etmek istiyor musunuz?",
                                               QMessageBox::Yes|QMessageBox::No);
                 if (reply == QMessageBox::Yes) {

                        resimKaydetmeIslemi(files);
                        ui->progressBar->setValue(50);

                 }
                 else
                 {
                          QMessageBox::warning(this,tr("Bilgilendirme"),tr("Lütfen resimleri tekrar seçiniz"));
                 }
               }
                else{
                  resimKaydetmeIslemi(files);
                  ui->progressBar->setValue(50);
                }
    }

}


void MainWindow::resimKaydetmeIslemi(QStringList resimList){

    for (int i =0;i<resimList.count();i++){
        string path=(resimList.at(i).toUtf8().constData());
        frame=cv::imread(path);
        cameraCalib.imageSize=frame.size();
         vector<Vec2f> foundPoints;
         bool found=false;

         found=findChessboardCorners(frame,boardSize,foundPoints,CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);

         if(found){
             Mat temp;
             frame.copyTo(temp);
             savedImages.push_back(temp);
         }

         if(koseleriGoster){
             frame.copyTo(drawToFrame);
             drawChessboardCorners(drawToFrame,boardSize,foundPoints,found);
             imshow("Köşeleri Tespit Edilmiş Resim",drawToFrame);
             waitKey(0);
         }

        }

    if(!savedImages.empty()){
        destroyAllWindows();
        QMessageBox::warning(this,tr("Bilgilendirme"),tr("Kalibrasyon İşlemine Geçebilirsiniz"));

    }
}

void MainWindow::on_startCalibration_clicked()
{

//selecting parameter process
    QList<QRadioButton *> allButtons = ui->groupBox->findChildren<QRadioButton *>();

      for(int i = 0; i < allButtons.size(); ++i)
      {
          group.addButton(allButtons[i],i);
      }
      int selectedParameterId= group.checkedId();

      switch (selectedParameterId) {
      case 0:
          flag=CALIB_USE_INTRINSIC_GUESS;
          break;
      case 1:
          flag=CALIB_FIX_PRINCIPAL_POINT;
          break;
      case 2:
          flag=CALIB_FIX_ASPECT_RATIO;
          break;
      case 3:
          flag=CALIB_ZERO_TANGENT_DIST;
          break;
      case 4:
          flag=CALIB_FIX_K1| CALIB_FIX_K2|CALIB_FIX_K3|CALIB_FIX_K4|CALIB_FIX_K5|CALIB_FIX_K6;
          break;
      case 5:
          flag=CALIB_RATIONAL_MODEL;
          break;
      case 6:
          flag=0;
          break;
      default:
          flag=0;
          break;
      }

    if(savedImages.empty() || boardSize==Size(0,0) )
    {
        QMessageBox::warning(this,tr("Bilgilendirme"),tr("Lütfen gerekli bilgileri sağlayınız"));
    }
    else
    {
        Mat distCoeffs;
        Mat cameraMatrix=Mat::eye(3,3,CV_64F);


            QMessageBox::information(this,tr("Bilgilendirme"),tr("Kalibrasyon işlemi başladı."));
            cameraCalib.cameraCalibration(savedImages,boardSize,cameraCalib.squareSize,cameraMatrix,distCoeffs,flag);

            ui->progressBar->setValue(90);
            QMessageBox::information(this,tr("Bilgilendirme"),tr("Kalibrasyon tamamlandı"));

            QMessageBox::information(this,tr("Bilgilendirme"),("Parametrelerin kaydedileceği XML dosyasını oluşturunuz!"));

            QString filename = QFileDialog::getSaveFileName(this,tr("Save Xml"), ".",tr("Xml files (*.xml)"));




            bool dosyaDurum=cameraCalib.saveCameraCalibrationToXml(filename,cameraMatrix,distCoeffs, cameraCalib.fovx, cameraCalib.fovy);

        if(dosyaDurum){
            QMessageBox::information(this,tr("Uyarı"),tr("Dosya Başarıyla Kaydedildi"));


        }
        else{
            QMessageBox::information(this,tr("Uyarı"),tr("Xml Dosyası oluşturma işlemi iptal edildi."));
        }

        ui->progressBar->setValue(100);

    }

}




