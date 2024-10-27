#pragma once

#include <vector>
#include "HelperFunctions.hpp"
#include <string>
#include <type_traits>
#include <cassert>
#include <unordered_map>

namespace Utils
{
    template <typename T>
    class StringableEnum
    {
    private:
        const std::unordered_map<std::string, int>& _vals;

    public:
        const std::type_info typeName;
        StringableEnum(const std::unordered_map<std::string, int> vals) 
            : typeName(typeid(T)), _vals(vals)
        {
            const bool isEnum = std::is_enum<T>::value;
            if (!isEnum)
            {
                Utils::Log(Utils::LogType::Error, "Tried to create stringable enum from non enum type");
                return;
            }
        }

        bool HasVal(const std::string& str) const
        {
            auto it = _vals.find(str);
            return it != _vals.end();
        }

        bool HasNumber(const int number) const
        {
            for (const auto& pair : _vals)
            {
                if (pair.second == number)
                    return true;
            }
            return false;
        }

        std::string GetValAtIndex(const int index) const
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

        int GetIndexFromVal(const std::string& value) const
        {
            auto it = _vals.find(value);
            if (it == _vals.end())
                return -1;

            int index = std::distance(_vals.begin(), it);
            return index;
        }

        std::string GetValFromNumber(const int number) const
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

        int GetNumberForVal(const std::string&) const
        {
            auto it = _vals.find(value);
            if (it == _vals.end())
                return 0;

            return (*it).second;
        }

        std::string ToString() const
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

        explicit operator std::string()
        {
            return ToString();
        }
    };
}