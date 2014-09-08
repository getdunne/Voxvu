// class PolyRend16BitGray renders a polyhedron with solid texturing
// assuming 16-bit grayscale data going into an 8-bit grayscale buffer.
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "PolyhedronRenderer.h"

class PolyRend16BitGray : public PolyhedronRenderer
{
// Attributes
public:
	WORD m_window;
	WORD m_level;

// Constructors
	PolyRend16BitGray ();

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
	BYTE wlt[65536];	// window/level lookup table
};
