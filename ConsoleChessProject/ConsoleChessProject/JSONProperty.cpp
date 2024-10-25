#include "JSONUtils.hpp"
#include "HelperFunctions.hpp"

namespace Utils
{
	namespace JSON
	{
		JSONProperty::JSONProperty(const std::string& name, const JSONType& type) :
			PROPERTY_NAME(name), PROPERTY_VALUE(type) {}

		const std::optional<int> JSONProperty::TryGetInt()
		{
			return TryGetType<int>();
		}

		const std::optional<double> JSONProperty::TryGetDouble()
		{
			return TryGetType<double>();
		}

		const std::optional<std::string> JSONProperty::TryGetString()
		{
			return TryGetType<std::string>();
		}

		const std::optional<JSONObject> JSONProperty::TryGetObject()
		{
			return TryGetType<JSONObject>();
		}
	}
}