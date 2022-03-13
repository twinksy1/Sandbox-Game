#pragma once

#include "Square.h"
#include <utility>
#include <memory>
using namespace Shapes2D;

const double TOP_SPEED = 20.0;

class Entity {
protected:
	// first = x, second = y
	XY pos;	// might turn to world position
	Square collisionBox;
	XY velocity;
	//

	virtual void OnUpdate() {}

	void MoveTo(double x, double y);
	void MoveTo(XY newPos);
	//SquareSides GetCollidingDirection(Entity* collider);
	virtual void OnMove() {}
	void ResolveCollisions();
	bool immovable = false;
public:
	Entity() {}
	Entity(const Entity& src);
	Entity(XY pos_p, Square collisionBox_p);
	Entity(XY pos_p, Square collisionBox_p, XY velocity_p);
	~Entity() {}

	virtual void Update() {}
	virtual void OnCollision(Entity* collider);
	void CheckCollision(Entity* collider);
	virtual void Move();
	
	void Rotate(double angle);
	

	inline XY GetPos() { return pos; }
	inline std::pair<double, double> GetDimmensions() { return collisionBox.GetDimmensions(); }
	inline void SetImmovable() { immovable = true; }
	inline void SetMovable() { immovable = false; }
	inline bool Collided() { return collisionBox.Collided(); }
	inline Square& GetCollisionBox() { return collisionBox; }
};

