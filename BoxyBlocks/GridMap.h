#pragma once

#include <vector>
#include <map>
#include "Line.h"
using namespace Shapes2D;

class GridMap {
	std::vector<std::vector<Point>> gridPoints;
	double horizontalDiff = 0;
	double verticalDiff = 0;
	int numRows = 0;
	int numCols = 0;
public:
	GridMap() {}
	~GridMap() {}

	void Init(int blockSize, Point start, Point end);
	Line GetLineRow(int row);
	Line GetLineCol(int col);
	Point GetPoint(XY pos);

	inline const int GetNumRows() { return numRows; }
	inline const int GetNumCols() { return numCols; }
};

