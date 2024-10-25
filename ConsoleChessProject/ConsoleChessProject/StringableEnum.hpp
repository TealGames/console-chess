#pragma once

#include <vector>
#include <string>
#include <unordered_map>

namespace Utils
{
    class StringableEnum
    {
    private:
        const std::unordered_map<std::string, int>& _vals;

    public:
        const std::string& typeName;
        StringableEnum(std::string, const std::unordered_map<std::string, int>);

        bool HasVal(const std::string&) const;
        bool HasNumber(const int) const;

        std::string GetValAtIndex(const int) const;
        int GetIndexFromVal(const std::string&) const;

        std::string GetValFromNumber(const int) const;
        int GetNumberForVal(const std::string&) const;

        std::string ToString() const;
        explicit operator std::string();
    };
}