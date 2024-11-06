#include <optional>
#include <vector>
#include "HelperFunctions.hpp"
#include "StringUtil.hpp"
#include "JSONUtils.hpp"

namespace Utils
{
	namespace JSON
	{
		std::string CleanJSON(const std::string& str)
		{
			std::string newStr = Utils::StringUtil(str).Trim().RemoveChar(' ').ToString();
			std::cout << str << newStr;
			return newStr;
		}

		std::optional<JSONObject> TryGetJSONFromFile(const std::filesystem::path& path)
		{
			const std::filesystem::path& cleanedPath = path;
			std::string extension = cleanedPath.extension().string();
			if (extension.size() == 0)
			{
				std::string err = std::format("Tried to get json at path {} but the file "
					"extension is empty", cleanedPath.string());
				Log(LogType::Error, err);
				return std::nullopt;
			}

			if (extension.substr(0, 1) == ".")
				extension = extension.substr(1);

			if (extension != JSON_EXTENSION)
			{
				std::string err = std::format("Tried to get json at path {} but the "
					"file extension is not JSON but {}", cleanedPath.string(), extension);
				Log(LogType::Error, err);
				return std::nullopt;
			}

			std::string content = ReadFile(cleanedPath);
			if (content.size() == 0) return std::nullopt;

			JSONObject obj(content);
			return obj;
		}

		std::string JSONSingleTypeToString(const JSONSingleType& type)
		{
			if (std::holds_alternative<std::string>(type))
				return std::get<std::string>(type);
			else if (std::holds_alternative<int>(type))
				return std::to_string(std::get<int>(type));
			else if (std::holds_alternative<double>(type))
				return std::to_string(std::get<double>(type));
			else if (std::holds_alternative<JSONObject>(type))
				return std::get<JSONObject>(type).ToString();
			else
			{
				std::string err = std::format("Tried to convert to a string single type "
					"but does not match defined type");
				Utils::Log(Utils::LogType::Error, err);
				return "";
			}
		}

		std::string JSONListToString(const JSONList& list)
		{
			if (list.size()==0)
			{
				std::string err = std::format("Tried to convert JSON list to a string "
					"but list has size of 0");
				Utils::Log(Utils::LogType::Error, err);
				return "";
			}

			std::string result = ToStringIterable<std::vector<JSONSingleType>, 
				JSONSingleType>(list, JSONSingleTypeToString);
		}

		inline std::optional<int> TryGetInt(const JSONObject& obj, const std::string& propertyName)
		{
			return obj.HasProperty<int>(propertyName);
		}

		inline std::optional<double> TryGetDouble(const JSONObject& obj, const std::string& propertyName)
		{
			return obj.HasProperty<double>(propertyName);
		}

		inline std::optional<std::string> TryGetString(const JSONObject& obj, const std::string& propertyName)
		{
			return obj.HasProperty<std::string>(propertyName);
		}

		inline std::optional<JSONObject> TryGetObject(const JSONObject& obj, const std::string& propertyName)
		{
			return obj.HasProperty<JSONObject>(propertyName);
		}

		inline std::optional<JSONList> TryGetList(const JSONObject& obj, const std::string& propertyName)
		{
			return obj.HasProperty<JSONList>(propertyName);
		}
	}
}
