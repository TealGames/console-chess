#include <format>
#include <string>
#include "Position2D.hpp"
#include "HelperFunctions.hpp"

namespace Utils
{
	Position2D::Position2D()
		:_x(0), _y(0), x(_x), y(_y) {}

	Position2D::Position2D(double xPos, double yPos)
		: _x(xPos), _y(y), x(_x), y(_y) {}

	Position2D::Position2D(const Position2D& copyPos)
		: _x(copyPos.x), _y(copyPos.y), x(_x), y(_y) {}


	std::string Position2D::ToString() const
	{
		std::string str = std::format("({},{})", x, y);
		return str;
	}

	Position2D Position2D::operator+(const Position2D& otherPos) const
	{
		Position2D result(x + otherPos.x, y + otherPos.y);
		return result;
	}

	Position2D Position2D::operator-(const Position2D& otherPos) const
	{
		Position2D result(x - otherPos.x, y - otherPos.y);
		return result;
	}

	Position2D Position2D::operator*(const Position2D& otherPos) const
	{
		Position2D result(x * otherPos.x, y * otherPos.y);
		return result;
	}

	Position2D Position2D::operator*(const double factor) const
	{
		Position2D result(x * factor, y * factor);
		return result;
	}

	Position2D Position2D::operator/(const Position2D& otherPos) const
	{
		if (otherPos.x == 0 || otherPos.y == 0)
		{
			std::string message = std::format("Tried to divide Position {} "
				"by a pointer with 0 {}", ToString(), otherPos.ToString());
			Utils::Log(Utils::LogType::Error, message);
			return {};
		}

		Position2D result(x / otherPos.x, y / otherPos.y);
		return result;
	}

	Position2D Position2D::operator/(const double factor) const
	{
		if (factor == 0)
		{
			std::string message = std::format("Tried to divide Position {} by factor of 0", ToString());
			Utils::Log(Utils::LogType::Error, message);
			return {};
		}

		Position2D result(x / factor, y / factor);
		return result;
	}

	bool Position2D::operator==(const Position2D& otherPos) const
	{
		bool sameX = Utils::ApproximateEquals(x, otherPos.x);
		bool sameY = Utils::ApproximateEquals(y, otherPos.y);
		return sameX && sameY;
	}

	Position2D Position2D::operator=(const Position2D& newPos)
	{
		if (this != &newPos)
		{
			_x = newPos.x;
			_y = newPos.y;
		}
		else return *this;
	}
}