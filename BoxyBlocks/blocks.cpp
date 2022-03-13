#include "blocks.h"
#include "GridMap.h"

Block::Block() {
	velocity = XY(0, 1);
	acceleration = XY(0, 0.25);
	immovable = false;
}

Block::Block(Point pt_p, double width, double height) : pt(pt_p) {
	widthHeight = XY(width, height);
	velocity = XY(0, 1);
	acceleration = XY(0.25, 0.25);
	immovable = false;
}

Block::Block(Point pt_p, XY mapPos_p, double width, double height) : pt(pt_p) {
	widthHeight = XY(width, height);
	mapPos = mapPos_p;
	velocity = XY(0, 1);
	acceleration = XY(0.25, 0.25);
	immovable = false;
}

Block::Block(Point pt_p, XY mapPos_p, XY velocity_p, double width, double height) {
	widthHeight = XY(width, height);
	mapPos = mapPos_p;
	velocity = velocity_p;
	acceleration = XY(0.25, 0.25);
	immovable = false;
}

Block::Block(const Block& src) {
	mapPos = src.mapPos;
	pt = src.pt;
	velocity = src.velocity;
	immovable = src.immovable;
	acceleration = src.acceleration;
	widthHeight = src.widthHeight;
	blockType = src.blockType;
}

void Block::operator=(const Block& src) {
	mapPos = src.mapPos;
	pt = src.pt;
	velocity = src.velocity;
	immovable = src.immovable;
	acceleration = src.acceleration;
	widthHeight = src.widthHeight;
	blockType = src.blockType;
}

void Block::SetVelocity(XY newVelocity) {
	velocity = newVelocity;
}

void Block::AccelerateX() {
	velocity.x = velocity.x < 0.0 ? (velocity.x - acceleration.x) : (velocity.x + acceleration.x);
}

void Block::AccelerateY() {
	velocity.y += acceleration.y;
}

void Block::DeaccelerateX() {
	velocity.x = velocity.x < 0.0 ? (velocity.x + acceleration.x < 0.0 ? (velocity.x + acceleration.x) : (0.0)) : (velocity.x - acceleration.x < 0.0 ? (0.0) : (velocity.x - acceleration.x));
}

void Block::DeaccelerateY() {
	velocity.y = velocity.y < 0.0 ? (velocity.y + acceleration.y < 0.0 ? (velocity.y + acceleration.y) : (0.0)) : (velocity.y - acceleration.y < 0.0 ? (0.0) : (velocity.y - acceleration.y));
}

void Block::OnMove(GridMap& gridMap) {
	if (velocity.y > 0) {
		AccelerateY();
	}
	else if (velocity.y < 0) {
		DeaccelerateY();
	}
	else {
		velocity.y = 1;
	}

	if (velocity.x != 0.0) {
		DeaccelerateX();
	}
}

void Block::Move(GridMap& gridMap) {
	if (immovable) return;
	colliding = false;
	XY tmpPos = mapPos;
	XY oldPos = mapPos;
	gridMap.SetUnoccupied(mapPos);
	for (int i = 0, j = 0; i < abs(velocity.y) || j < abs(velocity.x); i++, j++) {
		bool moved = false;
		if (i < abs(velocity.y) && j < abs(velocity.x)) {
			tmpPos += XY((velocity.x < 0.0 ? -1 : 1), (velocity.y < 0.0 ? -1 : 1));
			if (gridMap.InRange(tmpPos) && !gridMap.IsOccupied(tmpPos)) {
				mapPos = tmpPos;
				moved = true;
			}
			tmpPos = mapPos;
		}
		if (i < abs(velocity.y) && !moved) {
			tmpPos += XY(0, (velocity.y < 0.0 ? -1 : 1));
			if (gridMap.InRange(tmpPos) && !gridMap.IsOccupied(tmpPos)) {
				mapPos = tmpPos;
				moved = true;
			}
			tmpPos = mapPos;
		}
		if (j < abs(velocity.x) && !moved) {
			tmpPos += XY((velocity.x < 0.0 ? -1 : 1), 0);
			if (gridMap.InRange(tmpPos) && !gridMap.IsOccupied(tmpPos)) {
				mapPos = tmpPos;
				moved = true;
			}
			tmpPos = mapPos;
		}

		if (!moved) {
			// If you can't move anywhere, most likely colliding
			colliding = true;
			break;
		}		
	}
	mapPos = XY((int)mapPos.x, (int)mapPos.y);
	gridMap.SetOccupied(mapPos, this);
	pt = gridMap.GetPoint(mapPos);
	OnMove(gridMap);
	if (colliding) {
		OnCollision(gridMap);
	}
}

std::vector<Block*> Block::GetNeighbors(GridMap& gridMap) {
	std::vector<XY> neighbors = {
		XY((int)mapPos.x - 1, (int)mapPos.y - 1),
		XY((int)mapPos.x, (int)mapPos.y - 1),
		XY((int)mapPos.x + 1, (int)mapPos.y - 1),
		XY((int)mapPos.x - 1, (int)mapPos.y),
		XY((int)mapPos.x + 1, (int)mapPos.y),
		XY((int)mapPos.x - 1, (int)mapPos.y + 1),
		XY((int)mapPos.x, (int)mapPos.y + 1),
		XY((int)mapPos.x + 1, (int)mapPos.y + 1),
	};

	std::vector<Block*> ret;

	for (int i = 0; i < neighbors.size(); i++) {
		ret.push_back(gridMap.GetBlockAtPos(neighbors[i]));
	}
	return ret;
}

bool Block::OnEdge(GridMap& gridMap) {
	return mapPos.x == 0 || mapPos.y == 0 || mapPos.x == gridMap.GetNumCols() - 1 || mapPos.y == gridMap.GetNumRows() - 1;
}