#include "Shapes.h"

namespace Shapes {
	Line::Line(double p1X, double p1Y, double p2X, double p2Y) {
		p1 = Point(p1X, p1Y);
		p2 = Point(p2X, p2Y);

		run = p2X - p1X;
		rise = p2Y - p1Y;
		midpoint = Point((p1.x + p2.x) / 2.0, (p1.y + p2.y) / 2.0);
	}

	Line::Line(Point p1_p, Point p2_p) : p1(p1_p), p2(p2_p) {
		run = p2.x - p1.x;
		rise = p2.y - p1.y;
		midpoint = Point((p1.x + p2.x) / 2.0, (p1.y + p2.y) / 2.0);
	}

	std::shared_ptr<Point> Line::CheckLineIntersection(Point ptStart, Point ptEnd) {
		double p1DiffX = 0.0, p1DiffY = 0.0, p2DiffX = 0.0, p2DiffY = 0.0;
		p1DiffX = p2.x - p1.x;
		p1DiffY = p2.y - p1.y;
		p2DiffX = ptEnd.x - ptStart.x;
		p2DiffY = ptEnd.y - ptStart.y;

		double s = (-p1DiffY * (p1.x - ptStart.x) + p1DiffX * (p1.y - ptStart.y)) / (-p2DiffX * p1DiffY + p1DiffX * p2DiffY);
		double t = (p2DiffX * (p1.y - ptStart.y) - p2DiffY * (p1.x - ptStart.x)) / (-p2DiffX * p1DiffY + p1DiffX * p2DiffY);

		if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
			return std::make_shared<Point>(Point(p1.x + (t * p1DiffX), p1.y + (t * p1DiffY)));
		}

		return NULL;
	}

	std::shared_ptr<Point> Line::CheckLineIntersection(Line& line) {
		return CheckLineIntersection(line.GetPt1(), line.GetPt2());
	}

	void Line::Rotate(double angle, std::shared_ptr<Point> center) {
		if (center == NULL) {
			p1.Rotate(angle, midpoint);
			p2.Rotate(angle, midpoint);
		}
		else {
			p1.Rotate(angle, *center);
			p2.Rotate(angle, *center);
		}
	}

	double Line::GetSlope() {
		double retVal = 0.0;
		// More than likely be a divide by zero exception
		try {
			retVal = rise / run;
			return retVal;
		}
		catch (...) {
			return 0.0;
		}
	}

	double Line::GetLength() {
		return sqrt(run * run + rise * rise);
	}

	void Line::IncDecPt1(std::pair<double, double> incDec) {
		p1.x += incDec.first;
		p1.y += incDec.second;
	}

	void Line::IncDecPt2(std::pair<double, double> incDec) {
		p2.x += incDec.first;
		p2.y += incDec.second;
	}

	void Line::operator=(Line src) {
		this->p1 = src.GetPt1();
		this->p2 = src.GetPt2();
		this->rise = src.rise;
		this->run = src.run;
	}

	void Shape::CheckCollision(Line line) {
		collisionInfo.Clear();
		colliding = false;
		for (int i = 0; i < lines.size(); i++) {
			std::shared_ptr<Point> poi = lines[i].CheckLineIntersection(line);
			if (poi != NULL) {
				collisionInfo.collidingLines.push_back(i);
				collisionInfo.pointsOfIntersection.push_back(poi);
				collisionInfo.shapeType = L"Line";
				colliding = true;
				OnCollision();
			}
		}
	}

	void Shape::Rotate(double angle) {
		for (auto& line : lines) {
			line.Rotate(angle, std::make_shared<Point>(center));
		}
	}

	void Shape::CheckCollision(Shape* shape) {
		if (shape == NULL) return;
		collisionInfo.Clear();
		colliding = false;
		for (int i = 0; i < lines.size(); i++) {
			for (int j = 0; j < shape->lines.size(); j++) {
				std::shared_ptr<Point> poi = lines[i].CheckLineIntersection(shape->lines[j]);
				if (poi != NULL) {
					collisionInfo.collidingLines.push_back(i);
					collisionInfo.pointsOfIntersection.push_back(poi);
					collisionInfo.shapeType = shape->GetShapeName();
					colliding = true;
					OnCollision();
				}
			}
		}
	}

	void Shape::CalculateCenter() {
		double medianX = 0.0, medianY = 0.0;
		for (auto& line : lines) {
			medianX += line.GetPt1().x + line.GetPt2().x;
			medianY += line.GetPt1().y + line.GetPt2().y;
		}
		center = Point(medianX / (2.0 * (double)lines.size()), medianY / (2.0 * (double)lines.size()));
	}

	Square::Square(Square& src) {
		this->lines = src.lines;
		CalculateCenter();
	}

	void Square::IncDecSquarePoints(std::pair<double, double> incDec) {
		if (incDec.second < 0.0) {
			lines[(int)SquareSides::TOP].IncDecPt1(std::pair<double, double>(0.0, incDec.second));
			lines[(int)SquareSides::TOP].IncDecPt2(std::pair<double, double>(0.0, incDec.second));
			lines[(int)SquareSides::LEFT].IncDecPt1(std::pair<double, double>(0.0, incDec.second));
			lines[(int)SquareSides::RIGHT].IncDecPt1(std::pair<double, double>(0.0, incDec.second));
		}
		else {
			lines[(int)SquareSides::BOTTOM].IncDecPt1(std::pair<double, double>(0.0, incDec.second));
			lines[(int)SquareSides::BOTTOM].IncDecPt2(std::pair<double, double>(0.0, incDec.second));
			lines[(int)SquareSides::LEFT].IncDecPt1(std::pair<double, double>(0.0, incDec.second));
			lines[(int)SquareSides::RIGHT].IncDecPt1(std::pair<double, double>(0.0, incDec.second));
		}

		if (incDec.first < 0.0) {
			lines[(int)SquareSides::TOP].IncDecPt1(std::pair<double, double>(incDec.first, 0.0));
			lines[(int)SquareSides::LEFT].IncDecPt1(std::pair<double, double>(incDec.first, 0.0));
			lines[(int)SquareSides::LEFT].IncDecPt2(std::pair<double, double>(incDec.first, 0.0));
		}
		else {
			lines[(int)SquareSides::BOTTOM].IncDecPt2(std::pair<double, double>(incDec.first, 0.0));
			lines[(int)SquareSides::RIGHT].IncDecPt1(std::pair<double, double>(incDec.first, 0.0));
			lines[(int)SquareSides::RIGHT].IncDecPt2(std::pair<double, double>(incDec.first, 0.0));
		}
		CalculateCenter();
	}

	Square::Square(Line line) {
		SetAllEqualSides(line, SquareSides::LEFT);
		shapeName = L"Square";
	}

	std::pair<double, double> Square::GetDimmensions() {
		Line line = Line(lines[(int)SquareSides::TOP].GetPt1(), lines[(int)SquareSides::BOTTOM].GetPt2());
		return std::pair<double, double>(line.run, line.rise);
	}

	void Square::SetAllEqualSides(Line line, SquareSides side) {
		// Square should have all sides of equal lengths
		// top & bottom slopes should be equal
		// left & right slopes should be equal
		lines[(int)side] = line;
		switch (side) {
		case SquareSides::LEFT:
			lines[(int)SquareSides::TOP] = Line(line.GetPt1(), Point(line.GetPt1().x + line.rise, line.GetPt1().y - line.run));
			lines[(int)SquareSides::BOTTOM] = Line(line.GetPt2(), Point(line.GetPt2().x + line.rise, line.GetPt2().y - line.run));
			lines[(int)SquareSides::RIGHT] = Line(lines[(int)SquareSides::TOP].GetPt2(), lines[(int)SquareSides::BOTTOM].GetPt2());
			break;
		case SquareSides::RIGHT:
			lines[(int)SquareSides::TOP] = Line(Point(line.GetPt1().x - line.rise, line.GetPt1().y + line.run), line.GetPt1());
			lines[(int)SquareSides::BOTTOM] = Line(Point(line.GetPt2().x - line.rise, line.GetPt2().y + line.run), line.GetPt2());
			lines[(int)SquareSides::LEFT] = Line(lines[(int)SquareSides::TOP].GetPt1(), lines[(int)SquareSides::BOTTOM].GetPt1());
			break;
		case SquareSides::TOP:
			lines[(int)SquareSides::LEFT] = Line(line.GetPt1(), Point(line.GetPt1().x - line.rise, line.GetPt1().y + line.run));
			lines[(int)SquareSides::RIGHT] = Line(line.GetPt2(), Point(line.GetPt2().x - line.rise, line.GetPt2().y + line.run));
			lines[(int)SquareSides::BOTTOM] = Line(lines[(int)SquareSides::LEFT].GetPt2(), lines[(int)SquareSides::RIGHT].GetPt2());
			break;
		case SquareSides::BOTTOM:
			lines[(int)SquareSides::LEFT] = Line(Point(line.GetPt1().x + line.rise, line.GetPt1().y - line.run), line.GetPt1());
			lines[(int)SquareSides::RIGHT] = Line(Point(line.GetPt2().x + line.rise, line.GetPt2().y - line.run), line.GetPt2());
			lines[(int)SquareSides::TOP] = Line(lines[(int)SquareSides::LEFT].GetPt1(), lines[(int)SquareSides::RIGHT].GetPt1());
			break;
		}
		CalculateCenter();
	}

	void Square::SetTwoEqualSidesLR(Line left, Line right) {
		if (!AreParallel(left, right)) {
			SetAllEqualSides(left, SquareSides::LEFT);
		}
		else {
			lines[(int)SquareSides::LEFT] = left;
			lines[(int)SquareSides::RIGHT] = right;
			lines[(int)SquareSides::TOP] = Line(left.GetPt1(), right.GetPt1());
			lines[(int)SquareSides::BOTTOM] = Line(left.GetPt2(), right.GetPt2());
			CalculateCenter();
		}
	}

	void Square::Translate(std::pair<double, double> incDec) {
		double rise = incDec.second - lines[(int)SquareSides::TOP].GetPt1().y;
		double run = incDec.first - lines[(int)SquareSides::TOP].GetPt1().x;
		for (auto& line : lines) {
			line.SetP1(Point(line.GetPt1().x + run, line.GetPt1().y + rise));
			line.SetP2(Point(line.GetPt2().x + run, line.GetPt2().y + rise));
		}
	}

	void Square::SetTwoEqualSidesTB(Line top, Line bottom) {
		if (!AreParallel(top, bottom)) {
			SetAllEqualSides(top, SquareSides::TOP);
		}
		else {
			lines[(int)SquareSides::TOP] = top;
			lines[(int)SquareSides::BOTTOM] = bottom;
			lines[(int)SquareSides::LEFT] = Line(top.GetPt1(), bottom.GetPt1());
			lines[(int)SquareSides::RIGHT] = Line(top.GetPt2(), bottom.GetPt2());
			CalculateCenter();
		}
	}
}