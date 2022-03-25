#include "calibrationprocess.h"

calibrationProcess::calibrationProcess():
    meanError(0)

{
}

void calibrationProcess::createKnownBoardPosition(Size boardSize,float squareEdgeLength,vector<Point3f>& corners){

    for(int i=0;i<boardSize.height;i++)
    {
        for(int j=0;j<boardSize.width;j++)
        {
            //corners.push_back(Point3f(j*squareEdgeLength,i*squareEdgeLength,0.0f));
            corners.push_back(Point3f(i,j,0.0f));


        }
    }
}

void calibrationProcess::getChessboardCorners(vector<Mat> images,vector<vector<Point2f>> &allFoundCorners,bool showResult=false)
{

for(vector<Mat>::iterator iter=images.begin(); iter!=images.end();iter++)
{

    vector<Point2f> pointBuf;
    bool found=findChessboardCorners(*iter,chessboardDimension,pointBuf,CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);

    if(found)
    {
        allFoundCorners.push_back(pointBuf);

    }

    if(showResult)
     {
         drawChessboardCorners(*iter,chessboardDimension,pointBuf,found);
         imshow("Loking for corners",*iter);
         waitKey(0);
     }
}
}

void calibrationProcess::cameraCalibration(vector<Mat> calibrationImages,Size chessboardSize,float squareEdgeLength,Mat &cameraMatrix,Mat &distCoeffs,int flag)
{
    vector<vector<Point2f>> checkerboardImageSpacePoints;
    getChessboardCorners(calibrationImages,checkerboardImageSpacePoints,false);

    vector<vector<Point3f>> worldSpaceCornerPoints(1);

    createKnownBoardPosition(chessboardSize,squareEdgeLength,worldSpaceCornerPoints[0]);
    worldSpaceCornerPoints.resize(checkerboardImageSpacePoints.size(),worldSpaceCornerPoints[0]);


    distCoeffs=Mat::zeros(1,8,CV_64F);

    calibrateCamera(worldSpaceCornerPoints,checkerboardImageSpacePoints,chessboardSize,cameraMatrix,distCoeffs,rVectors,tVectors,flag);

    Mat imageUndistorted;
    Mat map1, map2;
    if(showUndistortedImage)
    {

        initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(),
                   getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, 0),
                   imageSize, CV_16SC2, map1, map2);

        for(int i=0;i<calibrationImages.size();i++){

            remap(calibrationImages[i],imageUndistorted,map1, map2, INTER_LINEAR);

            imshow("Sonuç",imageUndistorted);
            waitKey(0);
           }

        destroyAllWindows();
    }

    //re-projection hatasını hesaplama
    vector<Point2f> imagePoints2;
    vector<float> perViewErrors;
    int totalPoints = 0;
    double totalErr = 0, err;
    perViewErrors.resize(worldSpaceCornerPoints.size());

    for(int i=0;i<calibrationImages.size();i++){
        projectPoints(worldSpaceCornerPoints[i],rVectors[i],tVectors[i],cameraMatrix,distCoeffs,imagePoints2);
        err = norm(Mat(checkerboardImageSpacePoints[i]), Mat(imagePoints2), NORM_L2);

         int n = (int)worldSpaceCornerPoints[i].size();
         perViewErrors[i] = (float) std::sqrt(err*err/n);
         totalErr        += err*err;
         totalPoints     += n;
       }

meanError=std::sqrt(totalErr/totalPoints);
//re-projection hatasını hesaplama
}



bool calibrationProcess::saveCameraCalibrationToXml(QString fileName,Mat cameraMatrix,Mat distanceCoefficients,double fovx,double fovy){

    //write XML
    QDomDocument document;

    //Make the root element
    QDomElement head=document.createElement("Camera Parameters");
    QDomElement camMatrixTag=document.createElement("Camera Matrix");
    QDomElement distMatrixTag=document.createElement("Distortion Coefficent");
    QDomElement fieldOfView=document.createElement("Field Of View");
    QDomElement reProjectionError=document.createElement("Re-projection Error");

    //add it to the document
    document.appendChild(head);
    document.appendChild(camMatrixTag);
    document.appendChild(distMatrixTag);
    document.appendChild(fieldOfView);
    document.appendChild(reProjectionError);

    //Add some Element
    for(int r=0;r<cameraMatrix.rows;r++)
    {
        for(int c=0;c<cameraMatrix.cols;c++)
        {
            double value=cameraMatrix.at<double>(r,c);

            QDomElement cameraMatrix=document.createElement("Value");
            cameraMatrix.setAttribute("["+QString::number(r)+"]["+QString::number(c)+"]",value);
            camMatrixTag.appendChild(cameraMatrix);
        }

    }

    for(int i=0;i<distanceCoefficients.cols;i++)
    {
        for(int j=0;j<distanceCoefficients.rows;j++)
        {
            double values=distanceCoefficients.at<double>(i,j);

            QDomElement distortionMatrix=document.createElement("Value");
            distortionMatrix.setAttribute("["+QString::number(i)+"]["+QString::number(j)+"]",values);
            distMatrixTag.appendChild(distortionMatrix);
        }

    }

     calibrationMatrixValues(cameraMatrix,imageSize,apertureWidth,apertureHeight,fovx,fovy,focalLength,principalPoint,pasp);

        QDomElement fieldov=document.createElement("Field of View");
        fieldov.setAttribute(" fovx: ",fovx);
        fieldov.setAttribute(" fovy: ",fovy);
        fieldOfView.appendChild(fieldov);

        QDomElement error=document.createElement("Re-projection Error");
        error.setAttribute(" Value: ",meanError);
        reProjectionError.appendChild(error);


    //Write to file

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text ))
    {

        return false;

    }
    else
    {
        QTextStream stream(&file);
        stream <<document.toString();

        file.close();

        return true;
    }

}
