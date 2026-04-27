
#include "ImageProcessing.h"
#include "ColorSpaces.h"
#include "JPEG.h"

#include <cmath>

#include <QDebug>
#include <QString>
#include <QImage>

void imageProcessingFun(const QString& progName, QImage& outImgs, const QImage& inImgs, const QVector<double>& params)
{

	/* Create buffers for YUV image */

    int width = inImgs.width();
    int height = inImgs.height();

    uchar * Y_buff = new uchar[width * height];
    char* U_buff = new char[width*height/4];
    char* V_buff = new char[width*height/4];

	/* Create empty output image */
	outImgs = QImage(inImgs.width(), inImgs.height(), inImgs.format());

	/* Convert input image to YUV420 image */
    RGBtoYUV420(inImgs.bits(), inImgs.width(), inImgs.height(), Y_buff, U_buff, V_buff);

    if(progName == QString("JPEG Encoder"))
    {
        performJPEGEncoding(Y_buff, U_buff, V_buff, width, height, params[0]);
	}
    /* Convert YUV image back to RGB */
    YUV420toRGB(Y_buff, U_buff, V_buff, width, height, outImgs.bits());

    outImgs = QImage("example.jpg");

	/* Delete used memory buffers */
    delete[] Y_buff;
    delete[] U_buff;
    delete[] V_buff;
}

