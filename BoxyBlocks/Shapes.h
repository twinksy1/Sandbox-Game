#pragma once

#include <utility>
#include <memory>
#include <vector>
#include <string>
#include <cmath>

namespace Shapes {
	enum class SquareSides {
		LEFT=0, RIGHT, TOP, BOTTOM, NUM_SQUARE_SIDES
	};

	struct Point {
		double x = 0.0;
		double y = 0.0;


		Point() {}
		Point(double x_p, double y_p) : x(x_p), y(y_p) {}
		inline void Rotate(double angle, Point center = Point(0.0, 0.0)) {
			double oldX = x, oldY = y;
			x = (oldX - center.x) * cos(angle) - (oldY - center.y) * sin(angle) + center.x;
			y = (oldX - center.x) * sin(angle) + (oldY - center.y) * cos(angle) + center.y;
		}
	};

	class Line {
		Point p1;
		Point p2;
		Point midpoint;
	public:
		double run = 0.0;
		double rise = 0.0;

		Line() {}
		Line(double p1X, double p1Y, double p2X, double p2Y);
		Line(Point p1_p, Point p2_p);
		~Line() {}

		std::shared_ptr<Point> CheckLineIntersection(Point ptStart, Point ptEnd);
		std::shared_ptr<Point> CheckLineIntersection(Line& line);

		Point& GetPt1() { return p1; }
		Point& GetPt2() { return p2; }
		double GetSlope();
		double GetLength();
		void Rotate(double angle, std::shared_ptr<Point> center = NULL);
		void IncDecPt1(std::pair<double, double> incDec);
		void IncDecPt2(std::pair<double, double> incDec);

		void SetP1(Point p1_p) { p1 = p1_p; }
		void SetP2(Point p2_p) { p2 = p2_p; }
		void operator=(Line src);
	};

	struct CollisionInfo {
		std::wstring shapeType = L"Shape";
		std::vector<int> collidingLines;
		std::vector<std::shared_ptr<Point>> pointsOfIntersection;

		void Clear() { shapeType = L""; collidingLines.clear(); pointsOfIntersection.clear(); }
	};

	class Shape {
	protected:
		std::wstring shapeName = L"Shape";
		Point center;
	public:
		CollisionInfo collisionInfo;
		std::vector<Line> lines;
		bool colliding = false;
		Shape() {}
		virtual ~Shape() {}

		void CheckCollision(Line line);
		void CheckCollision(Shape* shape);
		void Rotate(double angle);
		void CalculateCenter();
		virtual void OnCollision() {}

		virtual std::wstring GetShapeName() { return shapeName; }
	};

	class Square : public Shape {
	public:
		Square() { lines = { Line(), Line(), Line(), Line() }; shapeName = L"Square"; }
		Square(Line line);
		Square(Line line1, Line line2, bool LR = true) { lines = { Line(), Line(), Line(), Line() }; shapeName = L"Square"; if (LR) { SetTwoEqualSidesLR(line1, line2); } else { SetTwoEqualSidesTB(line1, line2); } }
		Square(Square& src);
		~Square() {}

		void SetAllEqualSides(Line line, SquareSides side);
		void SetTwoEqualSidesLR(Line line1, Line line2);	//	left & right sides have equal length
		void SetTwoEqualSidesTB(Line line1, Line line2);	// top & bottom sides have equal length
		void IncDecSquarePoints(std::pair<double, double> incDec);
		void Translate(std::pair<double, double> incDec);

		Line GetLine(SquareSides side) { return lines[(int)side]; }
		std::pair<double, double> GetDimmensions();
		Point GetUpperLeftPoint() { return lines[(int)SquareSides::TOP].GetPt1(); }
	};

	inline bool AreParallel(Line line1, Line line2) {
		return line1.rise == line2.rise && line1.run == line2.run;
	}

	inline Line CreateLine(double x0, double y0, double x1, double y1) {
		return Line(x0, y0, x1, y1);
	}

	inline Square CreateEquilateralSquare(Line line) {
		return Square(line);
	}
}