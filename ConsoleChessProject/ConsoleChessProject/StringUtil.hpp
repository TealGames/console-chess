#pragma once
#include <string>

namespace Utils {
	class StringUtil {
	private:
		std::string str;
	public:
		StringUtil(const std::string&);
		StringUtil& trim();
		StringUtil& to_lower_case();

		std::string ToString();
		explicit operator std::string();
	};
}