#include "Sand.h"
#include "GridMap.h"

Sand::Sand() : Block() {
	blockType = BlockTypes::SAND;
}

Sand::Sand(Point pt_p, double width, double height) : Block(pt_p, width, height) {
	blockType = BlockTypes::SAND;
}

Sand::Sand(Point pt_p, XY mapPos_p, double width, double height) : Block(pt_p, mapPos_p, width, height) {
	blockType = BlockTypes::SAND;
}

Sand::Sand(Point pt_p, XY mapPos_p, XY velocity_p, double width, double height) : Block(pt_p, mapPos_p, velocity_p, width, height) {
	blockType = BlockTypes::SAND;
}

Sand::Sand(const Sand& src) : Block(src) {

}

void Sand::OnMove(GridMap& gridMap) {
	AccelerateY();
	std::vector<Block*> neighbors = GetNeighbors(gridMap);
	if (colliding) {
		bool slided = false;
		if (gridMap.InRange(XY(mapPos.x - 1, mapPos.y + 1))) {
			if (!neighbors[(int)NeighborPositions::LEFT] && !neighbors[(int)NeighborPositions::BOTTOM_LEFT]) {
				velocity = XY(-1, 1);
				slided = true;
			}
		}
		if (gridMap.InRange(XY(mapPos.x + 1, mapPos.y + 1)) && !slided) {
			if (!neighbors[(int)NeighborPositions::RIGHT] && !neighbors[(int)NeighborPositions::BOTTOM_RIGHT]) {
				velocity = XY(1, 1);
				return;
			}
		}
		if (!slided) {
			velocity = XY(0, 0);
		}
	}
	else {
		DeaccelerateX();
	}
}
