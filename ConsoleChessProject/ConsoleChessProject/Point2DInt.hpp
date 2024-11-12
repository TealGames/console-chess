#pragma once
#include <string>

namespace Utils
{
	class Point2DInt
	{
	private:
		int _x;
		int _y;

	public:
		const int x;
		const int y;

		Point2DInt();
		Point2DInt(int, int);
		Point2DInt(const Point2DInt&);

		std::string ToString() const;

		Point2DInt operator+(const Point2DInt&) const;
		Point2DInt operator-(const Point2DInt&) const;
		Point2DInt operator*(const Point2DInt&) const;
		Point2DInt operator*(const int) const;
		Point2DInt operator/(const Point2DInt&) const;
		Point2DInt operator/(const int) const;

		bool operator==(const Point2DInt&) const;

		//This is needed because const vars delete = operator definition
		Point2DInt& operator=(const Point2DInt&);
	};
}

namespace std
{
	template<> struct hash<Utils::Point2DInt>
	{
		std::size_t operator()(const Utils::Point2DInt& pos) const noexcept
		{
			std::hash<int> hashDouble;
			std::size_t xHash = hashDouble(pos.x);
			std::size_t yHash = hashDouble(pos.y);
			return xHash ^ (yHash << 1);
		}
	};
}

