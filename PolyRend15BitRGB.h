// class PolyRend15BitRGB renders a polyhedron with solid texturing
// assuming 15-bit RGB data going into a 15-bit RGB buffer.
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "PolyhedronRenderer.h"

class PolyRend15BitRGB : public PolyhedronRenderer
{
// Attributes
public:

// Constructors
	PolyRend15BitRGB ();

// Overrides
public:
	virtual void SetWindowLevel (double fw, double fl);
	static void FastLineProc (const LPVOID pUserData,
		const int y, const int xl, const int xr,
		const double z, const double delta_z,
		const Vector3 &tex, const Vector3 &dtex);
	static void BestLineProc (const LPVOID pUserData,
		const int y, const int xl, const int xr,
		const double z, const double delta_z,
		const Vector3 &tex, const Vector3 &dtex);
	virtual LineProcPtr GetFastLineProcPtr () { return FastLineProc; }
	virtual LineProcPtr GetBestLineProcPtr () { return BestLineProc; }

// Implementation
protected:
};
