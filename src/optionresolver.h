#include "errorcodes.h"

using namespace std;

enum paramType
{
    INT,
    STRING,
    BOOL
};

class OptionResolver
{
public:
    int getIntParam(string message, int defaultValue);
    string getStringParam(string message, string defaultValue);
    bool getBoolParam(string message, bool defaultValue);
    template <typename T>
    T getParam(const string &message, T defaultValue, const string &validValues = "")
    {
        string valueStr;
        cout << message << " (" << defaultValue << "): ";
        getline(cin, valueStr);
        if (valueStr == "")
        {
            return defaultValue;
        }

        if (!validValues.empty())
        {
            if (validValues.find(valueStr) == string::npos)
            {
                throw InvalidationError(InvalidArgument, "Invalid argument", valueStr);
            }
        }

        try
        {
            return boost::lexical_cast<T>(valueStr);
        }
        catch (boost::bad_lexical_cast &e)
        {
            throw InvalidationError(InvalidArgument, "Invalid argument", valueStr);
        }
    }

private:
    bool _validateParam(string value, paramType type);
};

bool OptionResolver::_validateParam(string value, paramType type)
{
    if (type == INT)
    {
        for (int i = 0; i < value.length(); i++)
        {
            if (!isdigit(value[i]))
            {
                throw InvalidationError(InvalidArgument, "Invalid argument", value);
            }
        }
        return true;
    }
    else if (type == STRING)
    {
        return true;
    }
    else if (type == BOOL)
    {
        if ((value != "y") && (value != "Y") && (value != "n") && (value != "N"))
            throw InvalidationError(InvalidArgument, "Invalid argument", value);
        return true;
    }
    throw InvalidationError(InvalidArgument, "Invalid argument", value);
}

int OptionResolver::getIntParam(string message, int defaultValue)
{
    string valueStr;
    cout << message << " (" << defaultValue << "): ";
    getline(cin, valueStr);
    if (valueStr == "")
    {
        return defaultValue;
    }
    _validateParam(valueStr, INT);
    return stoi(valueStr);
}

string OptionResolver::getStringParam(string message, string defaultValue)
{
    string valueStr;
    cout << message << " (" << defaultValue << "): ";
    getline(cin, valueStr);
    if (valueStr == "")
    {
        return defaultValue;
    }
    _validateParam(valueStr, STRING);
    return valueStr;
}

bool OptionResolver::getBoolParam(string message, bool defaultValue)
{
    string valueStr;
    cout << message << " (" << (defaultValue ? "Y/n" : "y/N") << "): ";
    getline(cin, valueStr);
    if (valueStr == "")
    {
        return defaultValue;
    }
    _validateParam(valueStr, BOOL);
    return valueStr == "y" || valueStr == "Y";
}