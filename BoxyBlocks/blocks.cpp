#include "blocks.h"

void Block::AccelerateX() {
	if (immovable) return;
	velocity.x += acceleration.x;
}

void Block::AccelerateY() {
	if (immovable) return;
	velocity.y += acceleration.y;
}

Square Block::GetAsSquare() {

}
