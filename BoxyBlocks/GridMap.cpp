#include "GridMap.h"
#include <cassert>

GridMap::GridMap(const GridMap& src) {
	gridPoints = src.gridPoints;
	occupiedCells = src.occupiedCells;
	horizontalDiff = src.horizontalDiff;
	verticalDiff = src.verticalDiff;
	numRows = src.numRows;
	numCols = src.numCols;
}

void GridMap::Init(int blockSize, Point start, Point end) {
	int i = 0, j = 0;
	for (int y = start.y; y < end.y; y += blockSize) {
		std::vector<std::shared_ptr<Point>> row;
		j = 0;
		for (int x = start.x; x < end.x; x += blockSize) {
			row.push_back(std::make_shared<Point>(x, y));
			j++;
		}
		gridPoints.push_back(row);
		i++;
	}
	numRows = gridPoints.size();
	numCols = gridPoints[0].size();
	occupiedCells = std::vector<std::vector<bool>>(numRows, std::vector<bool>(numCols, false));
	horizontalDiff = (double)blockSize;
	verticalDiff = horizontalDiff;
}

Line GridMap::GetLineRow(int row) {
	if (row < 0 || row >= numRows) {
		return Line();
	}
	std::shared_ptr<Point> end = gridPoints[row][numCols - 1];
	end->Translate(horizontalDiff, 0.0);
	return Line(gridPoints[row][0], end);
}

Line GridMap::GetLineCol(int col) {
	if (col < 0 || col >= numCols) {
		return Line();
	}
	std::shared_ptr<Point> end = gridPoints[numRows - 1][col];
	end->Translate(0.0, verticalDiff);
	return Line(gridPoints[0][col], end);
}

Point GridMap::GetPoint(XY pos) {
	assert((pos.x >= 0 && pos.x < numCols && pos.y >= 0 && pos.y < numRows));
	return *gridPoints[(int)pos.y][(int)pos.x];
}

bool GridMap::IsOccupied(XY pos) {
	if ((int)pos.x < 0 || (int)pos.x > numCols - 1 || (int)pos.y < 0 || (int)pos.y > numRows - 1) {
		return true;
	}
	return occupiedCells[(int)pos.y][(int)pos.x];
}

void GridMap::ClearOccupiedCells() {
	for (int i = 0; i < numRows; i++) {
		for (int j = 0; j < numCols; j++) {
			occupiedCells[i][j] = false;
		}
	}
}

void GridMap::SetOccupied(XY pos, Block* block) {
	occupiedCells[(int)pos.y][(int)pos.x] = true;
	std::pair<int, int> key(pos.x, pos.y);
	std::map<std::pair<int, int>, Block*>::iterator it = gridPosToBlock.find(key);
	if (it == gridPosToBlock.end()) {
		gridPosToBlock.emplace(key, block);
	}
	else {
		gridPosToBlock[key] = block;
	}
	
}

bool GridMap::InRange(XY pos) {
	return pos.x >= 0 && pos.x < numCols && pos.y >= 0 && pos.y < numRows;
}

void GridMap::SetUnoccupied(XY pos) {
	occupiedCells[(int)pos.y][(int)pos.x] = false;
	std::pair<int, int> key(pos.x, pos.y);
	gridPosToBlock[key] = NULL;
}

Block* GridMap::GetBlockAtPos(XY pos) {
	std::pair<int, int> key(pos.x, pos.y);
	std::map<std::pair<int, int>, Block*>::iterator it = gridPosToBlock.find(key);
	if (it != gridPosToBlock.end()) {
		return it->second;
	}
	return NULL;
}