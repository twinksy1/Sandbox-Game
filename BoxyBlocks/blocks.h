#pragma once

#include <cmath>
#include <string>
#include <fstream>
#include <utility>
#include "Square.h"
using namespace Shapes2D;

enum class BlockTypes {
	DEFAULT=0, NUM_BLOCK_TYPES
};

class Block {
protected:
	XY mapPos;			// XY in grid
	Point pt;			// actual point on screen
	XY velocity;
	XY acceleration = XY(0,1);		// 1 for gravity
	BlockTypes blockType = BlockTypes::DEFAULT;
	bool immovable = false;
public:
	Block() {}
	Block(Point pt_p) : pt(pt_p) {}
	virtual ~Block() {}

	void SetPoint(Point pt_p) { pt = pt_p; }
	virtual void Update() {}
	virtual void OnUpdate() {}
	virtual void OnCollision(Block& block) {}
	
	virtual void AccelerateX();
	virtual void AccelerateY();
	Square GetAsSquare();

	inline void SetImmovable() { immovable = true; }
	inline void SetMovable() { immovable = false; }
};