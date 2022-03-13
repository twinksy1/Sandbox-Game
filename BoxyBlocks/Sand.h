#pragma once
#include "blocks.h"

class Sand : public Block {
protected:
	void OnMove(GridMap& gridMap) override;
public:
	Sand();
	Sand(Point pt_p, double width = BLOCK_SIZE, double height = BLOCK_SIZE);
	Sand(Point pt_p, XY mapPos_p, double width = BLOCK_SIZE, double height = BLOCK_SIZE);
	Sand(Point pt_p, XY mapPos_p, XY velocity_p, double width = BLOCK_SIZE, double height = BLOCK_SIZE);
	Sand(const Sand& src);
};

