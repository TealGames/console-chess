#include <variant>
#include <optional>
#include <vector>
#include <format>
#include "JSONUtils.hpp"
#include "HelperFunctions.hpp"

namespace Utils
{
	namespace JSON
	{
		JSONProperty::JSONProperty(const std::string& name, const JSONSingleType& type) :
			PROPERTY_NAME(name), PROPERTY_VALUE({ type }), IS_LIST(false) {}

		JSONProperty::JSONProperty(const std::string& name, const JSONList& type):
			PROPERTY_NAME(name), PROPERTY_VALUE(type), IS_LIST(true) {}

		const std::optional<JSONSingleType> JSONProperty::TryGetSingleType() const
		{
			if (IS_LIST || PROPERTY_VALUE.size()==0) return std::nullopt;
			else return PROPERTY_VALUE[0];
		}

		std::string JSONProperty::ToString() const
		{
			std::string result;
			result += "(" + PROPERTY_NAME + ",";

			if (IS_LIST)
			{
				result += JSONListToString(PROPERTY_VALUE);
			}
			else if (std::optional<JSONSingleType> val= TryGetSingleType() && val.has_value())
			{
				result+=JSONSingleTypeToString(val.value());
			}
			else
			{
				std::string err = std::format("Tried to convert JSON property {} "
					"but does not match any defined type", PROPERTY_NAME);
				Utils::Log(Utils::LogType::Error, err);
				return "";
			}
			result += ")";
			return result;
		}
	}
}