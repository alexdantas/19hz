#include "Shape.hpp"

#include "InputManager.hpp"

Shape::Shape() {}
Shape::~Shape() {}

lalge::R2Vector Shape::range(const Shape& target) const {
	return (target.position - position);
}

lalge::R2Vector Shape::range(const lalge::R2Vector& target) const {
	return (target - position);
}

bool Shape::pointInside(const lalge::R2Vector& point) const {
	return pointInside(point.x(0), point.x(1));
}

bool Shape::mouseInside() const {
	return pointInside(InputManager::instance()->mouseX(), InputManager::instance()->mouseY());
}

bool Shape::mouseDownInside() const {
	return pointInside(InputManager::instance()->mouseDownX(), InputManager::instance()->mouseDownY());
}
