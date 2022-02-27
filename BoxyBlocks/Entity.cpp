#include "Entity.h"
#include <functional>

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

void Entity::CheckCollision(Entity* collider) {
	// Everything in this game is a square. So basic square collision should work.
	std::vector<std::function<std::pair<double, double>()>>  coordFunctions;
	coordFunctions.push_back([this]() { return GetUpperLeftCoord(); });
	coordFunctions.push_back([this]() { return GetUpperRightCoord(); });
	coordFunctions.push_back([this]() { return GetLowerLeftCoord(); });
	coordFunctions.push_back([this]() { return GetLowerRightCoord(); });

	for (auto function : coordFunctions) {
		std::pair<double, double> corner = function();
		if (corner.first >= collider->GetUpperLeftCoord().first && corner.first <= collider->GetUpperRightCoord().first && corner.second >= collider->GetUpperLeftCoord().second && corner.second <= collider->GetLowerRightCoord().second) {
			double bottomSideDiff = abs(GetLowerLeftCoord().second - collider->GetUpperLeftCoord().second);
			double topSideDiff = abs(GetUpperLeftCoord().second - collider->GetLowerLeftCoord().second);
			double leftSideDiff = abs(GetUpperLeftCoord().first - collider->GetUpperRightCoord().first);
			double rightSideDiff = abs(GetUpperRightCoord().first - collider->GetUpperLeftCoord().first);
			double min = std::min(std::min(bottomSideDiff, topSideDiff), std::min(leftSideDiff, rightSideDiff));

			CollisionDirections collisionLocation = CollisionDirections::LEFT;

			if (bottomSideDiff == min) {
				collisionLocation = CollisionDirections::DOWN;
				pos.second -= (bottomSideDiff + 0.1);
			}
			if (topSideDiff == min) {
				collisionLocation = CollisionDirections::UP;
				pos.second += (topSideDiff + 0.1);
			}
			if(rightSideDiff == min) {
				collisionLocation = CollisionDirections::RIGHT;
				pos.first -= (rightSideDiff + 0.1);
			}
			if(leftSideDiff == min) {
				pos.first += (leftSideDiff + 0.1);
			}

			OnCollision(collider, collisionLocation);
			break;
		}
	}
}

void Entity::OnCollision(Entity* collider, CollisionDirections collisionLocation) {
	//MakeIdle();
	CollisionDirections colliderCollisionSide = CollisionDirections::LEFT;
	switch (collisionLocation) {
	case CollisionDirections::LEFT:
		colliderCollisionSide = CollisionDirections::RIGHT;
		velocity.first = 0.0;
		break;
	case CollisionDirections::UP:
		colliderCollisionSide = CollisionDirections::DOWN;
		velocity.second = 0.0;
		break;
	case CollisionDirections::DOWN:
		colliderCollisionSide = CollisionDirections::UP;
		velocity.second = 0.0;
		grounded = true;
		break;
	case CollisionDirections::RIGHT:
		colliderCollisionSide = CollisionDirections::LEFT;
		velocity.first = 0.0;
		break;
	}
	if (collider != NULL) {
		//collider->MakeIdle();
		OnCollision(NULL, colliderCollisionSide);
	}
}

void Entity::Move() {
	if (IsIdle()) {
		return;
	}
	pos.first += velocity.first;
	pos.second += velocity.second;
}