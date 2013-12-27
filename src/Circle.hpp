
#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include "Shape.hpp"

class Circle : public Shape {
private:
	lalge::Scalar radius;
public:
	Circle();
	virtual ~Circle();
	
	bool pointInside(lalge::Scalar x, lalge::Scalar y) const;
	
	lalge::Scalar getRadius() const;
	void setRadius(lalge::Scalar radius);
	
	lalge::Scalar area() const;
	lalge::Scalar perimeter() const;
	
	bool collides(const Circle& target) const;
};

#endif
