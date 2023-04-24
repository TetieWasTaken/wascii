#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdlib.h>

using namespace std;
using namespace cv;

string toLower(string input)
{
    string output = "";
    for (int i = 0; i < input.length(); i++)
    {
        output += tolower(input[i]);
    }
    return output;
}

bool validateInput(string input)
{
    if (input.empty() || toLower(input) == string("n") || toLower(input) == string("y"))
    {
        return true;
    }
    return false;
}

/* char intensityToChar(int intensity)
{
    const string CHARSET = "@#MW%*oahkbdpwmZO0QYPXzvxnrt|(){}[]/?-+~^<>i!lI;:,\"^`'. ";
    int index = (int)(CHARSET.length() * (intensity) / 255);
    return CHARSET[index];
} */

string colorToChar(Vec3b color, bool withColor)
{
    const string CHARSET = "@#MW%*oahkbdpwmZO0QYPXzvxnrt|(){}[]/?-+~^<>i!lI;:,\"^`'. ";
    int intensity = (color[0] + color[1] + color[2]) / 3;
    int index = (int)(CHARSET.length() * (intensity) / 255);
    if (withColor)
    {
        return "\033[38;2;" + to_string(color[2]) + ";" + to_string(color[1]) + ";" + to_string(color[0]) + "m" + CHARSET[index] + "\033[0m";
    }

    return string(1, CHARSET[index]);
}

int main(int argc, char **argv)
{
    bool withColor = false;
    string userInput;

    // Prompt the user for color or grayscale
    cout << "Do you want color? (y/N): ";
    getline(cin, userInput);

    if (validateInput(userInput))
    {
        withColor = toLower(userInput) == string("y");
    }
    else
    {
        cout << "Input must be one of: y, n" << endl;
        return -1;
    }

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
                ascii += colorToChar(frame.at<Vec3b>(i, j), withColor);
            }
            ascii += "\n";
        }

        system("clear");
        cout << ascii << endl;

        char c = (char)waitKey(25);
        if (c == 27)
            break;
    }

    cap.release();

    return 0;
}