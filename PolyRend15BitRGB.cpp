// PolyRend15BitRGB.cpp		implementation of class PolyRend15BitRGB
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "PolyRend15BitRGB.h"
#include "Macros.h"


//--------------------------------------------------------------
// Constructor
//--------------------------------------------------------------
PolyRend15BitRGB::PolyRend15BitRGB ()
{
}

//--------------------------------------------------------------
// Compute new window/level lookup table
//--------------------------------------------------------------
void PolyRend15BitRGB::SetWindowLevel (double fw, double fl)
{
}

// macros for LineProcs
#define OUT_OF_BOUNDS(X,Y,Z) ((X)<0||(Y)<0||(Z)<0||(X)>=dimU||(Y)>=dimV||(Z)>=dimW)
#define LOOKUP(X,Y,Z) ap[((Z)*dimV+Y)*dimU+X]
#define SWAPBYTES(vox) ((LOBYTE(vox)<<8)|HIBYTE(vox))

//--------------------------------------------------------------
// Fast LineProc using nearest-neighbor lookup
//--------------------------------------------------------------
void PolyRend15BitRGB::FastLineProc (const LPVOID pUserData,
		const int y, const int xl, const int xr,
		const double z, const double delta_z,
		const Vector3 &tex, const Vector3 &dtex)
{
	PolyRend15BitRGB* pMe = (PolyRend15BitRGB*)pUserData;
	unsigned short dimU = pMe->m_pVoxels->GetArrayWidth();
	unsigned short dimV = pMe->m_pVoxels->GetArrayHeight();
	unsigned short dimW = pMe->m_pVoxels->GetArrayDepth();
	WORD* ap = (WORD*)pMe->m_pVoxels->pSlice(0);
	WORD* thePixel = (WORD*)pMe->m_pDB->pPixel(xl,y);
	WORD vox;

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
		vox = OUT_OF_BOUNDS(U,V,W) ? 0 : LOOKUP(U,V,W);
		*thePixel++ = SWAPBYTES(vox);
		u += du; v += dv; w += dw;
		}
}

//--------------------------------------------------------------
// Best LineProc using trilinear interpolation
//--------------------------------------------------------------
#define REDBITS(w) (((w)>>10) & 0x1F)
#define GREENBITS(w) (((w)>>5) & 0x1F)
#define BLUEBITS(w) ((w) & 0x1F)

void PolyRend15BitRGB::BestLineProc (const LPVOID pUserData,
		const int y, const int xl, const int xr,
		const double z, const double delta_z,
		const Vector3 &tex, const Vector3 &dtex)
{
	PolyRend15BitRGB* pMe = (PolyRend15BitRGB*)pUserData;
	unsigned short dimU = pMe->m_pVoxels->GetArrayWidth();
	unsigned short dimV = pMe->m_pVoxels->GetArrayHeight();
	unsigned short dimW = pMe->m_pVoxels->GetArrayDepth();
	WORD* ap = (WORD*)pMe->m_pVoxels->pSlice(0);
	WORD* thePixel = (WORD*)pMe->m_pDB->pPixel(xl,y);

	Fixed u = FtoFP(tex.x)+FPhalf;
	Fixed v = FtoFP(tex.y)+FPhalf;
	Fixed w = FtoFP(tex.z)+FPhalf;
	Fixed du = FtoFP(dtex.x);
	Fixed dv = FtoFP(dtex.y);
	Fixed dw = FtoFP(dtex.z);
	unsigned long U,V,W,Uf,Vf,Wf,Uc,Vc,Wc;
	unsigned long a,b,c,d,e,f,g,h,ab,cd,ef,gh,abcd,efgh;
	int red,green,blue;
	for (int x=xl; x < xr; ++x) {
		U = FPint(u); Uf = FPfrac(u); Uc = FPone - Uf;
		V = FPint(v); Vf = FPfrac(v); Vc = FPone - Vf;
		W = FPint(w); Wf = FPfrac(w); Wc = FPone - Wf;
		a = OUT_OF_BOUNDS(U,V,W) ? 0 : LOOKUP(U,V,W);
		a = SWAPBYTES(a);
		b = OUT_OF_BOUNDS(U+1,V,W) ? 0 : LOOKUP(U+1,V,W);
		b = SWAPBYTES(b);
		c = OUT_OF_BOUNDS(U,V+1,W) ? 0 : LOOKUP(U,V+1,W);
		c = SWAPBYTES(c);
		d = OUT_OF_BOUNDS(U+1,V+1,W) ? 0 : LOOKUP(U+1,V+1,W);
		d = SWAPBYTES(d);
		e = OUT_OF_BOUNDS(U,V,W+1) ? 0 : LOOKUP(U,V,W+1);
		e = SWAPBYTES(e);
		f = OUT_OF_BOUNDS(U+1,V,W+1) ? 0 : LOOKUP(U+1,V,W+1);
		f = SWAPBYTES(f);
		g = OUT_OF_BOUNDS(U,V+1,W+1) ? 0 : LOOKUP(U,V+1,W+1);
		g = SWAPBYTES(g);
		h = OUT_OF_BOUNDS(U+1,V+1,W+1) ? 0 : LOOKUP(U+1,V+1,W+1);
		h = SWAPBYTES(h);
		ab = (REDBITS(a)*Uc + REDBITS(b)*Uf + 0x800) >> 12;
		cd = (REDBITS(c)*Uc + REDBITS(d)*Uf + 0x800) >> 12;
		ef = (REDBITS(e)*Uc + REDBITS(f)*Uf + 0x800) >> 12;
		gh = (REDBITS(g)*Uc + REDBITS(h)*Uf + 0x800) >> 12;
		abcd = (ab*Vc + cd*Vf + 0x8000) >> 16;
		efgh = (ef*Vc + gh*Vf + 0x8000) >> 16;
		red = (abcd*Wc + efgh*Wf + 0x80000) >> 20;
		ab = (GREENBITS(a)*Uc + GREENBITS(b)*Uf + 0x800) >> 12;
		cd = (GREENBITS(c)*Uc + GREENBITS(d)*Uf + 0x800) >> 12;
		ef = (GREENBITS(e)*Uc + GREENBITS(f)*Uf + 0x800) >> 12;
		gh = (GREENBITS(g)*Uc + GREENBITS(h)*Uf + 0x800) >> 12;
		abcd = (ab*Vc + cd*Vf + 0x8000) >> 16;
		efgh = (ef*Vc + gh*Vf + 0x8000) >> 16;
		green = (abcd*Wc + efgh*Wf + 0x80000) >> 20;
		ab = (BLUEBITS(a)*Uc + BLUEBITS(b)*Uf + 0x800) >> 12;
		cd = (BLUEBITS(c)*Uc + BLUEBITS(d)*Uf + 0x800) >> 12;
		ef = (BLUEBITS(e)*Uc + BLUEBITS(f)*Uf + 0x800) >> 12;
		gh = (BLUEBITS(g)*Uc + BLUEBITS(h)*Uf + 0x800) >> 12;
		abcd = (ab*Vc + cd*Vf + 0x8000) >> 16;
		efgh = (ef*Vc + gh*Vf + 0x8000) >> 16;
		blue = (abcd*Wc + efgh*Wf + 0x80000) >> 20;
		*thePixel++ = (red<<10) | (green<<5) | blue;
		u += du; v += dv; w += dw;
		}
}
