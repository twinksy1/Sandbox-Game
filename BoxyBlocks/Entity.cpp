#include "Entity.h"
#include <functional>

Entity::Entity(std::pair<double, double> pos_p, Square collisionBox_p) : collisionBox(collisionBox_p) {
//	pos = std::pair<double, double>(collisionBox.GetUpperLeftPoint().x, collisionBox.GetUpperLeftPoint().y);
	collisionBox.Translate(pos_p);
	collisionBox.SetMass();
}

Entity::Entity(std::pair<double, double> pos_p, Square collisionBox_p, std::pair<double, double> velocity_p) : collisionBox(collisionBox_p) {
//	pos = std::pair<double, double>(collisionBox.GetUpperLeftPoint().x, collisionBox.GetUpperLeftPoint().y);
	collisionBox.SetVelocity(velocity_p);
	collisionBox.Translate(pos_p);
	collisionBox.SetMass();
}

Entity::Entity(const Entity& src) {
	//velocity = src.velocity;
	//acceleration = src.acceleration;
	collisionBox = src.collisionBox;
	collisionBox.SetMass();
}

void Entity::MoveTo(double x, double y) {
	collisionBox.Translate(std::pair<double, double>(x,y));
}

void Entity::MoveTo(std::pair<double, double> newPos) {
	collisionBox.Translate(newPos);
}

void Entity::CheckCollision(Entity* collider) {
	collisionBox.CheckCollision(&(collider->GetCollisionBox()));
}

SquareSides Entity::GetCollidingDirection(Entity* collider) {
	Line line = Line(pos.first, pos.second, collider->GetPos().first, collider->GetPos().second);
	
	if (abs(line.rise) > abs(line.run)) {
		return line.rise < 0.0 ? SquareSides::TOP : SquareSides::BOTTOM;
	}
	else {
		return line.run < 0.0 ? SquareSides::LEFT : SquareSides::RIGHT;
	}
}

void Entity::OnCollision(Entity* collider) {

}

void Entity::ResolveCollisions() {

}

void Entity::Rotate(double angle) {
	collisionBox.Rotate(angle);
	//pos = std::pair<double, double>(collisionBox.GetUpperLeftPoint().x, collisionBox.GetUpperLeftPoint().y);
}

void Entity::Move() {
	//if (IsIdle()) {
	//	return;
	//}

	OnMove();
	collisionBox.Move();
	//pos.first += velocity.first;
	//pos.second += velocity.second;
	//collisionBox.Translate(pos);
}