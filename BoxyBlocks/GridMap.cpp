#include "GridMap.h"
#include <cassert>

void GridMap::Init(int blockSize, Point start, Point end) {
	for (int y = start.y; y < end.y; y += blockSize) {
		std::vector<Point> row;
		for (int x = start.x; x < end.x; x += blockSize) {
			row.push_back(Point(x, y));
		}
		gridPoints.push_back(row);
	}
	numRows = gridPoints.size();
	numCols = gridPoints[0].size();
	horizontalDiff = (double)blockSize;
	verticalDiff = horizontalDiff;
}

Line GridMap::GetLineRow(int row) {
	if (row < 0 || row >= numRows) {
		return Line();
	}
	Point end = gridPoints[row][numCols - 1];
	end.Translate(horizontalDiff, 0.0);
	return Line(gridPoints[row][0], end);
}

Line GridMap::GetLineCol(int col) {
	if (col < 0 || col >= numCols) {
		return Line();
	}
	Point end = gridPoints[numRows - 1][col];
	end.Translate(0.0, verticalDiff);
	return Line(gridPoints[0][col], end);
}

Point GridMap::GetPoint(XY pos) {
	assert((pos.x >= 0 && pos.x < numRows&& pos.y >= 0 && pos.y < numCols));
	return gridPoints[pos.y][pos.x];
}