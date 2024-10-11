#ifndef OPENCVFACERECOGNITION_H
#define OPENCVFACERECOGNITION_H

#include <QObject>
#include <QThread>
#include <opencv2/core/core.hpp>

using namespace cv;

class OpenCVfaceRecognition : public QThread
{
    Q_OBJECT
public:
    #define FRONTAL_FACE_FILE "D:/work/OpenCV/opencv-4.5.1/QT-opencv-4.5.1/source/data/haarcascades/haarcascade_frontalface_alt2.xml"

    OpenCVfaceRecognition();
    ~OpenCVfaceRecognition();

signals:
    void frameProcessed(int id,const QImage &image);

private:
    QImage Mat2QImage(Mat cvImg);
    void run();
};

#endif // OPENCVFACERECOGNITION_H
