#pragma once

#include "Square.h"
#include <utility>
#include <memory>
using namespace Shapes2D;

const double TOP_SPEED = 20.0;

class Entity {
protected:
	// first = x, second = y
	std::pair<double, double> pos = std::pair<double, double>(0.0, 0.0);	// might turn to world position
	Square collisionBox;
	//

	virtual void OnUpdate() {}

	void MoveTo(double x, double y);
	void MoveTo(std::pair<double, double> newPos);
	SquareSides GetCollidingDirection(Entity* collider);
	virtual void OnMove() {}
	void ResolveCollisions();
public:
	Entity() {}
	Entity(const Entity& src);
	Entity(std::pair<double, double> pos_p, Square collisionBox_p);
	Entity(std::pair<double, double> pos_p, Square collisionBox_p, std::pair<double, double> velocity_p);
	~Entity() {}

	virtual void Update() {}
	virtual void OnCollision(Entity* collider);
	void CheckCollision(Entity* collider);
	virtual void Move();
	
	void Rotate(double angle);
	

	std::pair<double, double> GetPos() { return pos; }
	std::pair<double, double> GetDimmensions() { return collisionBox.GetDimmensions(); }
	void SetImmovable(bool val) { collisionBox.immovable = val; }
	bool Collided() { return collisionBox.xAxisCollision || collisionBox.yAxisCollision; }
	Square& GetCollisionBox() { return collisionBox; }
	void TurnOffGravity() { collisionBox.SetConstantGravity(0.0); }
};

