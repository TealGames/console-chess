#pragma once
#include <string>

namespace Utils
{
	class Position2D
	{
	private:
		double _x;
		double _y;

	public:
		const double x;
		const double y;

		Position2D();
		Position2D(double, double);
		Position2D(const Position2D&);

		std::string ToString() const;

		Position2D operator+(const Position2D&) const;
		Position2D operator-(const Position2D&) const;
		Position2D operator*(const Position2D&) const;
		Position2D operator*(const double) const;
		Position2D operator/(const Position2D&) const;
		Position2D operator/(const double) const;

		bool operator==(const Position2D&) const;

		//This is needed because const vars delete = operator definition
		Position2D operator=(const Position2D&);
	};
}