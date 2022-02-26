#pragma once

#include <cmath>
#include <string>
#include <fstream>
#include <utility>

enum class BLOCK_TYPES {
	NUM_BLOCK_TYPES
};

class Block {
protected:
	std::pair<double, double> pos = std::pair<double, double>(0.0,0.0);
	std::pair<double, double> velocity = std::pair<double, double>(0.0, 0.0);
	std::pair<double, double> dimmensions = std::pair<double, double>(0.0, 0.0);
	bool collided = false;
public:
	Block() {}
	virtual ~Block() {}

	virtual void Update() {}
	virtual void OnUpdate() {}
	virtual void OnCollision(Block& block) {}
	virtual void AddVelocityX(double incDec);
	virtual void AddVelocityY(double incDec);
};