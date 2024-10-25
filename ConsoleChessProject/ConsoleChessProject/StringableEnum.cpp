#include <string>
#include <vector>
#include <unordered_map>
#include "StringableEnum.hpp"
#include "HelperFunctions.hpp"

namespace Utils
{
    StringableEnum::StringableEnum(std::string type, const std::unordered_map<std::string, int> vals)
        : typeName(typeName), _vals(vals)
    {
        if (_vals.size() == 0)
        {
            Log(LogType::Error, "Tried to create a stringable enum with size 0!");
        }
    }

    bool StringableEnum::HasVal(const std::string& str) const
    {
        auto it = _vals.find(str);
        return it != _vals.end();
    }

    bool StringableEnum::HasNumber(const int number) const
    {
        for (const auto& pair : _vals)
        {
            if (pair.second == number)
                return true;
        }
        return false;
    }

    std::string StringableEnum::GetValAtIndex(const int index) const
    {
        int maxIndex = _vals.size() - 1;
        if (index < 0 || index > maxIndex)
        {
            std::string message = "Tried to get value at index " + std::to_string(index) +
                " but it is out of range of the collection [0, " +
                std::to_string(maxIndex) + "]";
            Log(LogType::Error, message);
        }

        auto it = _vals.begin();
        std::advance(it, index);
        return (*it).first;
    }

    int StringableEnum::GetIndexFromVal(const std::string& value) const
    {
        auto it = _vals.find(value);
        if (it == _vals.end())
            return -1;

        int index = std::distance(_vals.begin(), it);
        return index;
    }

    std::string StringableEnum::GetValFromNumber(const int number) const
    {
        if (!HasNumber(number))
            return "";

        for (const auto& pair : _vals)
        {
            if (pair.second == number)
                return pair.first;
        }
        return "";
    }

    int StringableEnum::GetNumberForVal(const std::string& value) const
    {
        auto it = _vals.find(value);
        if (it == _vals.end())
            return 0;

        return (*it).second;
    }

    std::string StringableEnum::ToString() const
    {
        if (_vals.size() == 0)
        {
            return "[]";
        }

        std::string str = "[";
        int index = 0;
        for (const auto& pair : _vals)
        {
            str += "(" + pair.first + "," + std::to_string(pair.second) + ")";
            if (index < _vals.size() - 1)
                str += ", ";
            index++;
        }
        str += "]";
        return str;
    }

    StringableEnum::operator std::string()
    {
        return ToString();
    }
}