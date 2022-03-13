#include "Entity.h"
#include <functional>

Entity::Entity(XY pos_p, Square collisionBox_p) : collisionBox(collisionBox_p) {
//	pos = std::pair<double, double>(collisionBox.GetUpperLeftPoint().x, collisionBox.GetUpperLeftPoint().y);
	((Polygon)collisionBox).Translate(pos_p);
	//collisionBox.SetMass();
}

Entity::Entity(XY pos_p, Square collisionBox_p, XY velocity_p) : collisionBox(collisionBox_p) {
//	pos = std::pair<double, double>(collisionBox.GetUpperLeftPoint().x, collisionBox.GetUpperLeftPoint().y);
	((Polygon)collisionBox).Translate(pos_p);
	//collisionBox.SetMass();
}

Entity::Entity(const Entity& src) {
	//velocity = src.velocity;
	//acceleration = src.acceleration;
	collisionBox = src.collisionBox;
	//collisionBox.SetMass();
}

void Entity::MoveTo(double x, double y) {
	((Polygon)collisionBox).Translate(XY(x,y));
}

void Entity::MoveTo(XY newPos) {
	((Polygon)collisionBox).Translate(newPos);
}

void Entity::CheckCollision(Entity* collider) {
	std::vector<void*> onCollisionArgs;
	collisionBox.CheckCollision(collider->GetCollisionBox(), velocity, onCollisionArgs);
}

//SquareSides Entity::GetCollidingDirection(Entity* collider) {
//	Line line = Line(pos.first, pos.second, collider->GetPos().first, collider->GetPos().second);
//	
//	if (abs(line.rise) > abs(line.run)) {
//		return line.rise < 0.0 ? SquareSides::TOP : SquareSides::BOTTOM;
//	}
//	else {
//		return line.run < 0.0 ? SquareSides::LEFT : SquareSides::RIGHT;
//	}
//}

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