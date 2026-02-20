#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
// Declaraciones de funciones
cv::Mat conversion_gray(cv::Mat frame);
cv::Mat bgr_a_rgb(cv::Mat frame);

cv::Mat bgr_a_rgb(cv::Mat frame)
{
    cv::Mat resultado = frame.clone();

    for (int i = 0; i < frame.rows; i++)
    {
        for (int j = 0; j < frame.cols; j++)
        {
            resultado.at<cv::Vec3b>(i, j)[0] = frame.at<cv::Vec3b>(i, j)[2];
            resultado.at<cv::Vec3b>(i, j)[2] = frame.at<cv::Vec3b>(i, j)[0];
        }
    }
    return resultado;
}

// Implementacion de funciones
cv::Mat conversion_gray(cv::Mat frame)
{
    cv::Mat resultado(frame.rows, frame.cols, CV_8UC1);

    for (int i = 0; i < frame.rows; i++)
    {
        for (int j = 0; j < frame.cols; j++)
        {
            cv::Vec3b pixel = frame.at<cv::Vec3b>(i, j);
            uchar gray = (pixel[0] + pixel[1] + pixel[2]) / 3;
            resultado.at<uchar>(i, j) = gray;
        }
    }
    return resultado;
}

int main()
{
    cv::VideoCapture cap(0);


    if (!cap.isOpened())
    {
        cerr << "Error: no se pudo abrir la camara" << endl;
        return -1;
    }

    Mat frame, prevFrame, grayFrame, prevGray, diff, resultado;

    cap >> prevFrame;
    prevGray = conversion_gray(prevFrame);
    diff = cv::Mat(prevGray.rows, prevGray.cols, CV_8UC1);
    resultado = cv::Mat(prevGray.rows, prevGray.cols, CV_8UC1);

    while(true){

        cap >> frame;
        if (frame.empty()) break;
        grayFrame = conversion_gray(frame);

        double threshold_value = 0.1;
        for(int i = 0; i < grayFrame.rows; i++){
            for(int j = 0; j < grayFrame.cols; j++){
                // Media exponencial del fondo: fondo = alpha*actual + (1-alpha)*fondo_anterior
                diff.at<uchar>(i, j) = (uchar)(grayFrame.at<uchar>(i, j) * threshold_value
                                              + prevGray.at<uchar>(i, j) * (1 - threshold_value));
                // Deteccion: diferencia absoluta entre frame actual y modelo de fondo
                resultado.at<uchar>(i, j) = (uchar)std::abs((int)grayFrame.at<uchar>(i, j) - (int)diff.at<uchar>(i, j));
            }
        }
        prevGray = diff.clone();
        imshow("Fondo (EMA)", diff);
        imshow("Deteccion de movimiento", resultado);
        if (waitKey(30) >= 0) break;
    }
    cv::destroyAllWindows();

    return 0;
}
