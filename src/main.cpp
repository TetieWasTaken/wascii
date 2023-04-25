#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <mach/mach.h>
#include <chrono>
#include "errorcodes.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include "optionresolver.h"

using namespace std;
using namespace cv;


#define clock chrono::steady_clock

struct winsize w;

const string CHARSET = " `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";

char intensityToChar(int intensity)
{
    int index = (int)(CHARSET.length() * (intensity) / 255);
    return CHARSET[index];
}

string logInfo(clock::time_point started_at)
{
    mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
    vm_statistics_data_t vmstat;
    if (host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vmstat, &count) == KERN_SUCCESS)
    {
        long long free_memory = (int64_t)vmstat.free_count * (int64_t)vm_page_size;
        long long used_memory = ((int64_t)vmstat.active_count + (int64_t)vmstat.inactive_count + (int64_t)vmstat.wire_count) * (int64_t)vm_page_size;

        string free_memory_color = "\033[1;32m";
        string used_memory_color = "\033[1;31m";
        if (free_memory < 100000000)
        {
            free_memory_color = "\033[1;31m";
        }
        else if (free_memory < 200000000)
        {
            free_memory_color = "\033[1;33m";
        }
        return "Free memory: " + free_memory_color + to_string(free_memory / 1024 / 1024) + "\033[0m MB         Used memory: " + used_memory_color + to_string(used_memory / 1024 / 1024) + "\033[0m MB         \033[1;34m" + to_string(chrono::duration_cast<chrono::seconds>(clock::now() - started_at).count()) + "\033[0m seconds";
    }
    return "";
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
         << logInfo(started_at) << endl;

    OptionResolver resolver;

    bool useWebcam = resolver.getBoolParam("Use webcam", true);
    int xSize = resolver.getIntParam("X size", width);
    int ySize = resolver.getIntParam("Y size", height);

    if (useWebcam)
    {
        int timeBetweenFrames = resolver.getIntParam("Time between frames (ms)", 25);
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
                 << logInfo(started_at) << endl;

            char c = (char)waitKey(timeBetweenFrames);
            if (c == 27)
                return 0;
        }

        cap.release();

        return 0;
    }
    else
    {
        string path = resolver.getStringParam("Path to image", "./assets/logo.jpg");

        Mat frame = imread(path, IMREAD_GRAYSCALE);

        if (frame.empty())
        {
            cout << "Error opening image" << endl;
            return -1;
        }

        resize(frame, frame, Size(xSize, ySize));

        string ascii = frameToAscii(frame);

        printf("\033[2J\033[1;1H");
        cout << ascii << endl
             << logInfo(started_at) << endl;
    }
}