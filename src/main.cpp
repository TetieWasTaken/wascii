#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <mach/mach.h>
#include <chrono>
#include "errorcodes.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include "optionresolver.h"
#include "utils.h"
#include <filesystem>

using namespace std;
using namespace cv;
namespace fs = std::filesystem;


#define clock chrono::steady_clock

struct winsize w;

enum Charsets
{
    LONG,
    SHORT
};

const string LONG_CHARSET = " `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";
const string SHORT_CHARSET = " .:-=+*#%@";

string CHARSET = SHORT_CHARSET;

char intensityToChar(int intensity)
{
    int index = (int)(CHARSET.length() * (intensity) / 255);
    return CHARSET[index];
}

string frameToAscii(Mat frame)
{
    string ascii = "";
    for (int i = 0; i < frame.rows; i++)
    {
        for (int j = 0; j < frame.cols; j++)
        {
            ascii += intensityToChar(frame.at<uchar>(i, j));
        }
        ascii += "\n";
    }
    return ascii;
}

int main(int argc, char **argv)
{
    ioctl(0, TIOCGWINSZ, &w);
    int width = w.ws_col;
    int height = w.ws_row - 2; // reserve 2 lines for info

    clock::time_point started_at = clock::now();

    Mat frame = imread("./assets/logo.jpg", IMREAD_GRAYSCALE);
    resize(frame, frame, Size(width, height));

    cout << frameToAscii(frame) << endl
         << logInfo(started_at, frame) << endl;

    OptionResolver resolver;

    string selectedOption = resolver.getDropdown("Select configuration", vector<string>{"Webcam", "Image"});

    cout << "Configuration: " << selectedOption << endl;
    string charset = resolver.getDropdown("Select charset", vector<string>{"Short", "Long"});
    charset == "Long" ? CHARSET = LONG_CHARSET : CHARSET = SHORT_CHARSET;

    cout << "Charset: " << charset << endl;

    int xSize = resolver.getParam<int>("X size", width);
    int ySize = resolver.getParam<int>("Y size", height);

    if (selectedOption == "Webcam")
    {
        int timeBetweenFrames = resolver.getParam<int>("Time between frames (ms)", 25);
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

            resize(frame, frame, Size(xSize, ySize));

            string ascii = frameToAscii(frame);

            printf("\033[2J\033[1;1H");
            cout << ascii << endl
                 << logInfo(started_at, frame) << endl;

            char c = (char)waitKey(timeBetweenFrames);
            if (c == 27)
                return 0;
        }

        cap.release();

        return 0;
    }
    else if (selectedOption == "Image")
    {
        vector<string> files;
        for (const auto &entry : fs::directory_iterator("./assets"))
        {
            files.push_back(entry.path());
        }

        string path = resolver.getDropdown("Select image", files);

        if (path == "./assets/samples")
        {
            vector<string> files;
            for (const auto &entry : fs::directory_iterator(path))
            {
                files.push_back(entry.path());
            }

            path = resolver.getDropdown("Select image", files);
        }

        Mat frame = imread(path);

        if (frame.empty())
        {
            cout << "Error opening image" << endl;
            return -1;
        }

        cvtColor(frame, frame, COLOR_BGR2GRAY);

        resize(frame, frame, Size(xSize, ySize));

        string ascii = frameToAscii(frame);

        printf("\033[2J\033[1;1H");
        cout << ascii << endl
             << logInfo(started_at, frame) << endl;
    }
}