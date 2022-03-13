#pragma once

#include <vector>
#include <map>
#include "Line.h"
using namespace Shapes2D;

class Block;

class GridMap {
	std::vector<std::vector<std::shared_ptr<Point>>> gridPoints;
	std::vector<std::vector<bool>> occupiedCells;
	std::map<std::pair<int, int>, Block*> gridPosToBlock;
	double horizontalDiff = 0;
	double verticalDiff = 0;
	int numRows = 0;
	int numCols = 0;
public:
	GridMap() {}
	GridMap(const GridMap& src);
	~GridMap() {}

	void Init(int blockSize, Point start, Point end);
	Line GetLineRow(int row);
	Line GetLineCol(int col);
	Point GetPoint(XY pos);
	bool IsOccupied(XY pos);
	void ClearOccupiedCells();
	void SetOccupied(XY pos, Block* block);
	Block* GetBlockAtPos(XY pos);
	void SetUnoccupied(XY pos);
	bool InRange(XY pos);

	inline const int GetNumRows() { return numRows; }
	inline const int GetNumCols() { return numCols; }
	inline double GetCellWidth() { return horizontalDiff; }
	inline double GetCellHeight() { return verticalDiff; }
};

