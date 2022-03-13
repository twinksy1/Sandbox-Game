#pragma once

#include <cmath>
#include <string>
#include <fstream>
#include <utility>
#include <vector>
#include "Point.h"
using namespace Shapes2D;

const double BLOCK_SIZE = 2.0;

enum class BlockTypes {
	DEFAULT=0, SAND, NUM_BLOCK_TYPES
};

class GridMap;

class Block {
protected:
	enum class NeighborPositions {
		TOP_LEFT=0,		TOP,		TOP_RIGHT,
		LEFT,						RIGHT,
		BOTTOM_LEFT,	BOTTOM,		BOTTOM_RIGHT
	};

	XY mapPos;			// XY in grid
	Point pt;			// actual point on screen
	XY velocity;
	XY acceleration = XY(0,1);		// 1 for gravity
	XY widthHeight;
	BlockTypes blockType = BlockTypes::DEFAULT;
	bool immovable = false;
	bool colliding = false;

	std::vector<Block*> GetNeighbors(GridMap& gridMap);
	
	virtual void AccelerateX();
	virtual void AccelerateY();
	virtual void DeaccelerateX();
	virtual void DeaccelerateY();
	virtual void OnCollision(GridMap& gridMap) {}
	virtual void OnMove(GridMap& gridMap);

	bool OnEdge(GridMap& gridMap);
public:
	Block();
	Block(Point pt_p, double width = BLOCK_SIZE, double height = BLOCK_SIZE);
	Block(Point pt_p, XY mapPos_p, double width = BLOCK_SIZE, double height = BLOCK_SIZE);
	Block(Point pt_p, XY mapPos_p, XY velocity_p, double width = BLOCK_SIZE, double height = BLOCK_SIZE);
	Block(const Block& src);
	virtual ~Block() {}

	void SetPoint(Point pt_p) { pt = pt_p; }
	void SetVelocity(XY newVelocity);
	void Move(GridMap& gridMap);
	
	virtual void Update() {}

	inline void SetImmovable() { immovable = true; }
	inline void SetMovable() { immovable = false; }
	inline bool IsImmovable() { return immovable; }
	inline XY GetVelocity() { return velocity; }
	inline Point GetPoint() { return pt; }
	inline XY GetDimmensions() { return widthHeight; }
	inline XY GetMapPos() { return mapPos; }

	void operator=(const Block& src);
};