#include "opencvfacerecognition.h"
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2\imgproc\types_c.h>
#include <QImage>
#include <QtDebug>

using namespace std;
using namespace cv;

OpenCVfaceRecognition::OpenCVfaceRecognition(QObject *parent) : QThread(parent){}

OpenCVfaceRecognition::~OpenCVfaceRecognition() {
    requestInterruption();
    wait();
}

void OpenCVfaceRecognition::run()
{
    Mat src;
    Mat gray;
    Mat dst;

    CascadeClassifier c;

    VideoCapture v(0);

    if (!v.isOpened()) {
        return;
    }

    if(!c.load(FRONTAL_FACE_FILE))
    {
        qDebug()<<"Load haarcascade_frontalface_alt2 fail!";
        return;
    }

    vector<Rect> faces;


    while(v.read(src))
    {
        flip(src, src, 1);
        cvtColor(src, gray, CV_BGR2GRAY);
        equalizeHist(gray, dst);
        c.detectMultiScale(dst, faces);

        for(std::vector<cv::Rect>::size_type i=0; i<faces.size(); i++)
        {
            rectangle(src, faces[i], Scalar(0,0,255), 2);
        }

        QImage qimg = Mat2QImage(src);
        emit frameProcessed(0x1,qimg);
        QThread::msleep(30);

        //imshow("Test1", src);
        //imshow("Test2", gray);
        //imshow("Test3", dst);

    }
}

QImage OpenCVfaceRecognition::Mat2QImage(Mat cvImg)
{
    QImage qImg;
    if(cvImg.channels()==3)     //3 channels color image
    {

        cv::cvtColor(cvImg,cvImg,CV_BGR2RGB);
        qImg =QImage((const unsigned char*)(cvImg.data),
                      cvImg.cols, cvImg.rows,
                      cvImg.cols*cvImg.channels(),
                      QImage::Format_RGB888);
    }
    else if(cvImg.channels()==1)                    //grayscale image
    {
        qImg =QImage((const unsigned char*)(cvImg.data),
                      cvImg.cols,cvImg.rows,
                      cvImg.cols*cvImg.channels(),
                      QImage::Format_Indexed8);
    }
    else
    {
        qImg =QImage((const unsigned char*)(cvImg.data),
                      cvImg.cols,cvImg.rows,
                      cvImg.cols*cvImg.channels(),
                      QImage::Format_RGB888);
    }
    return qImg;
}
