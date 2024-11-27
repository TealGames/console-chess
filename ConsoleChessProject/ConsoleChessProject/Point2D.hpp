#pragma once
#include <string>

namespace Utils
{
	class Point2D
	{
	private:
		double m_x;
		double m_y;

	public:
		const double& m_X;
		const double& m_Y;

		Point2D();
		Point2D(double, double);
		Point2D(const Point2D&);

		std::string ToString() const;

		//TODO: add all other operators for rvalues as well
		Point2D operator+(const Point2D&) const;
		Point2D operator-(const Point2D&) const;
		Point2D operator*(const Point2D&) const;
		Point2D operator*(const double) const;
		Point2D operator/(const Point2D&) const;
		Point2D operator/(const double) const;

		bool operator==(const Point2D&) const;

		//This is needed because const vars delete = operator definition
		Point2D& operator=(const Point2D&);
		Point2D& operator=(Point2D&&) noexcept;
	};
}

namespace std 
{
	template<> struct hash<Utils::Point2D> 
	{
		std::size_t operator()(const Utils::Point2D& pos) const noexcept 
		{
			std::hash<double> hashDouble;
			std::size_t xHash = hashDouble(pos.m_X);
			std::size_t yHash = hashDouble(pos.m_Y);
			return xHash ^ (yHash << 1);
		}
	};
}