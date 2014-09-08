#include "stdafx.h"

#include <math.h>
#include "Quaternion.h"
#include "Macros.h"

//--------------------------------------------------------------
// Third form of constructor: takes 3 Euler angles in degrees
//--------------------------------------------------------------
Quaternion::Quaternion (const double ex, const double ey, const double ez)
{
	double psi = DEGTORAD(ex)*0.5;
	double cos_psi = cos(psi), sin_psi = sin(psi);
	double theta = DEGTORAD(ey)*0.5;
	double cos_theta = cos(theta), sin_theta = sin(theta);
	double phi = DEGTORAD(ez)*0.5;
	double cos_phi = cos(phi), sin_phi = sin(phi);

	s = cos_psi*cos_theta*cos_phi + sin_psi*sin_theta*sin_phi;
	V.x = sin_psi*cos_theta*cos_phi - cos_psi*sin_theta*sin_phi;
	V.y = cos_psi*sin_theta*cos_phi + sin_psi*cos_theta*sin_phi;
	V.z = cos_psi*cos_theta*sin_phi - sin_psi*sin_theta*cos_phi;
}

//--------------------------------------------------------------
// return a 4x4 rotation matrix according to this quaternion
//--------------------------------------------------------------
Matrix4x4 Quaternion::RotMatrix() const
{
	double sx = 2. * s * V.x;
	double sy = 2. * s * V.y;
	double sz = 2. * s * V.z;
	double xx = 2. * V.x * V.x;
	double xy = 2. * V.x * V.y;
	double xz = 2. * V.x * V.z;
	double yx = 2. * V.y * V.x;
	double yy = 2. * V.y * V.y;
	double yz = 2. * V.y * V.z;
	double zx = 2. * V.z * V.x;
	double zy = 2. * V.z * V.y;
	double zz = 2. * V.z * V.z;

	Matrix4x4 M;
	M.a = 1. - yy - zz;
	M.b = xy + sz;
	M.c = xz - sy;
	M.d = 0.;
	M.e = xy - sz;
	M.f = 1. - xx - zz;
	M.g = yz + sx;
	M.h = 0.;
	M.i = xz + sy;
	M.j = yz - sx;
	M.k = 1. - xx - yy;
	M.l = 0.;
	M.m = 0.;
	M.n = 0.;
	M.o = 0.;
	M.p = 1.;
	return M;
}

//--------------------------------------------------------------
// multiply this quaternion by a second quaternion q
//--------------------------------------------------------------
Quaternion Quaternion::operator * (const Quaternion& q) const
{
	return Quaternion(q.s*V + s*q.V + V.Cross(q.V), s*q.s - V.Dot(q.V));
}
