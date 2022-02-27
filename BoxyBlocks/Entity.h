#pragma once

#include <utility>

enum class CollisionDirections {
	LEFT=1, RIGHT, UP, DOWN
};

class Entity {
protected:
	// first = x, second = y
	std::pair<double, double> pos = std::pair<double, double>(0.0, 0.0);
	std::pair<double, double> velocity = std::pair<double, double>(0.0, 0.0);
	std::pair<double, double> acceleration = std::pair<double, double>(0.0, 0.0);
	std::pair<double, double> dimmensions = std::pair<double, double>(0.0, 0.0);
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
public:
	Entity() {}
	Entity(std::pair<double, double> pos_p, std::pair<double, double> dimmensions_p) : pos(pos_p), dimmensions(dimmensions_p) {}
	Entity(std::pair<double, double> pos_p, std::pair<double, double> dimmensions_p, std::pair<double, double> velocity_p) : pos(pos_p), velocity(velocity_p), dimmensions(dimmensions_p) {}
	~Entity() {}

	virtual void Update() {}
	void CheckCollision(Entity* collider);
	void MakeIdle() { velocity.first = velocity.second = 0.0; }
	virtual void Move();
	bool IsIdle() { return velocity.first == 0.0 && velocity.second == 0.0; }
	bool IsGrounded() { return grounded; }
	virtual void OnCollision(Entity* collider, CollisionDirections collisionLocation);

	std::pair<double, double> GetPos() { return pos; }
	std::pair<double, double> GetDimmensions() { return dimmensions; }

	std::pair<double, double> GetUpperLeftCoord() { return pos; }
	std::pair<double, double> GetUpperRightCoord() { return std::pair<double, double>(pos.first + dimmensions.first, pos.second); }
	std::pair<double, double> GetLowerLeftCoord() { return std::pair<double, double>(pos.first, pos.second + dimmensions.second); }
	std::pair<double, double> GetLowerRightCoord() { return std::pair<double, double>(pos.first + dimmensions.first, pos.second + dimmensions.second); }
};

