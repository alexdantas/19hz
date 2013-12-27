#ifndef LINEARALGEBRA_HPP
#define LINEARALGEBRA_HPP

#include <cmath>

#define SQR(X)		((X)*(X))
#define ABSVAL(X)	(((X) >= 0) ? (X) : (-(X)))

namespace lalge
{

// =============================================================================
// Stuff
// =============================================================================

typedef float Scalar;

const Scalar pi = acos ( -1 );

Scalar rad2deg (const Scalar& param);
Scalar deg2rad (const Scalar& param);

// =============================================================================
// Exceptions
// =============================================================================

class VectorNotDefined {};
class NotZeroAssignment {};
class DivisionByZero {};
class AngleNotDefined {};
class DirectionNotDefined {};
class ProjectionNotDefined {};
class ScalarProjNotDefined {};
class RotationNotDefined {};
class MatrixNotDefined {};
class PowerNotDefined {};
class CofactorNotDefined {};
class DetNotDefined {};
class AdjugateNotDefined {};
class InverseNotDefined {};

// =============================================================================
// Vector of Euclidean Space Template
// =============================================================================

template <unsigned int n>
class RnVector
{
public:
	struct ArraySrc
	{
		Scalar x[n];
		ArraySrc ()
		{
			for ( unsigned int i = 0; i < n; i++ )
				x[i] = 0;
		}
	};
private:
	typename RnVector< n >::ArraySrc src_;
	Scalar length_;
public:
	RnVector ();
	RnVector (const RnVector< n >::ArraySrc& src);
	~RnVector ();
	
	void annul ();
	
	const Scalar& x (const unsigned int& i) const;
	typename RnVector< n >::ArraySrc src () const;
	void set (const unsigned int& i, const Scalar& param);
	void add (const unsigned int& i, const Scalar& param);
	
	bool operator== (const RnVector< n >& param) const;
	bool operator!= (const RnVector< n >& param) const;
	
	bool operator> (const RnVector< n >& param) const;
	bool operator< (const RnVector< n >& param) const;
	
	const Scalar& length () const;
	const Scalar& size () const;
	
	RnVector< n > opposite () const;
	RnVector< n > operator- () const;
	
	RnVector<n>& operator=(int zero);
	
	RnVector< n >& operator+= (const RnVector< n >& param);
	RnVector< n > operator+ (const RnVector< n >& param) const;
	
	RnVector< n >& operator-= (const RnVector< n >& param);
	RnVector< n > operator- (const RnVector< n >& param) const;
	
	RnVector< n >& operator*= (const Scalar& param);
	RnVector< n > operator* (const Scalar& param) const;
	
	RnVector< n >& operator/= (const Scalar& param);
	RnVector< n > operator/ (const Scalar& param) const;
	
	Scalar inner (const RnVector< n >& param) const;
	Scalar operator* (const RnVector< n >& param) const;
	
	Scalar angle (const RnVector< n >& param) const;
	Scalar operator% (const RnVector< n >& param) const;
	
	RnVector< n > unitvec () const;
	RnVector< n > proj (const RnVector< n >& param) const;
	Scalar scalarProj (const RnVector< n >& param) const;
};

// =============================================================================
// Some specializations of Rn Vector Template
// =============================================================================

typedef RnVector< 2 > R2Vector;
typedef RnVector< 3 > R3Vector;

// =============================================================================
// Functions to better handle R2 and R3 Vectors
// =============================================================================

R2Vector r2vec (const Scalar& x, const Scalar& y);
R2Vector r2vec (const R3Vector& param);
R3Vector r3vec (const Scalar& x, const Scalar& y, const Scalar& z);
R3Vector r3vec (const R2Vector& param);
R3Vector cross (const R2Vector& v, const R2Vector& w);
R3Vector cross (const R3Vector& v, const R3Vector& w);
R3Vector cross (const R2Vector& v, const R3Vector& w);
R3Vector cross (const R3Vector& v, const R2Vector& w);
R2Vector rotate (const Scalar& angle, const R2Vector& v);
void rotate (R2Vector& v, const Scalar& angle);

// =============================================================================
// Linear Transformation (Matrix) Template
// =============================================================================

template <unsigned int m, unsigned int n>
class LinearMap
{
public:
	struct MatrixSrc
	{
		Scalar a[m][n];
		
		MatrixSrc ()
		{
			for ( unsigned int i = 0; i < m; i++ )
			{
				for ( unsigned int j = 0; j < n; j++ )
					a[i][j] = 0;
			}
		}
	};
	struct VecSetSrc { RnVector< m > v[n]; };
private:
	class NullDeterminant {};
	
	typename LinearMap< m, n >::MatrixSrc src_;
	Scalar det_;
public:
	LinearMap ();
	LinearMap (const LinearMap< m, n >::MatrixSrc& src);
	LinearMap (const LinearMap< m, n >::VecSetSrc& src);
	~LinearMap ();
private:
	void calc_det ();
	Scalar calc_mxm () const;
	unsigned int find_zeros () const;
public:
	void annul ();
	
	const Scalar& a (const unsigned int& i, const unsigned int& j) const;
	void set (
		const unsigned int& i,
		const unsigned int& j,
		const Scalar& param
	);
	
	bool operator== (const LinearMap< m, n >& param) const;
	bool operator!= (const LinearMap< m, n >& param) const;
	
	LinearMap< m, n > opposite () const;
	LinearMap< m, n > operator- () const;
	
	LinearMap< m, n >& operator+= (const LinearMap< m, n >& param);
	LinearMap< m, n > operator+ (const LinearMap< m, n >& param) const;
	
	LinearMap< m, n >& operator-= (const LinearMap< m, n >& param);
	LinearMap< m, n > operator- (const LinearMap< m, n >& param) const;
	
	LinearMap< m, n >& operator*= (const Scalar& param);
	LinearMap< m, n > operator* (const Scalar& param) const;
	
	LinearMap< m, n >& operator/= (const Scalar& param);
	LinearMap< m, n > operator/ (const Scalar& param) const;
	
	template <unsigned int c>
	LinearMap< m, c > operator* (const LinearMap< n, c >& param) const;
	LinearMap< m, n >& operator*= (const LinearMap< n, n >& param);
	RnVector< m > operator* (const RnVector< n >& param) const;
	
	LinearMap< m, m > power (const unsigned int& param) const;
	LinearMap< m, m > operator^ (const unsigned int& param) const;
	LinearMap< m, n >& operator^= (const unsigned int& param);
	
	Scalar cofactor (const unsigned int& i, const unsigned int& j) const;
	
	const Scalar& det () const;
	
	LinearMap< n, m > transpose () const;
	LinearMap< m, m > adjugate () const;
	LinearMap< m, m > inverse () const;
};

// =============================================================================
// "Static" Linear Transformation Methods
// =============================================================================

template <unsigned int m>
LinearMap< m, m > identity ();

class SubMatrixNotDefined {};

#define SUBSIZE(X)	( ( (X) > 2 ) ? (X) - 1 : 1 )

template <unsigned int m, unsigned int n>
LinearMap< SUBSIZE(m), SUBSIZE(n) > submatrix (
	const LinearMap< m, n >& param,
	const unsigned int& i,
	const unsigned int& j
);

// =============================================================================
// Rn Linear Operator (Square Matrix) Template
// =============================================================================

template <unsigned int n>
struct RnLinearOp
{
	typedef LinearMap< n, n > type;
};

// =============================================================================
// Some specializations of Rn Linear Operator Template
// =============================================================================

typedef RnLinearOp< 2 >::type R2LinearOp;
typedef RnLinearOp< 3 >::type R3LinearOp;

// =============================================================================
// Functions to better handle some special linear operators
// =============================================================================

R2LinearOp r2lop (
	const Scalar& a11, const Scalar& a12,
	const Scalar& a21, const Scalar& a22
);

// =============================================================================
// Implementations of RnVector Methods
// =============================================================================

template <unsigned int n>
RnVector< n >::RnVector ()
{
	if ( !n )
		throw ( VectorNotDefined () );
	
	length_ = 0;
}

template <unsigned int n>
RnVector< n >::RnVector (const RnVector< n >::ArraySrc& src)
{
	if ( !n )
		throw ( VectorNotDefined () );
	
	length_ = 0;
	for ( unsigned int i = 0; i < n; i++ )
	{
		src_.x[i] = src.x[i];
		length_ += ( src_.x[i] * src_.x[i] );
	}
	length_ = sqrt ( length_ );
}

template <unsigned int n>
RnVector< n >::~RnVector ()
{
}

template <unsigned int n>
void RnVector< n >::annul ()
{
	if ( length_ )
	{
		for ( unsigned int i = 0; i < n; i++ )
			src_.x[i] = 0;
		
		length_ = 0;
	}
}

template <unsigned int n>
const Scalar& RnVector< n >::x (const unsigned int& i) const
{
	return src_.x[i];
}

template <unsigned int n>
typename RnVector< n >::ArraySrc RnVector< n >::src () const
{
	return src_;
}

template <unsigned int n>
void RnVector< n >::set (const unsigned int& i, const Scalar& param)
{
	length_ *= length_;
	length_ -= ( src_.x[i] * src_.x[i] );
	
	src_.x[i] = param;
	
	length_ += ( src_.x[i] * src_.x[i] );
	length_ = sqrt ( length_ );
}

template <unsigned int n>
void RnVector< n >::add (const unsigned int& i, const Scalar& param)
{
	length_ *= length_;
	length_ -= ( src_.x[i] * src_.x[i] );
	
	src_.x[i] += param;
	
	length_ += ( src_.x[i] * src_.x[i] );
	length_ = sqrt ( length_ );
}

template <unsigned int n>
bool RnVector< n >::operator== (const RnVector< n >& param) const
{
	if ( length_ != param.length () )
		return false;
	
	for ( unsigned int i = 0; i < n; i++ )
	{
		if ( src_.x[i] != param.x ( i ) )
			return false;
	}
	
	return true;
}

template <unsigned int n>
bool RnVector< n >::operator!= (const RnVector< n >& param) const
{
	return ( !( (*this) == param ) );
}

template <unsigned int n>
bool RnVector< n >::operator> (const RnVector< n >& param) const
{
	return ( length_ > param.length () );
}

template <unsigned int n>
bool RnVector< n >::operator< (const RnVector< n >& param) const
{
	return ( length_ < param.length () );
}

template <unsigned int n>
const Scalar& RnVector< n >::length () const
{
	return length_;
}

template <unsigned int n>
const Scalar& RnVector< n >::size () const
{
	return length_;
}

template <unsigned int n>
RnVector< n > RnVector< n >::opposite () const
{
	if ( !length_ )
		return *this;
	
	RnVector< n > ret;
	
	for ( unsigned int i = 0; i < n; i++ )
		ret.set ( i, -src_.x[i] );
	
	return ret;
}

template <unsigned int n>
RnVector< n > RnVector< n >::operator- () const
{
	return opposite ();
}

template <unsigned int n>
RnVector<n>& RnVector<n>::operator=(int zero) {
	if (zero)
		throw NotZeroAssignment();
	annul();
	return *this;
}

template <unsigned int n>
RnVector< n >& RnVector< n >::operator+= (const RnVector< n >& param)
{
	if ( !param.length () )
		return *this;
	
	if ( !length_ )
	{
		*this = param;
		return *this;
	}
	
	length_ = 0;
	
	for ( unsigned int i = 0; i < n; i++ )
	{
		src_.x[i] += param.x ( i );
		length_ += ( src_.x[i] * src_.x[i] );
	}
	
	length_ = sqrt ( length_ );
	
	return *this;
}

template <unsigned int n>
RnVector< n > RnVector< n >::operator+ (const RnVector< n >& param) const
{
	if ( !param.length () )
		return *this;
	
	if ( !length_ )
		return param;
	
	RnVector< n > ret;
	
	for ( unsigned int i = 0; i < n; i++ )
		ret.set ( i, src_.x[i] + param.x ( i ) );
	
	return ret;
}

template <unsigned int n>
RnVector< n >& RnVector< n >::operator-= (const RnVector< n >& param)
{
	if ( !param.length () )
		return *this;
	
	length_ = 0;
	
	for ( unsigned int i = 0; i < n; i++ )
	{
		src_.x[i] -= param.x ( i );
		length_ += ( src_.x[i] * src_.x[i] );
	}
	
	length_ = sqrt ( length_ );
	
	return *this;
}

template <unsigned int n>
RnVector< n > RnVector< n >::operator- (const RnVector< n >& param) const
{
	if ( !param.length () )
		return *this;
	
	if ( !length_ )
		return ( -param );
	
	RnVector< n > ret;
	
	for ( unsigned int i = 0; i < n; i++ )
		ret.set ( i, src_.x[i] - param.x ( i ) );
	
	return ret;
}

template <unsigned int n>
RnVector< n >& RnVector< n >::operator*= (const Scalar& param)
{
	if ( !length_ )
		return *this;
	
	for ( unsigned int i = 0; i < n; i++ )
		src_.x[i] *= param;
	
	length_ *= param;
	
	return *this;
}

template <unsigned int n>
RnVector< n > RnVector< n >::operator* (const Scalar& param) const
{
	if ( ( !param ) || ( !length_ ) )
		return RnVector< n > ();
	
	RnVector< n > ret;
	
	for ( unsigned int i = 0; i < n; i++ )
		ret.set ( i, src_.x[i] * param );
	
	return ret;
}

template <unsigned int n>
RnVector< n >& RnVector< n >::operator/= (const Scalar& param)
{
	if ( !param )
		throw ( DivisionByZero () );
	
	return ( (*this) *= ( 1 / param ) );
}

template <unsigned int n>
RnVector< n > RnVector< n >::operator/ (const Scalar& param) const
{
	if ( !param )
		throw ( DivisionByZero () );
	
	return ( (*this) * ( 1 / param ) );
}

template <unsigned int n>
Scalar RnVector< n >::inner (const RnVector< n >& param) const
{
	if ( ( !param.length () ) || ( !length_ ) )
		return 0;
	
	Scalar ret = 0;
	
	for ( unsigned int i = 0; i < n; i++ )
		ret += ( src_.x[i] * param.x ( i ) );
	
	return ret;
}

template <unsigned int n>
Scalar RnVector< n >::operator* (const RnVector< n >& param) const
{
	return inner ( param );
}

template <unsigned int n>
Scalar RnVector< n >::angle (const RnVector< n >& param) const
{
	if ( ( !length_ ) || ( !param.length () ) )
		throw ( AngleNotDefined () );
	
	Scalar ret = ( (*this) * param );
	
	return rad2deg ( acos ( ret / ( length_ * param.length () ) ) );
}

template <unsigned int n>
Scalar RnVector< n >::operator% (const RnVector< n >& param) const
{
	return angle ( param );
}

template <unsigned int n>
RnVector< n > RnVector< n >::unitvec () const
{
	if ( !length_ )
		throw ( DirectionNotDefined () );
	else if ( length_ == 1 )
		return *this;
	
	return ( (*this) / length_ );
}

template <unsigned int n>
RnVector< n > RnVector< n >::proj (const RnVector< n >& param) const
{
	if ( !length_ )
		throw ( ProjectionNotDefined () );
	
	Scalar ret = ( (*this) * param );
	
	ret /= ( length_*length_ );
	
	return ( (*this) * ret );
}

template <unsigned int n>
Scalar RnVector< n >::scalarProj (const RnVector< n >& param) const
{
	if ( !length_ )
		throw ( ScalarProjNotDefined () );
	
	Scalar ret = 0;
	
	for ( unsigned int i = 0; i < n; i++ )
		ret += ( ( src_.x[i] / length_ ) * param.x ( i ) );
	
	return ret;
}

// =============================================================================
// Implementations of LinearMap Methods
// =============================================================================

template <unsigned int m, unsigned int n>
LinearMap< m, n >::LinearMap ()
{
	if ( ( !m ) || ( !n ) )
		throw ( MatrixNotDefined () );
	
	det_ = 0;
}

template <unsigned int m, unsigned int n>
LinearMap< m, n >::LinearMap (const LinearMap< m, n >::MatrixSrc& src)
{
	if ( ( !m ) || ( !n ) )
		throw ( MatrixNotDefined () );
	
	src_ = src;
	
	calc_det ();
}

template <unsigned int m, unsigned int n>
LinearMap< m, n >::LinearMap (const LinearMap< m, n >::VecSetSrc& src)
{
	if ( ( !m ) || ( !n ) )
		throw ( MatrixNotDefined () );
	
	for ( unsigned int i = 0; i < m; i++ )
	{
		for ( unsigned int j = 0; j < n; j++ )
			src_.a[i][j] = src.v[j].x ( i );
	}
	
	calc_det ();
}

template <unsigned int m, unsigned int n>
LinearMap< m, n >::~LinearMap ()
{
}

template <unsigned int m, unsigned int n>
void LinearMap< m, n >::calc_det ()
{
	if ( m != n )
	{
		det_ = 0;
	}
	else if ( m == 1 )
	{
		det_  = ( src_.a[0][0] );
	}
	else if ( m == 2 )
	{
		det_  = ( src_.a[0][0] * src_.a[1][1] );
		det_ -= ( src_.a[0][1] * src_.a[1][0] );
	}
	else if ( m == 3 )
	{
		det_  = ( src_.a[0][0] * src_.a[1][1] * src_.a[2][2] );
		det_ += ( src_.a[0][1] * src_.a[1][2] * src_.a[2][0] );
		det_ += ( src_.a[0][2] * src_.a[1][0] * src_.a[2][1] );
		det_ -= ( src_.a[0][2] * src_.a[1][1] * src_.a[2][0] );
		det_ -= ( src_.a[0][0] * src_.a[1][2] * src_.a[2][1] );
		det_ -= ( src_.a[0][1] * src_.a[1][0] * src_.a[2][2] );
	}
	else
	{
		try {
			det_ = calc_mxm ();
		}
		catch (NullDeterminant& e)
		{
			det_ = 0;
		}
	}
}

template <unsigned int m, unsigned int n>
Scalar LinearMap< m, n >::calc_mxm () const
{
	Scalar ret = 0;
	
	unsigned int i = find_zeros ();
	
	for ( unsigned int j = 0; j < m; j++ )
	{
		if ( src_.a[i][j] )
			ret += ( src_.a[i][j] * cofactor ( i, j ) );
	}
	
	return ret;
}

template <unsigned int m, unsigned int n>
unsigned int LinearMap< m, n >::find_zeros () const
{
	unsigned int count = 0;
	unsigned int index = 0;
	
	for ( unsigned int i = 0; i < m; i++ )
	{
		unsigned int tmp_row = 0;
		unsigned int tmp_col = 0;
		
		for ( unsigned int j = 0; j < m; j++ )
		{
			if ( !src_.a[i][j] )
				tmp_row++;
			
			if ( !src_.a[j][i] )
				tmp_col++;
		}
		
		if ( ( tmp_row == m ) || ( tmp_col == m ) )
			throw ( NullDeterminant () );
		
		if ( tmp_row > count )
		{
			count = tmp_row;
			index = i;
		}
	}
	
	return index;
}

template <unsigned int m, unsigned int n>
void LinearMap< m, n >::annul ()
{
	for ( unsigned int i = 0; i < m; i++ )
	{
		for ( unsigned int j = 0; j < n; j++ )
			src_.a[i][j] = 0;
	}
	
	if ( m == n )
		det_ = 0;
}

template <unsigned int m, unsigned int n>
const Scalar& LinearMap< m, n >::a (
	const unsigned int& i,
	const unsigned int& j
) const
{
	return src_.a[i][j];
}

template <unsigned int m, unsigned int n>
void LinearMap< m, n >::set (	const unsigned int& i,
				const unsigned int& j,
				const Scalar& param	)
{
	src_.a[i][j] = param;
	
	calc_det ();
}

template <unsigned int m, unsigned int n>
bool LinearMap< m, n >::operator== (const LinearMap< m, n >& param) const
{
	if ( m == n )
	{
		if ( det_ != param.det () )
			return false;
	}
	
	for ( unsigned int i = 0; i < m; i++ )
	{
		for ( unsigned int j = 0; j < n; j++ )
		{
			if ( src_.a[i][j] != param.a ( i, j ) )
				return false;
		}
	}
	
	return true;
}

template <unsigned int m, unsigned int n>
bool LinearMap< m, n >::operator!= (const LinearMap< m, n >& param) const
{
	return ( !( (*this) == param ) );
}

template <unsigned int m, unsigned int n>
LinearMap< m, n > LinearMap< m, n >::opposite () const
{
	typename LinearMap< m, n >::MatrixSrc src;
	
	for ( unsigned int i = 0; i < m; i++ )
	{
		for ( unsigned int j = 0; j < n; j++ )
			src.a[i][j] = ( -src_.a[i][j] );
	}
	
	return LinearMap< m, n > ( src );
}

template <unsigned int m, unsigned int n>
LinearMap< m, n > LinearMap< m, n >::operator- () const
{
	return opposite ();
}

template <unsigned int m, unsigned int n>
LinearMap< m, n >& LinearMap< m, n >::operator+= (
	const LinearMap< m, n >& param
)
{
	for ( unsigned int i = 0; i < m; i++ )
	{
		for ( unsigned int j = 0; j < n; j++ )
			src_.a[i][j] += param.a ( i, j );
	}
	
	calc_det ();
	
	return *this;
}

template <unsigned int m, unsigned int n>
LinearMap< m, n > LinearMap< m, n >::operator+ (
	const LinearMap< m, n >& param
) const
{
	typename LinearMap< m, n >::MatrixSrc src;
	
	for ( unsigned int i = 0; i < m; i++ )
	{
		for ( unsigned int j = 0; j < n; j++ )
			src.a[i][j] = ( src_.a[i][j] + param.a ( i, j ) );
	}
	
	return LinearMap< m, n > ( src );
}

template <unsigned int m, unsigned int n>
LinearMap< m, n >& LinearMap< m, n >::operator-= (
	const LinearMap< m, n >& param
)
{
	for ( unsigned int i = 0; i < m; i++ )
	{
		for ( unsigned int j = 0; j < n; j++ )
			src_.a[i][j] -= param.a ( i, j );
	}
	
	calc_det ();
	
	return *this;
}

template <unsigned int m, unsigned int n>
LinearMap< m, n > LinearMap< m, n >::operator- (
	const LinearMap< m, n >& param
) const
{
	typename LinearMap< m, n >::MatrixSrc src;
	
	for ( unsigned int i = 0; i < m; i++ )
	{
		for ( unsigned int j = 0; j < n; j++ )
			src.a[i][j] = ( src_.a[i][j] - param.a ( i, j ) );
	}
	
	return LinearMap< m, n > ( src );
}

template <unsigned int m, unsigned int n>
LinearMap< m, n >& LinearMap< m, n >::operator*= (const Scalar& param)
{
	for ( unsigned int i = 0; i < m; i++ )
	{
		for ( unsigned int j = 0; j < n; j++ )
			src_.a[i][j] *= param;
	}
	
	if ( m == n )
		det_ *= pow ( param, m );
	
	return *this;
}

template <unsigned int m, unsigned int n>
LinearMap< m, n > LinearMap< m, n >::operator* (const Scalar& param) const
{
	if ( !param )
		return LinearMap< m, n > ();
	
	typename LinearMap< m, n >::MatrixSrc src;
	
	for ( unsigned int i = 0; i < m; i++ )
	{
		for ( unsigned int j = 0; j < n; j++ )
			src.a[i][j] = ( src_.a[i][j] * param );
	}
	
	return LinearMap< m, n > ( src );
}

template <unsigned int m, unsigned int n>
LinearMap< m, n >& LinearMap< m, n >::operator/= (const Scalar& param)
{
	if ( !param )
		throw ( DivisionByZero () );
	
	return ( (*this) *= ( 1 / param ) );
}

template <unsigned int m, unsigned int n>
LinearMap< m, n > LinearMap< m, n >::operator/ (const Scalar& param) const
{
	if ( !param )
		throw ( DivisionByZero () );
	
	return ( (*this) * ( 1 / param ) );
}

template <unsigned int m, unsigned int n>
template <unsigned int c>
LinearMap< m, c > LinearMap< m, n >::operator* (
	const LinearMap< n, c >& param
) const
{
	typename LinearMap< m, c >::MatrixSrc src;
	
	for ( unsigned int i = 0; i < m; i++ )
	{
		for ( unsigned int j = 0; j < c; j++ )
		{
			Scalar sum = 0;
			
			for ( unsigned int k = 0; k < n; k++ )
				sum += ( src_.a[i][k] * param.a ( k, j ) );
			
			src.a[i][j] = sum;
		}
	}
	
	return LinearMap< m, c > ( src );
}

template <unsigned int m, unsigned int n>
LinearMap< m, n >& LinearMap< m, n >::operator*= (
	const LinearMap< n, n >& param
)
{
	*this = ( (*this) * param );
	
	return *this;
}

template <unsigned int m, unsigned int n>
RnVector< m > LinearMap< m, n >::operator* (const RnVector< n >& param) const
{
	if ( !param.length () )
		return RnVector< m > ();
	
	typename RnVector< m >::ArraySrc src;
	
	for ( unsigned int i = 0; i < m; i++ )
	{
		Scalar sum = 0;
		
		for ( unsigned int j = 0; j < n; j++ )
			sum += ( src_.a[i][j] * param.x ( j ) );
		
		src.x[i] = sum;
	}
	
	return RnVector< m > ( src );
}

template <unsigned int m, unsigned int n>
LinearMap< m, m > LinearMap< m, n >::power (const unsigned int& param) const
{
	if ( m != n )
		throw ( PowerNotDefined () );
	
	if ( !param )
		return identity< m > ();
	
	LinearMap< m, m > RET ( *this );
	
	for ( unsigned int i = 0; i < param - 1; i++ )
		RET *= RET;
	
	return RET;
}

template <unsigned int m, unsigned int n>
LinearMap< m, m > LinearMap< m, n >::operator^ (const unsigned int& param) const
{
	return power ( param );
}

template <unsigned int m, unsigned int n>
LinearMap< m, n >& LinearMap< m, n >::operator^= (const unsigned int& param)
{
	*this = power ( param );
	
	return *this;
}

template <unsigned int m, unsigned int n>
Scalar LinearMap< m, n >::cofactor (
	const unsigned int& i,
	const unsigned int& j
) const
{
	if ( m != n )
		throw ( CofactorNotDefined () );
	
	return (	submatrix< m, n > ( *this, i, j ).det () *
			( ( ( i + j ) % 2 ) ? -1 : 1 )	);
}

template <unsigned int m, unsigned int n>
const Scalar& LinearMap< m, n >::det () const
{
	if ( m != n )
		throw ( DetNotDefined () );
	
	return det_;
}

template <unsigned int m, unsigned int n>
LinearMap< n, m > LinearMap< m, n >::transpose () const
{
	typename LinearMap< n, m >::MatrixSrc src;
	
	for ( unsigned int i = 0; i < n; i++ )
	{
		for ( unsigned int j = 0; j < m; j++ )
			src.a[i][j] = src_.a[j][i];
	}
	
	return LinearMap< n, m > ( src );
}

template <unsigned int m, unsigned int n>
LinearMap< m, m > LinearMap< m, n >::adjugate () const
{
	if ( m != n )
		throw ( AdjugateNotDefined () );
	
	typename LinearMap< m, m >::MatrixSrc src;
	
	for ( unsigned int i = 0; i < m; i++ )
	{
		for ( unsigned int j = 0; j < m; j++ )
			src.a[i][j] = cofactor ( i, j );
	}
	
	return LinearMap< m, m > ( src ).transpose ();
}

template <unsigned int m, unsigned int n>
LinearMap< m, m > LinearMap< m, n >::inverse () const
{
	if ( !det_ )
		throw ( InverseNotDefined () );
	
	return ( adjugate () / det_ );
}

// =============================================================================
// Implementations of LinearMap Template Functions
// =============================================================================

template <unsigned int m>
LinearMap< m, m > identity ()
{
	typename LinearMap< m, m >::MatrixSrc src;
	
	for ( unsigned int i = 0; i < m; i++ )
	{
		for ( unsigned int j = 0; j < i; j++ )
			src.a[i][j] = 0;
		
		src.a[i][i] = 1;
		
		for ( unsigned int j = i + 1; j < m; j++ )
			src.a[i][j] = 0;
	}
	
	return LinearMap< m, m > ( src );
}

template <unsigned int m, unsigned int n>
LinearMap< SUBSIZE(m), SUBSIZE(n) > submatrix (
	const LinearMap< m, n >& param,
	const unsigned int& i,
	const unsigned int& j
)
{
	if ( ( m < 2 ) || ( n < 2 ) )
		throw ( SubMatrixNotDefined () );
	
	typename LinearMap< SUBSIZE(m), SUBSIZE(n) >::MatrixSrc src;
	
	for ( unsigned int k = 0; k < i; k++ )
	{
		for ( unsigned int l = 0; l < j; l++ )
			src.a[k][l] = param.a ( k, l );
		
		for ( unsigned int l = j + 1; l < n; l++ )
			src.a[k][ l - 1 ] = param.a ( k, l );
	}
	
	for ( unsigned int k = i + 1; k < m; k++ )
	{
		for ( unsigned int l = 0; l < j; l++ )
			src.a[ k - 1 ][l] = param.a ( k, l );
		
		for ( unsigned int l = j + 1; l < n; l++ )
			src.a[ k - 1 ][ l - 1 ] = param.a ( k, l );
	}
	
	return LinearMap< SUBSIZE(m), SUBSIZE(n) > ( src );
}

}

#endif
