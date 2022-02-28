#pragma once

#include <utility>
#include <memory>
#include "Shapes.h"
using namespace Shapes;

class Entity {
protected:
	// first = x, second = y
	std::pair<double, double> pos = std::pair<double, double>(0.0, 0.0);	// might turn to world position
	std::pair<double, double> velocity = std::pair<double, double>(0.0, 0.0);
	std::pair<double, double> acceleration = std::pair<double, double>(0.0, 0.0);
	Square collisionBox;
	//
	bool grounded = false;

	virtual void OnUpdate() {}

	void AddVelocityX(double incDec);
	void AddVelocityX(int incDec);
	void AddVelocityY(double incDec);
	void AddVelocityY(int incDec);

	void AccelerateX();
	void AccelerateY();
	void DeaccelerateX();
	void DeaccelerateY();

	void MoveTo(double x, double y);
	void MoveTo(std::pair<double, double> newPos);
	SquareSides GetColliderOrientation(Entity* collider);
	virtual void OnMove() {}
public:
	Entity() {}
	Entity(const Entity& src);
	Entity(std::pair<double, double> pos_p, Square collisionBox_p);
	Entity(std::pair<double, double> pos_p, Square collisionBox_p, std::pair<double, double> velocity_p);
	~Entity() {}

	virtual void Update() {}
	void CheckCollision(Entity* collider);
	void MakeIdle() { velocity.first = velocity.second = 0.0; }
	virtual void Move();
	bool IsIdle() { return velocity.first == 0.0 && velocity.second == 0.0; }
	bool IsGrounded() { return grounded; }
	bool IsMoving() { return velocity.first != 0.0 || velocity.second != 0.0; }
	bool Collided() { return collisionBox.colliding; }
	Square& GetCollisionBox() { return collisionBox; }
	virtual void OnCollision(Entity* collider);
	

	std::pair<double, double> GetPos() { return pos; }
	std::pair<double, double> GetDimmensions() { return collisionBox.GetDimmensions(); }
};

