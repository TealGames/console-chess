#pragma once
#include <string>
#include <format>
#include <sstream>
#include <optional>
#include <cstdint>
#include <filesystem>
#include <iostream>

namespace Utils
{
	/// <summary>
	/// A more versatile version of to_string meant to handle not just numbers
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	template <typename T>
	std::string ToString(const T& obj)
	{
		std::ostringstream oss;
		oss << obj;
		return oss.str();
	}

	enum class LogType
	{
		Error,
		Warning,
		Log,
	};

	void Log(const LogType& logType, const std::string& str);

	template <typename T>
	void Log(const LogType& logType, const T& obj)
	{
		std::string objAsStr = Utils::ToString(obj);
		Log(logType, objAsStr);
	}

	template <typename T>
	std::string ToStringIterable(const T& collection)
	{
		std::string str = "[";
		int index = 0;
		for (const auto& element : collection)
		{
			str += Utils::ToString(element);
			if (index < collection.size() - 1)
				str += ", ";
			index++;
		}
		str += "]";
		return str;
	}

	template <typename T1, typename T2>
	bool IterableHas(const T1& collection, const T2& findElement)
	{
		if (T1.size() <= 0) return false;

		for (const auto& element : collection)
		{
			if (&element == &findElement || element == findElement)
			{
				return true;
			}
		}
		return false;
	}

	double ToRadians(const double);
	double ToDegrees(const double);
	bool ApproximateEquals(double, double);

	bool IsInifinity(double);
	bool IsNegInifinity(double);

	template<typename T>
	static std::optional<T> TryParse(const std::string& str)
	{
		T parsedVal;
		type_info tType = typeid(T);

		if (tType == typeid(unsigned int) || tType == typeid(uint16_t))
		{
			std::string error = std::format("Tried to parse string {} to unsigned int or uint16_t, "
				"which is not supported in C++ STL (there is no stoui function)", str);
			Utils::Log(Utils::LogType::Error, error);
			return std::nullopt;
		}
		else if (tType == typeid(long))
		{
			std::string error = std::format("Tried to parse string {} to long, "
				"which is not supported since most systems only guarantee 64 "
				"bits with long long (use long long or uint64_t instead)", str);
			Utils::Log(Utils::LogType::Error, error);
			return std::nullopt;
		}
		else if (tType == typeid(unsigned long) || tType == typeid(unsigned long long))
		{
			std::string error = std::format("Tried to parse string {} to unsigned long or "
				"unsigned long long, which is not supported since uint64_t and uint128_t "
				"offer better platform independence", str);
			Utils::Log(Utils::LogType::Error, error);
			return std::nullopt;
		}

		try
		{
			if (tType == typeid(int)) parsedVal = std::stoi(str);
			else if (tType == typeid(long long)) parsedVal = std::stoll(str);
			else if (tType == typeid(long double)) parsedVal = std::stold(str);
			else if (tType == typeid(float)) parsedVal = std::stof(str);
			else if (tType == typeid(double)) parsedVal = std::stod(str);
			else if (tType == typeid(long double)) parsedVal = std::stold(str);
			else
			{
				std::string error = std::format("Tried to parse string {} to type {} that can not "
					"be converted to string", str, tType.name());
				Utils::Log(Utils::LogType::Error, error);
				return std::nullopt;
			}
		}
		catch (const std::invalid_argument& e)
		{
			return std::nullopt;
		}
		catch (const std::out_of_range& e)
		{
			return std::nullopt;
		}
		catch (...)
		{
			std::string err = std::format("Tried to parse string {} to int but encountered unknwon error", str);
			Utils::Log(Utils::LogType::Error, err);
			return std::nullopt;
		}
		return parsedVal;
	}

	static std::filesystem::path CleanPath(const std::filesystem::path& path);
	static bool DoesPathExist(const std::filesystem::path& path);

	std::string ReadFile(const std::filesystem::path& path);
	void WriteFile(const std::filesystem::path&, const std::string content);
}