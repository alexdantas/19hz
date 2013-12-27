
#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "Shape.hpp"

class Rectangle : public Shape {
private:
	lalge::Scalar width;
	lalge::Scalar height;
public:
	Rectangle(const lalge::R2Vector& pos, lalge::Scalar w, lalge::Scalar h);
	Rectangle();
	virtual ~Rectangle();
	
	bool pointInside(lalge::Scalar x, lalge::Scalar y) const;
	
	lalge::Scalar getWidth() const;
	void setWidth(lalge::Scalar width);
	
	lalge::Scalar getHeight() const;
	void setHeight(lalge::Scalar height);
	
	lalge::Scalar area() const;
	lalge::Scalar perimeter() const;
	
	bool collides(const Rectangle& other) const;
};

#endif
