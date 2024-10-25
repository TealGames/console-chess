#include "HelperFunctions.hpp"
#include "JSONUtils.hpp"
#include <optional>

namespace Utils
{
	namespace JSON
	{
		std::optional<JSONObject> TryGetJSONFromFile(const std::filesystem::path& path)
		{
			const std::filesystem::path cleanedPath = CleanPath(path);
			std::string extension = cleanedPath.extension().string();
			if (extension.size() == 0)
			{
				std::string err = std::format("Tried to get json at path {} but the file extension is empty", cleanedPath);
				Log(LogType::Error, err);
				return std::nullopt;
			}

			if (extension.substr(0, 1) == ".")
				extension = extension.substr(1);

			if (extension != "json")
			{
				std::string err = std::format("Tried to get json at path {} but the "
					"file extension is not JSON but {}", cleanedPath, extension);
				Log(LogType::Error, err);
				return std::nullopt;
			}

			std::string content = ReadFile(cleanedPath);
			if (content.size() == 0) return std::nullopt;

			JSONObject obj(content);
			return obj;
		}
	}
}
