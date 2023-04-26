#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>

using namespace std;
using namespace cv;

#define clock chrono::steady_clock

string type2str(int type)
{
    string r;

    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);

    switch (depth)
    {
    case CV_8U:
        r = "8U";
        break;
    case CV_8S:
        r = "8S";
        break;
    case CV_16U:
        r = "16U";
        break;
    case CV_16S:
        r = "16S";
        break;
    case CV_32S:
        r = "32S";
        break;
    case CV_32F:
        r = "32F";
        break;
    case CV_64F:
        r = "64F";
        break;
    default:
        r = "User";
        break;
    }

    r += "C";
    r += (chans + '0');

    return r;
}

string logInfo(clock::time_point started_at, Mat frame)
{
    string matrix = "\033[1;34m" + type2str(frame.type()) + "\033[22;32;3m " + to_string(frame.cols) + "\033[39mx\033[32m" + to_string(frame.rows) + "\033[0m";
    string seconds = "\033[1;34m" + to_string(chrono::duration_cast<chrono::seconds>(clock::now() - started_at).count()) + "\033[0m";

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
        return "Free memory: " + free_memory_color + to_string(free_memory / 1024 / 1024) + "\033[0m MB         Used memory: " + used_memory_color + to_string(used_memory / 1024 / 1024) + "\033[0m MB         " + seconds + "s elapsed         Matrix: " + matrix;
    }

    return "Matrix: " + matrix + "         " + seconds + "s elapsed\n\033[1;4;31mWarning\033[24m: Could not get free memory\033[0m";
}