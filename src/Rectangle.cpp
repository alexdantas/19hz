#include "Rectangle.hpp"

using namespace lalge;

Rectangle::Rectangle(const lalge::R2Vector& pos, lalge::Scalar w, lalge::Scalar h) {
	position = pos;
	width = w;
	height = h;
}

Rectangle::Rectangle() : width(1), height(1) {}
Rectangle::~Rectangle() {}

bool Rectangle::pointInside(Scalar x, Scalar y) const {
	if ((x >= position.x(0) - width/2) && (x < position.x(0) + width/2)) {
		return ((y >= position.x(1) - height/2) && (y < position.x(1) + height/2));
	}
	return false;
}

Scalar Rectangle::getWidth() const { return width; }
void Rectangle::setWidth(Scalar width) { if (width >= 1) this->width = width; }

Scalar Rectangle::getHeight() const { return height; }
void Rectangle::setHeight(Scalar height) { if (height >= 1) this->height = height; }

Scalar Rectangle::area() const { return width*height; }
Scalar Rectangle::perimeter() const { return 2*width + 2*height; }

bool Rectangle::collides(const Rectangle& other) const {
	bool collide_x = false;
	bool collide_y = false;
	R2Vector position = this->position - r2vec(width, height)/2;
	R2Vector other_position = other.position - r2vec(other.width, other.height)/2;
	
	if (position.x(0) <= other_position.x(0)) {
		if (other_position.x(0) < position.x(0) + width)
			collide_x = true;
	}
	else if (position.x(0) < other_position.x(0) + other.width)
		collide_x = true;
	
	if (position.x(1) <= other_position.x(1)) {
		if (other_position.x(1) < position.x(1) + height)
			collide_y = true;
	}
	else if (position.x(0) < other_position.x(0) + other.height)
		collide_y = true;
	
	return (collide_x && collide_y);
}
