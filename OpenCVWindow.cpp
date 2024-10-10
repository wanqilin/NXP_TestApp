#include "OpenCVWindow.h"
#include <QtDebug>
#include <opencv2/opencv.hpp>
#include <opencv2\imgproc\types_c.h>
#include <QImage>

using namespace std;
using namespace cv;

OpenCVWindow::OpenCVWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qDebug()<<"IntoOpenCVWindow!";
    setWindowTitle(tr("OpenCV"));
    resize(500, 600);

    OpencvImage = new QLabel("ImageDisplay",this);
    OpencvImage->setFixedSize(120,140);
    OpencvImage->setScaledContents(true);

    string imgPath = "D:\\wql\\openCV\\testPIC\\OpenCV_alpha_1.png";
    Mat src = imread(imgPath, IMREAD_UNCHANGED);
    QImage qimg = Mat2QImage(src);
    OpencvImage->setPixmap(QPixmap::fromImage(qimg));


    //imshow("test",src);

}

OpenCVWindow::~OpenCVWindow() {}


QImage OpenCVWindow::Mat2QImage(Mat cvImg)
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
