
#ifndef SHAPE_HPP
#define SHAPE_HPP

#include "linearalgebra.hpp"

class Shape {
public:
	lalge::R2Vector position;
	
	Shape();
	virtual ~Shape();
	
	lalge::R2Vector range(const Shape& target) const;
	lalge::R2Vector range(const lalge::R2Vector& target) const;
	
	virtual bool pointInside(lalge::Scalar x, lalge::Scalar y) const = 0;
	bool pointInside(const lalge::R2Vector& point) const;
	bool mouseInside() const;
	bool mouseDownInside() const;
};

#endif
