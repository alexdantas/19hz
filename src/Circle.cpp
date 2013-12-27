#include "Circle.hpp"

using namespace lalge;

Circle::Circle() : radius(1) {}
Circle::~Circle() {}

bool Circle::pointInside(Scalar x, Scalar y) const {
	return ((position - r2vec(x, y)).size() <= radius);
}

Scalar Circle::getRadius() const { return radius; }
void Circle::setRadius(Scalar radius) { if (radius >= 1) this->radius = radius; }

Scalar Circle::area() const { return pi*radius*radius; }
Scalar Circle::perimeter() const { return 2*pi*radius; }

bool Circle::collides(const Circle& target) const {
	return (ceil(radius + target.radius) >= floor(range(target).size()));
}
