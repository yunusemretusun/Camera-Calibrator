#ifndef CALIBRATIONPROCESS_H
#define CALIBRATIONPROCESS_H

#include <stdio.h>
#include <stdlib.h>


#include <opencv2/opencv.hpp>
#include<QtCore>
#include<QtXml>
#include<QDebug>

using namespace cv;
using namespace std;


class calibrationProcess
{
public:
    calibrationProcess();

    void getChessboardCorners(vector<Mat> images,vector<vector<Point2f>>& allFoundCorners,bool showResult);
    void cameraCalibration(vector<Mat> calibrationImages,Size chessboardSize,float squareEdgeLength,Mat &cameraMatrix,Mat &distanceCoefficients,int flag);

    bool saveCameraCalibrationToXml(QString fileName,Mat cameraMatrix,Mat distanceCoefficients,double fovx,double fovy);

    Size chessboardDimension;
    const float squareSize=0.01905f;
    Size imageSize;
    Mat distanceCoefficients;
    Mat cameraMatrix=Mat::eye(3,3,CV_64F);
    double fovx,fovy;
    double focalLength,apertureWidth,apertureHeight,pasp;
    Point2d principalPoint;
    vector<Mat> rVectors,tVectors;
    vector<float> reprojErrs;
    bool showUndistortedImage;
private:
    double meanError;
    bool mustInitUndistort;
    void createKnownBoardPosition(Size boardSize,float squareEdgeLength,vector<Point3f>& corners);

};

#endif // CALIBRATIONPROCESS_H
