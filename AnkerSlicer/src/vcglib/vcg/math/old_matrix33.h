/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2016                                           \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#ifndef VCG_USE_EIGEN
#include "deprecated_matrix33.h"
#else

#ifndef __VCGLIB_MATRIX33_H
#define __VCGLIB_MATRIX33_H

#include "eigen.h"
#include "matrix44.h"

namespace vcg{
template<class Scalar> class Matrix33;
}

namespace Eigen{
template<typename Scalar>
struct ei_traits<vcg::Matrix33<Scalar> > : ei_traits<Eigen::Matrix<Scalar,3,3,RowMajor> > {};
template<typename XprType> struct ei_to_vcgtype<XprType,3,3,RowMajor,3,3>
{ typedef vcg::Matrix33<typename XprType::Scalar> type; };
}

namespace vcg {

/** \deprecated use Matrix<Scalar,3,3>
	@name Matrix33
	Class Matrix33.
    This is the class for definition of a matrix 3x3.
	@param S (Templete Parameter) Specifies the ScalarType field.
*/
template<class _Scalar>
class Matrix33 : public Eigen::Matrix<_Scalar,3,3,Eigen::RowMajor> // FIXME col or row major ?
{

	typedef Eigen::Matrix<_Scalar,3,3,Eigen::RowMajor> _Base;
public:

	using _Base::coeff;
	using _Base::coeffRef;
	using _Base::setZero;

	_EIGEN_GENERIC_PUBLIC_INTERFACE(Matrix33,_Base);
	typedef _Scalar ScalarType;

	VCG_EIGEN_INHERIT_ASSIGNMENT_OPERATORS(Matrix33)

	/// Default constructor
	inline Matrix33() : Base() {}

	/// Copy constructor
	Matrix33(const Matrix33& m ) : Base(m) {}

	/// create from a \b row-major array
	Matrix33(const Scalar * v ) : Base(Eigen::Map<Eigen::Matrix<Scalar,3,3,Eigen::RowMajor> >(v)) {}

	/// create from Matrix44 excluding row and column k
	Matrix33(const Matrix44<Scalar> & m, const int & k) : Base(m.minor(k,k)) {}

	template<typename OtherDerived>
	Matrix33(const Eigen::MatrixBase<OtherDerived>& other) : Base(other) {}

	/*! \deprecated use *this.row(i) */
	inline typename Base::RowXpr operator[](const unsigned int i)
	{ return Base::row(i); }

	/*! \deprecated use *this.row(i) */
	inline const typename Base::RowXpr operator[](const unsigned int i) const
	{ return Base::row(i); }

	/** \deprecated */
	Matrix33 & SetRotateRad(Scalar angle, const Point3<Scalar> & axis )
	{
		*this = Eigen::AngleAxis<Scalar>(angle,axis).toRotationMatrix();
		return (*this);
	}
	/** \deprecated */
	Matrix33 & SetRotateDeg(Scalar angle, const Point3<Scalar> & axis ){
		return SetRotateRad(math::ToRad(angle),axis);
	}

  // Warning, this Inversion code can be HIGHLY NUMERICALLY UNSTABLE!
  // In most case you are advised to use the Invert() method based on SVD decomposition.
  /** \deprecated */
	Matrix33 & FastInvert() { return  *this = Base::inverse(); }

	void show(FILE * fp)
	{
		for(int i=0;i<3;++i)
		    printf("| %g \t%g \t%g |\n",coeff(i,0),coeff(i,1),coeff(i,2));
	}

	/** \deprecated use a * b.transpose()
	compute the matrix generated by the product of a * b^T
	*/
	// hm.... this is the outer product
	void ExternalProduct(const Point3<Scalar> &a, const Point3<Scalar> &b) { *this = a * b.transpose(); }

	/** Compute the Frobenius Norm of the Matrix */
	Scalar Norm() { return Base::cwise().abs2().sum(); }

	/** Computes the  covariance matrix of a set of 3d points. Returns the barycenter.
	*/
	// FIXME should be outside Matrix


	/**
	It computes the cross covariance matrix of two set of 3d points P and X;
	it returns also the barycenters of P and X.
	fonte:

	Besl, McKay
	A method for registration o f 3d Shapes
	IEEE TPAMI Vol 14, No 2 1992

	*/
	// FIXME should be outside Matrix
	template <class STLPOINTCONTAINER >
	void CrossCovariance(const STLPOINTCONTAINER &P, const STLPOINTCONTAINER &X,
											Point3<Scalar> &bp, Point3<Scalar> &bx)
	{
		setZero();
		assert(P.size()==X.size());
		bx.setZero();
		bp.setZero();
		Matrix33<Scalar> tmp;
		typename std::vector <Point3<Scalar> >::const_iterator pi,xi;
		for(pi=P.begin(),xi=X.begin();pi!=P.end();++pi,++xi){
			bp+=*pi;
			bx+=*xi;
			tmp.ExternalProduct(*pi,*xi);
			(*this)+=tmp;
		}
		bp/=P.size();
		bx/=X.size();
		(*this)/=P.size();
		tmp.ExternalProduct(bp,bx);
		(*this)-=tmp;
	}

	template <class STLPOINTCONTAINER, class STLREALCONTAINER>
	void WeightedCrossCovariance(const STLREALCONTAINER &  weights,
								const STLPOINTCONTAINER &P,
								const STLPOINTCONTAINER &X,
								Point3<Scalar> &bp,
								Point3<Scalar> &bx)
	{
		setZero();
		assert(P.size()==X.size());
		bx.SetZero();
		bp.SetZero();
		Matrix33<Scalar> tmp;
		typename std::vector <Point3<Scalar> >::const_iterator pi,xi;
		typename STLREALCONTAINER::const_iterator pw;

		for(pi=P.begin(),xi=X.begin();pi!=P.end();++pi,++xi){
			bp+=(*pi);
			bx+=(*xi);
		}
		bp/=P.size();
		bx/=X.size();

		for(pi=P.begin(),xi=X.begin(),pw = weights.begin();pi!=P.end();++pi,++xi,++pw){

			tmp.ExternalProduct(((*pi)-(bp)),((*xi)-(bp)));

			(*this)+=tmp*(*pw);
		}
	}
};

template <class S>
void Invert(Matrix33<S> &m) { m = m.lu().inverse(); }

template <class S>
Matrix33<S> Inverse(const Matrix33<S>&m) { return m.lu().inverse(); }

///given 2 vector centered into origin calculate the rotation matrix from first to the second
template <class S>
Matrix33<S> RotationMatrix(vcg::Point3<S> v0,vcg::Point3<S> v1,bool normalized=true)
	{
		typedef typename vcg::Point3<S> CoordType;
		Matrix33<S> rotM;
		const S epsilon=0.00001;
		if (!normalized)
		{
			v0.Normalize();
			v1.Normalize();
		}
		S dot=v0.dot(v1);
		///control if there is no rotation
		if (dot>((S)1-epsilon))
		{
			rotM.SetIdentity();
			return rotM;
		}

		///find the axis of rotation
		CoordType axis;
		axis=v0^v1;
		axis.Normalize();

		///construct rotation matrix
		S u=axis.X();
		S v=axis.Y();
		S w=axis.Z();
		S phi=acos(dot);
		S rcos = cos(phi);
		S rsin = sin(phi);

		rotM[0][0] =      rcos + u*u*(1-rcos);
		rotM[1][0] =  w * rsin + v*u*(1-rcos);
		rotM[2][0] = -v * rsin + w*u*(1-rcos);
		rotM[0][1] = -w * rsin + u*v*(1-rcos);
		rotM[1][1] =      rcos + v*v*(1-rcos);
		rotM[2][1] =  u * rsin + w*v*(1-rcos);
		rotM[0][2] =  v * rsin + u*w*(1-rcos);
		rotM[1][2] = -u * rsin + v*w*(1-rcos);
		rotM[2][2] =      rcos + w*w*(1-rcos);

		return rotM;
	}

///return the rotation matrix along axis
template <class S>
Matrix33<S> RotationMatrix(const vcg::Point3<S> &axis,
						   const float &angleRad)
	{
		vcg::Matrix44<S> matr44;
		vcg::Matrix33<S> matr33;
		matr44.SetRotate(angleRad,axis);
		for (int i=0;i<3;i++)
			for (int j=0;j<3;j++)
				matr33[i][j]=matr44[i][j];
		return matr33;
	}

/// return a random rotation matrix, from the paper:
/// Fast Random Rotation Matrices, James Arvo
/// Graphics Gems III pp. 117-120
template <class S>
 Matrix33<S> RandomRotation(){
	S x1,x2,x3;
	Matrix33<S> R,H,M,vv;
	Point3<S> v;
	R.SetIdentity();
	H.SetIdentity();
	x1 = rand()/S(RAND_MAX);
	x2 = rand()/S(RAND_MAX);
	x3 = rand()/S(RAND_MAX);

	R[0][0] =		cos(S(2)*M_PI*x1);
	R[0][1] =		sin(S(2)*M_PI*x1);
	R[1][0] = -	R[0][1];
	R[1][1] =		R[0][0];

	v[0] = cos(2.0 * M_PI * x2)*sqrt(x3);
	v[1] = sin(2.0 * M_PI * x2)*sqrt(x3);
	v[2] = sqrt(1-x3);

	vv.OuterProduct(v,v);
	H -= vv*S(2);
	M = H*R*S(-1);
	return M;
}

///
typedef Matrix33<short>  Matrix33s;
typedef Matrix33<int>    Matrix33i;
typedef Matrix33<float>  Matrix33f;
typedef Matrix33<double> Matrix33d;

} // end of namespace

#endif

#endif
