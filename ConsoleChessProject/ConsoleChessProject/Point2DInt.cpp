#include <string>
#include "Point2DInt.hpp"
#include "HelperFunctions.hpp"

namespace Utils
{
	Point2DInt::Point2DInt()
		:_x(0), _y(0), x(_x), y(_y) {}

	Point2DInt::Point2DInt(int xPos, int yPos)
		: _x(xPos), _y(yPos), x(_x), y(_y) {}

	Point2DInt::Point2DInt(const Point2DInt& copyPos)
		: _x(copyPos.x), _y(copyPos.y), x(_x), y(_y) {}


	std::string Point2DInt::ToString() const
	{
		std::string str = std::format("({},{})", x, y);
		return str;
	}

	Point2DInt Point2DInt::operator+(const Point2DInt& otherPos) const
	{
		return { x + otherPos.x, y + otherPos.y };
	}

	Point2DInt Point2DInt::operator-(const Point2DInt& otherPos) const
	{
		return { x - otherPos.x, y - otherPos.y };
	}

	Point2DInt Point2DInt::operator*(const Point2DInt& otherPos) const
	{
		return { x * otherPos.x, y * otherPos.y };
	}

	Point2DInt Point2DInt::operator*(const int factor) const
	{
		return { x * factor, y * factor };
	}

	Point2DInt Point2DInt::operator/(const Point2DInt& otherPos) const
	{
		if (otherPos.x == 0 || otherPos.y == 0)
		{
			std::string message = std::format("Tried to divide Position {} "
				"by a pointer with 0 {}", ToString(), otherPos.ToString());
			Utils::Log(Utils::LogType::Error, message);
			return {};
		}

		return { x / otherPos.x, y / otherPos.y };
	}

	Point2DInt Point2DInt::operator/(const int factor) const
	{
		if (factor == 0)
		{
			std::string message = std::format("Tried to divide Position {} by factor of 0", ToString());
			Utils::Log(Utils::LogType::Error, message);
			return {};
		}

		return { x / factor, y / factor };
	}

	bool Point2DInt::operator==(const Point2DInt& otherPos) const
	{
		bool sameX = Utils::ApproximateEquals(x, otherPos.x);
		bool sameY = Utils::ApproximateEquals(y, otherPos.y);
		return sameX && sameY;
	}

	Point2DInt& Point2DInt::operator=(const Point2DInt& newPos)
	{
		if (this != &newPos)
		{
			_x = newPos.x;
			_y = newPos.y;
		}
		else return *this;
	}
}
