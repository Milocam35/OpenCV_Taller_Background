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
    resultado = cv::Mat::zeros(prevGray.rows, prevGray.cols, CV_8UC1);

    while(true){
        cap >> frame;
        frame = conversion_gray(frame);
        diff = cv::Mat(frame.rows, frame.cols, CV_8UC1);
        
        double threshold_value = 0.05;
        for(int i = 0; i < frame.rows; i++){
            for(int j = 0; j < frame.cols; j++){
                frame.at<uchar>(i, j) = frame.at<uchar>(i, j)*threshold_value;
                prevGray.at<uchar>(i, j) = prevGray.at<uchar>(i, j)*(1 - threshold_value);
                diff.at<uchar>(i, j) = frame.at<uchar>(i, j) + prevGray.at<uchar>(i, j);
            }
        }
        imshow("Diferencia", diff);
        resultado.setTo(0); // Asegura que la imagen resultado esté completamente en negro antes de actualizarla
        for(int i = 0; i < frame.rows; i++){
            for(int j = 0; j < frame.cols; j++){
                resultado.at<uchar>(i, j) = abs(diff.at<uchar>(i, j) - frame.at<uchar>(i, j));
            }
        }
        prevGray = resultado.clone();
        imshow("Resultado", resultado);
        if (waitKey(30) >= 0) break;
    }
    cv::destroyAllWindows();

    return 0;
}
