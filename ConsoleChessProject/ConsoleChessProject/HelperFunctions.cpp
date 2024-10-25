#include <string>
#include <array>
#include <format>
#include <numbers>
#include <cmath>
#include <optional>
#include <limits>
#include <fstream>
#include <filesystem>
#include "HelperFunctions.hpp"
#include "StringUtil.hpp"

namespace Utils
{
	void Log(const LogType& logType, const std::string& str)
	{
		std::string logTypeMessage;
		switch (logType)
		{
		case LogType::Error:
			logTypeMessage = "ERROR";
			break;
		case LogType::Warning:
			logTypeMessage = "WARNING";
			break;
		case LogType::Log:
			logTypeMessage = "LOG";
			break;
		default:
			std::string errMessage = "Tried to log message of message type "
				"that is not defined: ";
			Log(LogType::Error, errMessage);
			return;
		}
		std::cout << "\n" + logTypeMessage + ": " << str << std::endl;
	}

	double ToRadians(const double deg)
	{
		return deg * (std::numbers::pi / 180.0);
	}

	double ToDegrees(const double rad)
	{
		return rad * (180.0 / std::numbers::pi);
	}

	/// <summary>
	/// To handle approximate equivalence between floating numbers
	/// </summary>
	/// <param name="d1"></param>
	/// <param name="d2"></param>
	/// <returns></returns>
	bool ApproximateEquals(double d1, double d2)
	{
		double diff = std::fabs(d2 - d1);
		return diff < std::numeric_limits<double>().epsilon();
	}

	bool IsInifinity(double value)
	{
		//Note: sign bit returns true if negative value
		return std::isinf(value) && !std::signbit(value);
	}

	bool IsNegInifinity(double value)
	{
		//Note: sign bit returns true if negative value
		return std::isinf(value) && std::signbit(value);
	}

	std::filesystem::path CleanPath(const std::filesystem::path& path)
	{
		Utils::StringUtil cleaner(path.string());
		const std::string cleaned = cleaner.trim().ToString();
		return cleaned;
	}

	bool DoesPathExist(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			std::string err = std::format("File path {} does not exist", path);
			Log(LogType::Error, err);
			return false;
		}
		return true;
	}

	std::string ReadFile(const std::filesystem::path& path)
	{
		const std::filesystem::path cleanedPath = CleanPath(path);
		if (!DoesPathExist(cleanedPath)) return "";

		std::string content;
		std::ifstream file(cleanedPath);
		if (!file.is_open())
		{
			std::string err = std::format("Tried to read file at path {} but it could not be opened", cleanedPath);
			Utils::Log(Utils::LogType::Error, err);
			return "";
		}

		std::string line;
		while (std::getline(file, line))
		{
			content += line;
		}
		file.close();
		return content;
	}

	void WriteFile(const std::filesystem::path& path, const std::string content)
	{
		const std::filesystem::path cleanedPath = CleanPath(path);
		if (!DoesPathExist(cleanedPath)) return;

		std::ofstream file(cleanedPath);
		if (!file.is_open())
		{
			std::string err = std::format("Tried to write {} to file at path {} "
				"but it could not be opened", content, cleanedPath);
			Utils::Log(Utils::LogType::Error, err);
			return;
		}

		file << content;
		file.close();
	}
}