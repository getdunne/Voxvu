// class PolyRend8BitGray renders a polyhedron with solid texturing
// assuming 8-bit grayscale data going into an 8-bit grayscale buffer.
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "PolyhedronRenderer.h"

class PolyRend8BitGray : public PolyhedronRenderer
{
// Attributes
public:
	BYTE m_window;
	BYTE m_level;

// Constructors
	PolyRend8BitGray ();

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
	BYTE wlt[256];	// window/level lookup table
};
