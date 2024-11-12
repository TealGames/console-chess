#pragma once
#include <string>
#include "Point2D.hpp"
#include "Point2DInt.hpp"

namespace Utils
{
	class Vector2D
	{
	public:
		enum class AngleMode
		{
			Degrees,
			Radians,
		};

		enum class VectorForm
		{
			Component,
			Unit,
			MagnitudeDirection,
		};

	private:
		double _x;
		double _y;

		static double CalcDirection(const Vector2D&, const AngleMode = AngleMode::Radians);
		static double CalcMagnitude(const Vector2D&);
		static Vector2D Normalize(const Vector2D&);

	public:
		const double& x;
		const double& y;

		static const Vector2D UP;
		static const Vector2D DOWN;
		static const Vector2D LEFT;
		static const Vector2D RIGHT;

		static const Vector2D ZERO;
		static const Vector2D ONE;

		Vector2D(double, double);
		Vector2D(const Vector2D&);

		double GetDirection(const AngleMode mode = AngleMode::Radians) const;
		double GetMagnitude() const;
		Vector2D GetNormalized() const;

		std::string ToString(const VectorForm form = VectorForm::MagnitudeDirection);

		Vector2D operator+(const Vector2D&) const;
		Vector2D operator-(const Vector2D&) const;
		Vector2D operator*(const Vector2D&) const;
		Vector2D operator*(const double) const;

		bool operator==(const Vector2D&) const;
	};

	/// <summary>
	/// Will get the vector formed by the 2 points
	/// </summary>
	/// <param name="startPos"></param>
	/// <param name="endPos"></param>
	/// <returns></returns>
	Vector2D GetVector(const Point2D& startPos, const Point2D& endPos);
	Utils::Point2D GetVectorEndPoint(const Point2D& startPos, const Vector2D& vector);

	Vector2D GetVector(const Point2DInt& startPos, const Point2DInt& endPos);
	Utils::Point2DInt GetVectorEndPoint(const Point2DInt& startPos, const Vector2D& vector);

	std::string ToString(const Vector2D::AngleMode& mode);
	std::string ToString(const Vector2D::VectorForm& mode);
}