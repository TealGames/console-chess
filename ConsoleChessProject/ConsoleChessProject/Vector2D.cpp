#include <cmath>
#include <string>
#include <format>
#include "HelperFunctions.hpp"
#include "Vector2D.hpp"

namespace Utils
{
	const Vector2D Vector2D::UP(0, 1);
	const Vector2D Vector2D::DOWN(0, -1);
	const Vector2D Vector2D::LEFT(-1, 0);
	const Vector2D Vector2D::RIGHT(1, 0);

	const Vector2D Vector2D::ZERO(0, 0);
	const Vector2D Vector2D::ONE(1, 1);

	Vector2D::Vector2D(double xComp, double yComp)
		: _x(xComp), _y(yComp), x(_x), y(_y)
	{

	}

	Vector2D::Vector2D(const Vector2D& vec)
		: _x(vec.x), _y(vec.y), x(_x), y(_y)
	{

	}

	double Vector2D::CalcDirection(const Vector2D& vec, const AngleMode angleMode)
	{
		if (vec.x == 0) return vec.y;
		else if (vec.y == 0) return vec.x;

		double rad = std::atan2(vec.y, vec.x);
		if (angleMode == AngleMode::Radians) return rad;
		else return Utils::ToDegrees(rad);
	}

	double Vector2D::CalcMagnitude(const Vector2D& vec)
	{
		return std::sqrt(std::pow(vec.x, 2) + std::pow(vec.y, 2));
	}

	Vector2D Vector2D::Normalize(const Vector2D& vec)
	{
		double magnitude = CalcMagnitude(vec);
		Vector2D unit(vec.x / magnitude, vec.y / magnitude);
		return unit;
	}

	/// <summary>
	/// Gets the direction of the vector in radians
	/// </summary>
	/// <returns></returns>
	double Vector2D::GetDirection(const AngleMode angleMode) const
	{
		return CalcDirection(*this, angleMode);
	}

	double Vector2D::GetMagnitude() const
	{
		return CalcMagnitude(*this);
	}

	Vector2D Vector2D::GetNormalized() const
	{
		return Normalize(*this);
	}

	std::string Vector2D::ToString(VectorForm form)
	{
		std::string str;
		switch (form)
		{
		case VectorForm::Component:
			str = std::format("({},{})", x, y);
			break;

		case VectorForm::Unit:
			str = std::format("{{}i+{}j}", x, y);
			break;

		case VectorForm::MagnitudeDirection:
			str = std::format("{}@ {}°", GetMagnitude(), GetDirection(AngleMode::Degrees));
			break;

		default:
			std::string error = std::format("Tried to convert vector ({},{}) to string "
				"with undefined form {}", x, y, form);
			Utils::Log(LogType::Error, error);
			break;
		}
		return str;
	}

	Vector2D Vector2D::operator+(const Vector2D& otherVec) const
	{
		Vector2D resultant(x + otherVec.x, y + otherVec.y);
		return resultant;
	}

	Vector2D Vector2D::operator-(const Vector2D& otherVec) const
	{
		Vector2D resultant(x - otherVec.x, y - otherVec.y);
		return resultant;
	}

	Vector2D Vector2D::operator*(const Vector2D& otherVec) const
	{
		Vector2D resultant(x * otherVec.x, y * otherVec.y);
		return resultant;
	}

	Vector2D Vector2D::operator*(const double scalar) const
	{
		Vector2D resultant(x * scalar, y * scalar);
		return resultant;
	}

	bool Vector2D::operator==(const Vector2D& otherVec) const
	{
		bool sameX = Utils::ApproximateEquals(x, otherVec.x);
		bool sameY = Utils::ApproximateEquals(y, otherVec.y);
		return sameX && sameY;
	}

	Vector2D GetVector(const Position2D& startPos, const Position2D& endPos)
	{
		Vector2D result(endPos.x - startPos.x, endPos.y - startPos.y);
		return result;
	}
}