// PolyRend8BitGray.cpp		implementation of class PolyRend8BitGray
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "PolyRend8BitGray.h"
#include "Macros.h"


//--------------------------------------------------------------
// Constructor
//--------------------------------------------------------------
PolyRend8BitGray::PolyRend8BitGray ()
{
	SetWindowLevel(1.0,0.5);
}

//--------------------------------------------------------------
// Compute new window/level lookup table
//--------------------------------------------------------------
void PolyRend8BitGray::SetWindowLevel (double fw, double fl)
{
	int window = ROUND(fw*255);
	int level = ROUND(fl*255);
	ASSERT(window>=0 && window<=255 && level>=0 && level<=255);

	m_window = window;
	m_level = level;

	int a = level - ((int)window+1)/2;
	int b = a + window;
	for (int i=0; i < a; ++i) wlt[i] = 0;
	for (int i=b+1; i < 256; ++i) wlt[i] = 255;
	double m = (window > 0) ? 255./window : 0.;
	double v = 0.5;
	for (int i=a; i <= b; ++i) {
		if (i>=0 && i<=255) wlt[i] = (BYTE)v;
		v += m;
	}
}

// macros for LineProcs
#define OUT_OF_BOUNDS(X,Y,Z) ((X)<0||(Y)<0||(Z)<0||(X)>=dimU||(Y)>=dimV||(Z)>=dimW)
#define LOOKUP(X,Y,Z) ap[((Z)*dimV+Y)*dimU+X]

//--------------------------------------------------------------
// Fast LineProc using nearest-neighbor lookup
//--------------------------------------------------------------
void PolyRend8BitGray::FastLineProc (const LPVOID pUserData,
		const int y, const int xl, const int xr,
		const double z, const double delta_z,
		const Vector3 &tex, const Vector3 &dtex)
{
	PolyRend8BitGray* pMe = (PolyRend8BitGray*)pUserData;
	unsigned short dimU = pMe->m_pVoxels->GetArrayWidth();
	unsigned short dimV = pMe->m_pVoxels->GetArrayHeight();
	unsigned short dimW = pMe->m_pVoxels->GetArrayDepth();
	BYTE* ap = (BYTE*)pMe->m_pVoxels->pSlice(0);
	BYTE* thePixel = (BYTE*)pMe->m_pDB->pPixel(xl,y);

	Fixed u = FtoFP(tex.x)+FPhalf;
	Fixed v = FtoFP(tex.y)+FPhalf;
	Fixed w = FtoFP(tex.z)+FPhalf;
	Fixed du = FtoFP(dtex.x);
	Fixed dv = FtoFP(dtex.y);
	Fixed dw = FtoFP(dtex.z);
	for (int x=xl; x < xr; ++x) {
		long U = FPint(u);
		long V = FPint(v);
		long W = FPint(w);
		*thePixel++ = pMe->wlt[OUT_OF_BOUNDS(U,V,W) ? 0 : LOOKUP(U,V,W)];
		u += du; v += dv; w += dw;
		}
}

//--------------------------------------------------------------
// Best LineProc using trilinear interpolation
//--------------------------------------------------------------
void PolyRend8BitGray::BestLineProc (const LPVOID pUserData,
		const int y, const int xl, const int xr,
		const double z, const double delta_z,
		const Vector3 &tex, const Vector3 &dtex)
{
	PolyRend8BitGray* pMe = (PolyRend8BitGray*)pUserData;
	unsigned short dimU = pMe->m_pVoxels->GetArrayWidth();
	unsigned short dimV = pMe->m_pVoxels->GetArrayHeight();
	unsigned short dimW = pMe->m_pVoxels->GetArrayDepth();
	BYTE* ap = (BYTE*)pMe->m_pVoxels->pSlice(0);
	BYTE* thePixel = (BYTE*)pMe->m_pDB->pPixel(xl,y);

	Fixed u = FtoFP(tex.x)+FPhalf;
	Fixed v = FtoFP(tex.y)+FPhalf;
	Fixed w = FtoFP(tex.z)+FPhalf;
	Fixed du = FtoFP(dtex.x);
	Fixed dv = FtoFP(dtex.y);
	Fixed dw = FtoFP(dtex.z);
	long U,V,W,Uf,Vf,Wf,Uc,Vc,Wc;
	long a,b,c,d,e,f,g,h,ab,cd,ef,gh,abcd,efgh;
	for (int x=xl; x < xr; ++x) {
		U = FPint(u); Uf = FPfrac(u); Uc = FPone - Uf;
		V = FPint(v); Vf = FPfrac(v); Vc = FPone - Vf;
		W = FPint(w); Wf = FPfrac(w); Wc = FPone - Wf;
		a = OUT_OF_BOUNDS(U,V,W) ? 0 : LOOKUP(U,V,W);
		b = OUT_OF_BOUNDS(U+1,V,W) ? 0 : LOOKUP(U+1,V,W);
		c = OUT_OF_BOUNDS(U,V+1,W) ? 0 : LOOKUP(U,V+1,W);
		d = OUT_OF_BOUNDS(U+1,V+1,W) ? 0 : LOOKUP(U+1,V+1,W);
		e = OUT_OF_BOUNDS(U,V,W+1) ? 0 : LOOKUP(U,V,W+1);
		f = OUT_OF_BOUNDS(U+1,V,W+1) ? 0 : LOOKUP(U+1,V,W+1);
		g = OUT_OF_BOUNDS(U,V+1,W+1) ? 0 : LOOKUP(U,V+1,W+1);
		h = OUT_OF_BOUNDS(U+1,V+1,W+1) ? 0 : LOOKUP(U+1,V+1,W+1);
		ab = (a*Uc + b*Uf) >> 12;
		cd = (c*Uc + d*Uf) >> 12;
		ef = (e*Uc + f*Uf) >> 12;
		gh = (g*Uc + h*Uf) >> 12;
		abcd = (ab*Vc + cd*Vf) >> 16;
		efgh = (ef*Vc + gh*Vf) >> 16;
		*thePixel++ = pMe->wlt[(BYTE)((abcd*Wc + efgh*Wf) >> 20)];
		u += du; v += dv; w += dw;
		}
}
