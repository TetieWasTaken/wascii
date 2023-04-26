#include "errorcodes.h"
#include <curses.h>

using namespace std;
using namespace cv;

#define UP 259
#define DOWN 258
#define ENTER 10

class OptionResolver
{
private:
    template <typename T>
    void _moveCursor(int oldPos, int newPos, const vector<T> &options)
    {
        move(oldPos + 1, 0);
        clrtoeol();
        move(oldPos + 1, 7);
        printw(options[oldPos].c_str());

        move(newPos + 1, 4);
        clrtoeol();
        attron(COLOR_PAIR(1));
        attron(A_BOLD);
        printw("-> ");
        attroff(A_BOLD);
        attroff(COLOR_PAIR(1));
        printw(options[newPos].c_str());
        move(newPos + 1, 0);
        refresh();
    }

public:
    template <typename T>
    T getParam(const string &message, T defaultValue, const string &validValues = "")
    {
        string valueStr;
        if constexpr (is_same_v<T, bool>)
        {
            cout << message << " (" << (defaultValue ? "\033[1;32mY\033[0m/\033[1;31mn\033[0m" : "\033[1;32my\033[0m/\033[1;31mN\033[0m") << "): ";
        }
        else
        {
            cout << message << " (\033[1;37m" << defaultValue << "\033[0m): ";
        }
        getline(cin, valueStr);
        if (valueStr == "")
        {
            return defaultValue;
        }

        if (!validValues.empty())
        {
            bool valid = false;
            for (const auto &validValue : validValues)
            {
                if (valueStr.size() == 1 && valueStr[0] == validValue)
                {
                    valid = true;
                    break;
                }
            }
            if (!valid)
            {
                throw InvalidationError(InvalidArgument, "Invalid argument", valueStr);
            }
        }

        try
        {
            if constexpr (is_same_v<T, bool>)
            {
                if (valueStr.size() == 1)
                {
                    if (valueStr[0] == 'y' || valueStr[0] == 'Y')
                    {
                        return true;
                    }
                    else if (valueStr[0] == 'n' || valueStr[0] == 'N')
                    {
                        return false;
                    }
                }
                throw InvalidationError(InvalidArgument, "Invalid argument", valueStr);
            }
            else if constexpr (is_same_v<T, string>)
            {
                return valueStr;
            }
            else
            {
                return stoi(valueStr);
            }
        }
        catch (invalid_argument &e)
        {
            throw InvalidationError(InvalidArgument, "Invalid argument", valueStr);
        }
    }

    template <typename T>
    T getDropdown(const string &message, const vector<T> &options)
    {
        initscr();
        start_color();
        cbreak();
        keypad(stdscr, TRUE);
        noecho();

        use_default_colors();

        init_pair(1, COLOR_GREEN, -1);
        init_pair(2, COLOR_RED, -1);

        attron(A_BOLD);
        printw("%s\n", message.c_str());
        attroff(A_BOLD);

        for (int i = 0; i < options.size(); i++)
        {

            attron(COLOR_PAIR(1));
            attron(A_BOLD);
            printw("    %s", (i == 0 ? "->" : "  "));
            attroff(A_BOLD);
            attroff(COLOR_PAIR(1));

            printw(" %s\n", options[i].c_str());
        }

        move(1, 0);

        refresh();

        int selectedIndex = 0;
        int ch;
        while (true)
        {
            ch = getch();

            if (ch == UP && selectedIndex > 0)
            {
                _moveCursor(selectedIndex, selectedIndex - 1, options);
                selectedIndex--;
            }
            else if (ch == DOWN && selectedIndex < options.size() - 1)
            {

                _moveCursor(selectedIndex, selectedIndex + 1, options);
                selectedIndex++;
            }
            else if (ch == ENTER || ch == '\n')
            {
                break;
            }
        }

        clear();
        endwin();

        return options[selectedIndex];
    };
};
