#include <format>
#include <string>
#include "Point2D.hpp"
#include "HelperFunctions.hpp"

namespace Utils
{
	Point2D::Point2D()
		:m_x(0), m_y(0), m_X(m_x), m_Y(m_y) {}

	Point2D::Point2D(double xPos, double yPos)
		: m_x(xPos), m_y(yPos), m_X(m_x), m_Y(m_y) {}

	Point2D::Point2D(const Point2D& copyPos)
		: m_x(copyPos.m_X), m_y(copyPos.m_Y), m_X(m_x), m_Y(m_y) {}


	std::string Point2D::ToString() const
	{
		std::string str = std::format("({},{})", m_X, m_Y);
		return str;
	}

	Point2D Point2D::operator+(const Point2D& otherPos) const
	{
		return { m_X + otherPos.m_X, m_Y + otherPos.m_Y };
	}

	Point2D Point2D::operator-(const Point2D& otherPos) const
	{
		return { m_X - otherPos.m_X, m_Y - otherPos.m_Y };
	}

	Point2D Point2D::operator*(const Point2D& otherPos) const
	{
		return { m_X * otherPos.m_X, m_Y * otherPos.m_Y };
	}

	Point2D Point2D::operator*(const double factor) const
	{
		return { m_X * factor, m_Y * factor };
	}

	Point2D Point2D::operator/(const Point2D& otherPos) const
	{
		if (otherPos.m_X == 0 || otherPos.m_Y == 0)
		{
			std::string message = std::format("Tried to divide Position {} "
				"by a pointer with 0 {}", ToString(), otherPos.ToString());
			Utils::Log(Utils::LogType::Error, message);
			return {};
		}

		return { m_X / otherPos.m_X, m_Y / otherPos.m_Y };
	}

	Point2D Point2D::operator/(const double factor) const
	{
		if (factor == 0)
		{
			std::string message = std::format("Tried to divide Position {} by factor of 0", ToString());
			Utils::Log(Utils::LogType::Error, message);
			return {};
		}

		return { m_X / factor, m_Y / factor };
	}

	bool Point2D::operator==(const Point2D& otherPos) const
	{
		bool sameX = Utils::ApproximateEquals(m_X, otherPos.m_X);
		bool sameY = Utils::ApproximateEquals(m_Y, otherPos.m_Y);
		return sameX && sameY;
	}

	Point2D& Point2D::operator=(const Point2D& newPos)
	{
		if (this != &newPos)
		{
			m_x = newPos.m_X;
			m_y = newPos.m_Y;
		}
		else return *this;
	}

	Point2D& Point2D::operator=(Point2D&& other) noexcept
	{
		if (this == &other)
			return *this;

		m_x = std::exchange(other.m_x, 0);
		m_y = std::exchange(other.m_y, 0);

		return *this;
	}
}