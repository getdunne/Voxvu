// PolyRend16BitGray.cpp		implementation of class PolyRend16BitGray
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "PolyRend16BitGray.h"
#include "Macros.h"


//--------------------------------------------------------------
// Constructor
//--------------------------------------------------------------
PolyRend16BitGray::PolyRend16BitGray ()
{
	SetWindowLevel(1.0,0.5);
}

//--------------------------------------------------------------
// Compute new window/level lookup table
//--------------------------------------------------------------
void PolyRend16BitGray::SetWindowLevel (double fw, double fl)
{
	int window = ROUND(fw*65535);
	int level = ROUND(fl*65535);
	ASSERT(window>=0 && window<=65535 && level>=0 && level<=65535);

	m_window = window;
	m_level = level;

	int a = level - ((int)window+1)/2;
	int b = a + window;
	for (int i=0; i < a; ++i) wlt[i] = 0;
	for (int i=b+1; i < 65536; ++i) wlt[i] = 255;
	double m = (window > 0) ? 255./window : 0.;
	double v = 0.5;
	for (int i=a; i <= b; ++i) {
		if (i>=0 && i<=65535) wlt[i] = (BYTE)v;
		v += m;
	}
}

// macros for LineProcs
#define OUT_OF_BOUNDS(X,Y,Z) ((X)<0||(Y)<0||(Z)<0||(X)>=dimU||(Y)>=dimV||(Z)>=dimW)
#define LOOKUP(X,Y,Z) ap[((Z)*dimV+Y)*dimU+X]
#define SWAPBYTES(vox) ((LOBYTE(vox)<<8)|HIBYTE(vox))

//--------------------------------------------------------------
// Fast LineProc using nearest-neighbor lookup
//--------------------------------------------------------------
void PolyRend16BitGray::FastLineProc (const LPVOID pUserData,
		const int y, const int xl, const int xr,
		const double z, const double delta_z,
		const Vector3 &tex, const Vector3 &dtex)
{
	PolyRend16BitGray* pMe = (PolyRend16BitGray*)pUserData;
	unsigned short dimU = pMe->m_pVoxels->GetArrayWidth();
	unsigned short dimV = pMe->m_pVoxels->GetArrayHeight();
	unsigned short dimW = pMe->m_pVoxels->GetArrayDepth();
	WORD* ap = (WORD*)pMe->m_pVoxels->pSlice(0);
	BYTE* thePixel = (BYTE*)pMe->m_pDB->pPixel(xl,y);
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
		vox = SWAPBYTES(vox) + 1024;
		*thePixel++ = pMe->wlt[vox];
		u += du; v += dv; w += dw;
		}
}

//--------------------------------------------------------------
// Best LineProc using trilinear interpolation
//--------------------------------------------------------------
void PolyRend16BitGray::BestLineProc (const LPVOID pUserData,
		const int y, const int xl, const int xr,
		const double z, const double delta_z,
		const Vector3 &tex, const Vector3 &dtex)
{
	PolyRend16BitGray* pMe = (PolyRend16BitGray*)pUserData;
	unsigned short dimU = pMe->m_pVoxels->GetArrayWidth();
	unsigned short dimV = pMe->m_pVoxels->GetArrayHeight();
	unsigned short dimW = pMe->m_pVoxels->GetArrayDepth();
	WORD* ap = (WORD*)pMe->m_pVoxels->pSlice(0);
	BYTE* thePixel = (BYTE*)pMe->m_pDB->pPixel(xl,y);
	WORD vox;

	Fixed u = FtoFP(tex.x)+FPhalf;
	Fixed v = FtoFP(tex.y)+FPhalf;
	Fixed w = FtoFP(tex.z)+FPhalf;
	Fixed du = FtoFP(dtex.x);
	Fixed dv = FtoFP(dtex.y);
	Fixed dw = FtoFP(dtex.z);
	unsigned long U,V,W,Uf,Vf,Wf,Uc,Vc,Wc;
	unsigned long a,b,c,d,e,f,g,h,ab,cd,ef,gh,abcd,efgh;
	for (int x=xl; x < xr; ++x) {
		U = FPint(u); Uf = FPfrac(u); Uc = FPone - Uf; Uf>>=8; Uc>>=8;
		V = FPint(v); Vf = FPfrac(v); Vc = FPone - Vf; Vf>>=8; Vc>>=8;
		W = FPint(w); Wf = FPfrac(w); Wc = FPone - Wf; Wf>>=8; Wc>>=8;
		vox = OUT_OF_BOUNDS(U,V,W) ? 0 : LOOKUP(U,V,W);
		a = (WORD)(SWAPBYTES(vox) + 1024);
		vox = OUT_OF_BOUNDS(U+1,V,W) ? 0 : LOOKUP(U+1,V,W);
		b = (WORD)(SWAPBYTES(vox) + 1024);
		vox = OUT_OF_BOUNDS(U,V+1,W) ? 0 : LOOKUP(U,V+1,W);
		c = (WORD)(SWAPBYTES(vox) + 1024);
		vox = OUT_OF_BOUNDS(U+1,V+1,W) ? 0 : LOOKUP(U+1,V+1,W);
		d = (WORD)(SWAPBYTES(vox) + 1024);
		vox = OUT_OF_BOUNDS(U,V,W+1) ? 0 : LOOKUP(U,V,W+1);
		e = (WORD)(SWAPBYTES(vox) + 1024);
		vox = OUT_OF_BOUNDS(U+1,V,W+1) ? 0 : LOOKUP(U+1,V,W+1);
		f = (WORD)(SWAPBYTES(vox) + 1024);
		vox = OUT_OF_BOUNDS(U,V+1,W+1) ? 0 : LOOKUP(U,V+1,W+1);
		g = (WORD)(SWAPBYTES(vox) + 1024);
		vox = OUT_OF_BOUNDS(U+1,V+1,W+1) ? 0 : LOOKUP(U+1,V+1,W+1);
		h = (WORD)(SWAPBYTES(vox) + 1024);
		ab = (a*Uc + b*Uf) >> 4;
		cd = (c*Uc + d*Uf) >> 4;
		ef = (e*Uc + f*Uf) >> 4;
		gh = (g*Uc + h*Uf) >> 4;
		abcd = (ab*Vc + cd*Vf) >> 8;
		efgh = (ef*Vc + gh*Vf) >> 8;
		*thePixel++ = pMe->wlt[(WORD)(((abcd*Wc + efgh*Wf) >> 12)&0xFFFF)];
		u += du; v += dv; w += dw;
		}
}
