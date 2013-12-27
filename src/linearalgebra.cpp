#include "linearalgebra.hpp"

using namespace lalge;

// =============================================================================
// Stuff
// =============================================================================

Scalar lalge::rad2deg (const Scalar& param)
{
	return ( param * 180 / pi );
}

Scalar lalge::deg2rad (const Scalar& param)
{
	return ( param * pi / 180 );
}

// =============================================================================
// Functions to better handle R2 and R3 Vectors
// =============================================================================

R2Vector lalge::r2vec (const Scalar& x, const Scalar& y)
{
	R2Vector ret;
	
	ret.set ( 0, x );
	ret.set ( 1, y );
	
	return ret;
}

R2Vector lalge::r2vec (const R3Vector& param)
{
	R2Vector ret;
	
	ret.set ( 0, param.x ( 0 ) );
	ret.set ( 1, param.x ( 1 ) );
	
	return ret;
}

R3Vector lalge::r3vec (const Scalar& x, const Scalar& y, const Scalar& z)
{
	R3Vector ret;
	
	ret.set ( 0, x );
	ret.set ( 1, y );
	ret.set ( 2, z );
	
	return ret;
}

R3Vector lalge::r3vec (const R2Vector& param)
{
	R3Vector ret;
	
	ret.set ( 0, param.x ( 0 ) );
	ret.set ( 1, param.x ( 1 ) );
	ret.set ( 2, 0 );
	
	return ret;
}

R3Vector lalge::cross (const R2Vector& v, const R2Vector& w)
{
	R3Vector ret;
	
	ret.set ( 0, 0 );
	ret.set ( 1, 0 );
	ret.set ( 2, ( v.x ( 0 ) * w.x ( 1 ) ) - ( v.x ( 1 ) * w.x ( 0 ) ) );
	
	return ret;
}

R3Vector lalge::cross (const R3Vector& v, const R3Vector& w)
{
	R3Vector ret;
	
	ret.set ( 0, ( v.x ( 1 ) * w.x ( 2 ) ) - ( v.x ( 2 ) * w.x ( 1 ) ) );
	ret.set ( 1, ( v.x ( 2 ) * w.x ( 0 ) ) - ( v.x ( 0 ) * w.x ( 2 ) ) );
	ret.set ( 2, ( v.x ( 0 ) * w.x ( 1 ) ) - ( v.x ( 1 ) * w.x ( 0 ) ) );
	
	return ret;
}

R3Vector lalge::cross (const R2Vector& v, const R3Vector& w)
{
	R3Vector ret;
	
	ret.set ( 0, v.x ( 1 ) * w.x ( 2 ) );
	ret.set ( 1, - ( v.x ( 0 ) * w.x ( 2 ) ) );
	ret.set ( 2, ( v.x ( 0 ) * w.x ( 1 ) ) - ( v.x ( 1 ) * w.x ( 0 ) ) );
	
	return ret;
}

R3Vector lalge::cross (const R3Vector& v, const R2Vector& w)
{
	R3Vector ret;
	
	ret.set ( 0, - ( v.x ( 2 ) * w.x ( 1 ) ) );
	ret.set ( 1, v.x ( 2 ) * w.x ( 0 ) );
	ret.set ( 2, ( v.x ( 0 ) * w.x ( 1 ) ) - ( v.x ( 1 ) * w.x ( 0 ) ) );
	
	return ret;
}

R2Vector lalge::rotate (const Scalar& angle, const R2Vector& v)
{
	if ( !v.length () )
		throw ( RotationNotDefined () );
	
	Scalar tmp = deg2rad ( angle );
	
	return r2vec (	v.x( 0 ) * cos ( tmp ) - v.x( 1 ) * sin ( tmp ),
			v.x( 0 ) * sin ( tmp ) + v.x( 1 ) * cos ( tmp )	);
}

void lalge::rotate (R2Vector& v, const Scalar& angle)
{
	if ( !v.length () )
		throw ( RotationNotDefined () );
	
	Scalar tmp = deg2rad ( angle );
	
	Scalar x = v.x ( 0 );
	Scalar y = v.x ( 1 );
	
	v.set ( 0, x * cos ( tmp ) - y * sin ( tmp ) );
	v.set ( 1, x * sin ( tmp ) + y * cos ( tmp ) );
}

// =============================================================================
// Functions to better handle some special linear operators
// =============================================================================

R2LinearOp lalge::r2lop (	const Scalar& a11, const Scalar& a12,
				const Scalar& a21, const Scalar& a22	)
{
	R2LinearOp::MatrixSrc src;
	
	src.a[0][0] = a11;	src.a[0][1] = a12;
	src.a[1][0] = a21;	src.a[1][1] = a22;
	
	return R2LinearOp ( src );
}
