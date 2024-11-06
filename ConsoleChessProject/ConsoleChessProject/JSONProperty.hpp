#pragma once
#include "JSONUtils.hpp"

namespace Utils
{
	namespace JSON
	{
		class JSONProperty
		{
			friend class JSONObject;

		public:
			const std::string PROPERTY_NAME;
			const JSONType PROPERTY_VALUE;

			JSONProperty(const std::string& name, const JSONType& type);
			JSONProperty(const JSONProperty& other) = default;
			JSONProperty(JSONProperty&& other) noexcept = default;
			JSONProperty& operator=(const JSONProperty& other) = default;
			JSONProperty& operator=(JSONProperty&& other) noexcept = default;
			~JSONProperty() = default;

		private:
			template<typename T>
			const std::optional<T> TryGetType() const
			{
				const T value = std::get_if<T>(&PROPERTY_VALUE);
				if (value != nullptr) return value;
				else return std::nullopt;
			}

		public:
			const std::optional<int> TryGetInt() const;
			const std::optional<double> TryGetDouble() const;
			const std::optional<std::string> TryGetString() const;
			const std::optional<JSONObject> TryGetObject() const;
			const std::optional<JSONList> TryGetList() const;

			std::string ToString() const;
		};
	}
}