#include "stdafx.h"
#include <math.h>
#include "PolygonFiller.h"

#define round(x) int((x)+.5)

//--------------------------------------------------------------
// Constructor establishes valid null state.
//--------------------------------------------------------------
PolygonFiller::PolygonFiller ()
{
	m_nMaxVerts = -1;
	m_pVertices = NULL;
	m_pEdges = NULL;
	m_ScrnToTex.MakeIdentity();
	m_clipRect = CRect(0,0,0,0);
	m_pbContinue = NULL;
	m_pLineProc = NULL;
	m_pUserData = NULL;
}

//--------------------------------------------------------------
// Setup() allocates owned working storage according to the
// maximum polygon size we expect.
//--------------------------------------------------------------
void PolygonFiller::Setup (const int nMaxVerts)
{
	// delete old working store if any, so can do this more than once
	if (m_pVertices) delete[] m_pVertices;
	if (m_pEdges) delete[] m_pEdges;

	// actual setup
	m_nMaxVerts = nMaxVerts;
	m_pVertices = new VLE[nMaxVerts];
	ASSERT(m_pVertices);
	m_pEdges = new ELE[nMaxVerts];
	ASSERT(m_pEdges);
}

//--------------------------------------------------------------
// Destructor deallocates owned working storage if any
//--------------------------------------------------------------
PolygonFiller::~PolygonFiller ()
{
	if (m_pVertices) delete[] m_pVertices;
	if (m_pEdges) delete[] m_pEdges;
}

//--------------------------------------------------------------
// Main operation: fills given polygon.  Monitors state of
// Boolean variable pointed to by m_pbContinue, returns FALSE
// immediately if it goes false.  Returns TRUE otherwise.
//--------------------------------------------------------------
BOOL PolygonFiller::Fill (const CPolygon& poly, const Plane& plane)
{
	int nverts = poly.GetVertexCount();
	if (nverts < 3) return TRUE;
	ASSERT(nverts <= m_nMaxVerts);
	ASSERT(m_pLineProc != NULL);
	ASSERT(m_pUserData != NULL);

	// compute deltas for entire polygon
	m_deltaZx = -plane.n.x / plane.n.z;
	m_deltaZy = -plane.n.y / plane.n.z;
	m_deltaTx = Vector3(1.,0.,m_deltaZx).Lin(m_ScrnToTex);
	m_deltaTy = Vector3(0.,1.,m_deltaZy).Lin(m_ScrnToTex);
	
	// compute vertex list at m_pVertices
	int i = nverts-1;
	VLE* vlp = m_pVertices+i;
	for (; i >= 0; --i, --vlp) {
		vlp->scrn.x = vlp->x = round(poly.v[i].x);
		vlp->scrn.y = vlp->y = round(poly.v[i].y);
		vlp->scrn.z = plane.ZfromXY(vlp->scrn.x,vlp->scrn.y);
		vlp->tex = (vlp->scrn).Aff(m_ScrnToTex);
		}
	
	// compute edge list, painting any horizontal edges found
	int side_count = 0;
	int p1 = nverts-1;
	int bottomscan = m_pVertices[p1].y;
	for (int p2=0; p2 < nverts; ++p2) {
		if (m_pVertices[p1].y != m_pVertices[p2].y) {
			// non-horizontal edge: put it in table
			int p = p2;
			while (m_pVertices[p].y == m_pVertices[p2].y)
				if (++p == nverts) p = 0;
			AddEdge(side_count,p1,p2,m_pVertices[p].y);
			++side_count;
			}
		else if (m_pVertices[p1].x < m_pVertices[p2].x)
			// horizontal edge: draw immediately
			PaintLine(m_pVertices[p1].y,
						m_pVertices[p1].x,m_pVertices[p2].x,
						m_pVertices[p1].scrn.z,m_pVertices[p2].scrn.z,
						m_pVertices[p1].tex,m_pVertices[p2].tex);
		if (m_pVertices[p2].y > bottomscan)
			bottomscan = m_pVertices[p2].y;
		p1 = p2;
		}
	if (side_count==0) return TRUE;
	
	// loop over scan lines from top to bottom of polygon
	int first = 0;
	int last = 0;
	for (int scan = m_pEdges[0].ymin; scan <= bottomscan; scan++) {
		// check for external interrupt
		if (m_pbContinue && *m_pbContinue == FALSE)
			return FALSE;
		// get indices of first & last edges in list which intersect
		// current scan line (active edges)
		while (m_pEdges[last+1].ymin <= scan && last < side_count-1) ++last;
		while (m_pEdges[first].delta_y == 0) {
			++first;
			if (first >= side_count) return TRUE;
		}
		// sort active edges left to right, count intersections with
		// current scan line
		int x_int_count = 0;
		int edge;
		for (edge = first; edge <= last; ++edge)
			if (m_pEdges[edge].delta_y > 0) {
				++x_int_count;
				for (int e=edge; e > first &&
					  m_pEdges[e].x_int < m_pEdges[e-1].x_int; --e) {
					ELE tmp = m_pEdges[e-1];
					m_pEdges[e-1] = m_pEdges[e];
					m_pEdges[e] = tmp;
					}
				}
		// paint segments of current scan line
		int index = first;
		for (int k = 1; k <= round(x_int_count/2); k++) {
			while (m_pEdges[index].delta_y == 0) ++index;
			int x1 = FPround(m_pEdges[index].x_int);
			double z = m_pEdges[index].z;
			Vector3 tex = m_pEdges[index].tex;
			++index;
			while (m_pEdges[index].delta_y == 0) ++index;
			int x2 = FPround(m_pEdges[index].x_int);
			PaintLine(scan,x1,x2,z,m_pEdges[index].z,tex,m_pEdges[index].tex);
			++index;
			}
		// advance all active edges down to next scan line
		ELE* elp = m_pEdges+first;
		for (edge = first; edge <= last; ++edge, ++elp)
			if (elp->delta_y > 0) {
				--elp->delta_y;
				double c = -FPround(elp->x_int);
				c += FPround(elp->x_int += elp->x_inc);
				elp->z += c*m_deltaZx + m_deltaZy;
				elp->tex += m_deltaTx * c;
				elp->tex += m_deltaTy;
				}
		}

	// finished
	return TRUE;
}

//--------------------------------------------------------------
// call user-supplied paintline routine with clipping, ensuring
// that endpoints are passed in left-to-right order
//--------------------------------------------------------------
void PolygonFiller::PaintLine(const int y, const int x1, const int x2,
							 const double z1, const double z2,
							 Vector3 tex1, Vector3 tex2)
{
	Vector3 dtx;	// corresponding shift in texture coordinates
	
	// if we're outside vertical extent of bounds, quit now
	if (y < m_clipRect.top || y >= m_clipRect.bottom) return;
	
	// ensure that we always draw left-to-right
	int xmin, xmax;
	double z;
	Vector3 tex;
	if (x1 < x2) { xmin = x1; xmax = x2; z = z1; tex = tex1; }
	else { xmin = x2; xmax = x1; z = z2; tex = tex2; }
	
	// adjust line bounds to clip
	int la = m_clipRect.left - xmin;
	if (la > 0) xmin = m_clipRect.left;
	if (xmax >= m_clipRect.right) xmax = m_clipRect.right;
	if (xmax < xmin) return;	// nothing to do if entire line is outside bounds
	
	// if left endpoint was adjusted, adjust z and texture values accordingly
	if (la > 0) {
		z += la * m_deltaZx;
		tex += m_deltaTx * (double)la;
		}
	
	// call user-supplied line-drawing routine
	(*m_pLineProc)(m_pUserData,y,xmin,xmax,z,m_deltaZx,tex,m_deltaTx);
}

//--------------------------------------------------------------
// Adds edge to edge list at m_pEdges.
//--------------------------------------------------------------
void PolygonFiller::AddEdge (int entry, int p1, int p2, int next_y)
{
	int y1 = m_pVertices[p1].y;
	int y2 = m_pVertices[p2].y;
	Fixed x2 = ItoFP(m_pVertices[p2].scrn.x);
	Fixed x_inc = FtoFP((m_pVertices[p2].scrn.x - m_pVertices[p1].scrn.x) /
						(m_pVertices[p2].scrn.y - m_pVertices[p1].scrn.y));
	double z2 = m_pVertices[p2].scrn.z;
	Vector3 tex2 = m_pVertices[p2].tex;
	
	// make adjustments for "problem" vertices
	if (y2 > y1 && y2 < next_y) {
		// raise the lower vertex of this edge
		--y2;
		x2 -= x_inc;
		}
	else if (y2 < y1 && y2 > next_y) {
		// drop the upper vertex of this edge
		y2++;
		double c = -FPround(x2);
		c += FPround(x2 += x_inc);
		z2 += c*m_deltaZx + m_deltaZy;
		tex2 += m_deltaTx * c;
		tex2 += m_deltaTy;
		}
		
	// insert into edge list
	int miny = (y1 < y2) ? y1 : y2;
	while (entry > 0 && miny < m_pEdges[entry-1].ymin) {
		m_pEdges[entry] = m_pEdges[entry-1];
		--entry;
		}
	ELE* elp = m_pEdges+entry;
	
	// fill in newly-created edge list entry
	elp->ymin = miny;
	elp->delta_y = abs(y2 - y1) + 1;
	elp->x_inc = x_inc;
	if (y1 < y2) {
		elp->x_int = ItoFP(m_pVertices[p1].x);
		elp->z = m_pVertices[p1].scrn.z;
		elp->tex = m_pVertices[p1].tex;
		}
	else {
		elp->x_int = x2;
		elp->z = z2;
		elp->tex = tex2;
		}
}
