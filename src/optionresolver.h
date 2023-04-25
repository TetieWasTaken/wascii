#include "errorcodes.h"

class OptionResolver
{
public:
    template <typename T>
    T getParam(const std::string &message, T defaultValue, const std::string &validValues = "")
    {
        std::string valueStr;
        if constexpr (std::is_same_v<T, bool>)
        {
            std::cout << message << " (" << (defaultValue ? "\033[1;32mY\033[0m/\033[1;31mn\033[0m" : "\033[1;32my\033[0m/\033[1;31mN\033[0m") << "): ";
        }
        else
        {
            std::cout << message << " (\033[1;37m" << defaultValue << "\033[0m): ";
        }
        std::getline(std::cin, valueStr);
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
            if constexpr (std::is_same_v<T, bool>)
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
            else if constexpr (std::is_same_v<T, std::string>)
            {
                return valueStr;
            }
            else
            {
                return std::stoi(valueStr);
            }
        }
        catch (std::invalid_argument &e)
        {
            throw InvalidationError(InvalidArgument, "Invalid argument", valueStr);
        }
    }
};
