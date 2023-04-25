#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <mach/mach.h>
#include <chrono>

using namespace std;
using namespace cv;

*/

#define clock chrono::steady_clock
const string CHARSET = " `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";

char intensityToChar(int intensity)
{
    int index = (int)(CHARSET.length() * (intensity) / 255);
    return CHARSET[index];
}

int getParam(string message, int defaultValue)
{
    string valueStr;
    cout << message << " (" << defaultValue << "): ";
    getline(cin, valueStr);
    if (valueStr == "")
    {
        return defaultValue;
    }
    return stoi(valueStr);
}

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

int main(int argc, char **argv)
{
    VideoCapture cap(0);
    if (!cap.isOpened())
    {
        cout << "Error opening video stream or file" << endl;
        return -1;
    }

int main(int argc, char **argv)
{
    clock::time_point started_at = clock::now();

    int timeBetweenFrames = getParam("Time between frames (ms)", 25);
    int xSize = getParam("X size", 160);
    int ySize = getParam("Y size", 120);

    while (1)
    {
        Mat frame;
        cap >> frame;

        flip(frame, frame, 1);

        cvtColor(frame, frame, COLOR_BGR2GRAY);

        resize(frame, frame, Size(xSize, ySize));

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
        cout << ascii << endl
             << logInfo(started_at) << endl;

        char c = (char)waitKey(timeBetweenFrames);
        if (c == 27)
            return 0;
    }

    cap.release();

    return 0;
}