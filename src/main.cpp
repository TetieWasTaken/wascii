#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdlib.h>

using namespace std;
using namespace cv;

char intensityToChar(int intensity)
{
    const string CHARSET = " `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";
    int index = (int)(CHARSET.length() * (intensity) / 255);
    return CHARSET[index];
}

int main(int argc, char **argv)
{
    VideoCapture cap(0);
    if (!cap.isOpened())
    {
        cout << "Error opening video stream or file" << endl;
        return -1;
    }

    while (1)
    {
        Mat frame;
        cap >> frame;

        flip(frame, frame, 1);

        cvtColor(frame, frame, COLOR_BGR2GRAY);

        resize(frame, frame, Size(160, 120));

        string ascii = "";
        for (int i = 0; i < frame.rows; i++)
        {
            for (int j = 0; j < frame.cols; j++)
            {
                ascii += intensityToChar(frame.at<uchar>(i, j));
            }
            ascii += "\n";
        }

        printf("\033[2J\033[1;1H");
        cout << ascii << endl;

        char c = (char)waitKey(25);
        if (c == 27)
            break;
    }

    cap.release();

    return 0;
}