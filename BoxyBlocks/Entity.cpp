#include "Entity.h"
#include <functional>

Entity::Entity(std::pair<double, double> pos_p, Square collisionBox_p) : pos(pos_p), collisionBox(collisionBox_p) {
//	pos = std::pair<double, double>(collisionBox.GetUpperLeftPoint().x, collisionBox.GetUpperLeftPoint().y);
	collisionBox.Translate(pos);
}

Entity::Entity(std::pair<double, double> pos_p, Square collisionBox_p, std::pair<double, double> velocity_p) : pos(pos_p), velocity(velocity_p), collisionBox(collisionBox_p) {
//	pos = std::pair<double, double>(collisionBox.GetUpperLeftPoint().x, collisionBox.GetUpperLeftPoint().y);
	collisionBox.Translate(pos);
}

Entity::Entity(const Entity& src) {
	velocity = src.velocity;
	acceleration = src.acceleration;
	collisionBox = src.collisionBox;
	pos = src.pos;
	grounded = src.grounded;
}

void Entity::AddVelocityX(double incDec) {
	velocity.first += incDec;
}

void Entity::AddVelocityX(int incDec) {
	velocity.first += (double)incDec;
}

void Entity::AddVelocityY(double incDec) {
	velocity.second += incDec;
}

void Entity::AddVelocityY(int incDec) {
	velocity.second += (double)incDec;
}

void Entity::AccelerateX() {
	if (velocity.first < 0.0) {
		velocity.first -= acceleration.first;
	}
	else {
		velocity.first += acceleration.first;
	}
}

void Entity::AccelerateY() {
	if (velocity.second < 0.0) {
		velocity.second -= acceleration.second;
	}
	else {
		velocity.second += acceleration.second;
	}
}

void Entity::DeaccelerateX() {
	if (velocity.first < 0.0) {
		velocity.first = velocity.first + acceleration.first > 0.0 ? 0.0 : velocity.first + acceleration.first;
	}
	else {
		velocity.first = velocity.first - acceleration.first < 0.0 ? 0.0 : velocity.first - acceleration.first;
	}
}

void Entity::DeaccelerateY() {
	if (velocity.second < 0.0) {
		velocity.second = velocity.second + acceleration.second > 0.0 ? 0.0 : velocity.second + acceleration.second;
	}
	else {
		velocity.second = velocity.second - acceleration.second < 0.0 ? 0.0 : velocity.second - acceleration.second;
	}
}

void Entity::MoveTo(double x, double y) {
	pos.first = x;
	pos.second = y;
}

void Entity::MoveTo(std::pair<double, double> newPos) {
	pos = newPos;
}

void Entity::CheckCollision(Entity* collider) {
	collisionBox.CheckCollision(&(collider->GetCollisionBox()));
	if (collisionBox.colliding) {
		OnCollision(collider);
		return;
	}

	if (velocity.first != 0.0 || velocity.second != 0.0) {
		Square stretchedCollisionBox = Square(collisionBox);
		stretchedCollisionBox.IncDecSquarePoints(velocity);
		stretchedCollisionBox.CheckCollision(&(collider->GetCollisionBox()));
		if (stretchedCollisionBox.colliding) {
			collisionBox.collisionInfo = stretchedCollisionBox.collisionInfo;
			OnCollision(collider);
		}
		else {
			Move();
		}
	}
}

SquareSides Entity::GetColliderOrientation(Entity* collider) {
	Line line = Line(pos.first, pos.second, collider->GetPos().first, collider->GetPos().second);
	
	if (abs(line.rise) < (line.run)) {
		return line.rise < 0.0 ? SquareSides::BOTTOM : SquareSides::TOP;
	}
	else {
		return line.run < 0.0 ? SquareSides::LEFT : SquareSides::RIGHT;
	}
}

void Entity::OnCollision(Entity* collider) {
	auto& collidingLines = collisionBox.collisionInfo.collidingLines;
	auto& pois = collisionBox.collisionInfo.pointsOfIntersection;
	SquareSides colliderOrientation = GetColliderOrientation(collider);
	switch (colliderOrientation) {
	case SquareSides::BOTTOM:
		velocity.second = 0.0;
		pos.second = collider->GetPos().second - 0.1;
		grounded = true;
		break;
	case SquareSides::TOP:
		velocity.second = 0.0;
		pos.second = collider->GetPos().second + collider->GetDimmensions().second + 0.1;
		break;
	case SquareSides::LEFT:
		velocity.first = 0.0;
		pos.first = collider->GetPos().first + collider->GetDimmensions().first + 0.1;
		break;
	case SquareSides::RIGHT:
		velocity.first = 0.0;
		pos.first = collider->GetPos().first - 0.1;
		break;
	}
	/*for (int i = 0; i < collidingLines.size(); i++) {
		switch ((SquareSides)collidingLines[i]) {
		case SquareSides::BOTTOM:
			if (IsMoving()) {
				
			}
			velocity.second = 0.0;
			pos.second -= 0.1;
			grounded = true;
			break;
		case SquareSides::TOP:
			if (collider != NULL) {

			}
			pos.second += 0.1;
			break;
		case SquareSides::LEFT:
			if (collider != NULL) {

			}
			velocity.first = 0.0;
			pos.first += 0.1;
			break;
		case SquareSides::RIGHT:
			if (collider != NULL) {

			}
			velocity.first = 0.0;
			pos.first -= 0.1;
			break;
		}
	}*/
	collisionBox.Translate(pos);
}

void Entity::Move() {
	if (IsIdle()) {
		return;
	}
	OnMove();
	pos.first += velocity.first;
	pos.second += velocity.second;
	collisionBox.Translate(pos);
}