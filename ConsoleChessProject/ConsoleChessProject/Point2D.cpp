#include <format>
#include <string>
#include "Point2D.hpp"
#include "HelperFunctions.hpp"

namespace Utils
{
	Point2D::Point2D()
		:_x(0), _y(0), x(_x), y(_y) {}

	Point2D::Point2D(double xPos, double yPos)
		: _x(xPos), _y(yPos), x(_x), y(_y) {}

	Point2D::Point2D(const Point2D& copyPos)
		: _x(copyPos.x), _y(copyPos.y), x(_x), y(_y) {}


	std::string Point2D::ToString() const
	{
		std::string str = std::format("({},{})", x, y);
		return str;
	}

	Point2D Point2D::operator+(const Point2D& otherPos) const
	{
		return { x + otherPos.x, y + otherPos.y };
	}

	Point2D Point2D::operator-(const Point2D& otherPos) const
	{
		return { x - otherPos.x, y - otherPos.y };
	}

	Point2D Point2D::operator*(const Point2D& otherPos) const
	{
		return { x * otherPos.x, y * otherPos.y };
	}

	Point2D Point2D::operator*(const double factor) const
	{
		return { x * factor, y * factor };
	}

	Point2D Point2D::operator/(const Point2D& otherPos) const
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

	Point2D Point2D::operator/(const double factor) const
	{
		if (factor == 0)
		{
			std::string message = std::format("Tried to divide Position {} by factor of 0", ToString());
			Utils::Log(Utils::LogType::Error, message);
			return {};
		}

		return { x / factor, y / factor };
	}

	bool Point2D::operator==(const Point2D& otherPos) const
	{
		bool sameX = Utils::ApproximateEquals(x, otherPos.x);
		bool sameY = Utils::ApproximateEquals(y, otherPos.y);
		return sameX && sameY;
	}

	Point2D& Point2D::operator=(const Point2D& newPos)
	{
		if (this != &newPos)
		{
			_x = newPos.x;
			_y = newPos.y;
		}
		else return *this;
	}
}