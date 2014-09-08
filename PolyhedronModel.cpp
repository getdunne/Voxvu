#include "stdafx.h"

#include "PolyhedronModel.h"

#include <math.h>

#define PI 3.1415926
#define MIN_DELTA_THETA PI/180.	// see SortVertices() below
#define MAXINTVERTS 50			// size of some workspace arrays
								

//--------------------------------------------------------------
// GetDiagonal: return length of maximum diagonal of model.
//--------------------------------------------------------------
double PolyhedronModel::GetDiagonal ()
{
	ASSERT(m_bValid);

	double d1 = (tlf-brb).Length();
	double d2 = (trf-blb).Length();
	return (d1 > d2) ? d1 : d2;
}

//--------------------------------------------------------------
// Helper function: is point v "inside" (in negative half-space
// of) plane p?
//--------------------------------------------------------------
static BOOL Inside (const Vector3 &v, const Plane &p)
{
	Vector3 vv = v;
	return (vv.Dot(p.n) < p.d);
}

//--------------------------------------------------------------
// Helper function: Returns point of intersection between line
// uv and plane p.
//--------------------------------------------------------------
static Vector3 Intersect (const Vector3 &u, const Vector3 &v, const Plane &p)
{
	Vector3 uu = u, vv = v;
	Vector3 w;

	double du = p.d - uu.Dot(p.n);	// distance from plane to u
	double dv = vv.Dot(p.n) - p.d;	// distance from plane to v

	ASSERT(du > 0. && dv > 0.);	// fail if no intersection

	double t = du/(du+dv);
	w = u + t*(v-u);
	return w;
}

//--------------------------------------------------------------
// Helper function: Clips given polygon against given plane,
// adds points of intersection to given list.
//--------------------------------------------------------------
static void IntersectCPolygonWithPlane (const CPolygon& poly,
	const Plane& plane, int *ints, Vector3 intersections[])
{
	int nverts = poly.GetVertexCount();
	if (nverts == 0) return;
	
	Vector3 u = poly.v[nverts-1];
	for (int j=0; j < nverts; j++) {
		Vector3 v = poly.v[j];
		if (Inside(u,plane) && !Inside(v,plane))
			intersections[(*ints)++] = Intersect(u,v,plane);
		else if (Inside(v,plane) && !Inside(u,plane))
			intersections[(*ints)++] = Intersect(v,u,plane);
		u = v;
	}
}

//--------------------------------------------------------------
// Helper function: just like IntersectCPolygonWithPlane() above,
// but actually performs clipping, i.e. modifies given CPolygon.
//--------------------------------------------------------------
static void ClipCPolygonAgainstPlane (CPolygon& poly,
	const Plane& plane, int *ints, Vector3 intersections[])
{
	int nverts = poly.GetVertexCount();
	if (nverts == 0) return;

	Vector3 clipped[MAXINTVERTS];
	int nClip=0;
	
	Vector3 u = poly.v[nverts-1];
	for (int j=0; j < nverts; j++) {
		Vector3 v = poly.v[j];
		if (Inside(v,plane)) {
			if (Inside(u,plane))
				clipped[nClip++] = v;
			else {
				clipped[nClip++] = intersections[(*ints)++] = Intersect(v,u,plane);
				clipped[nClip++] = v;
			}
		}
		else if (Inside(u,plane))
			clipped[nClip++] = intersections[(*ints)++] = Intersect(u,v,plane);
		u = v;
	}

	poly.SetVertexCount(nClip);
	for (int i=0; i < nClip; ++i) poly.v[i] = clipped[i];
}

//--------------------------------------------------------------
// Helper function: Given a list of vertices, possibly with some
// duplicate members, returns a sorted list with no duplicates.
// Sort is counter-clockwise about centroid of vertices, looking
// backward along given normal vector.
//--------------------------------------------------------------
static void SortVertices (
	const int nin, Vector3 vin[],	// vin[] is used as a work area
	const Vector3 &normal,
	int *nout, Vector3 vout[]
	)
{
	double theta[MAXINTVERTS];

	if (nin <= 0) {
		*nout = 0;
		return;
	}
	
	// compute centroid of input vertices
	Vector3 centroid(0.,0.,0.);
	int i;
	for (i=0; i < nin; i++) centroid+=vin[i];
	centroid *= 1./nin;
	
	// compute fixed vectors v0, v1
	Vector3 v0 = vin[0] - centroid;
	Vector3 nn = normal;
	Vector3 v1 = nn.Cross(v0);
	double epsilon = v0.Length()*v1.Length()*.001;
	
	theta[0] = 0.;
	for (i=1; i < nin; i++) {
		// step 1: compute V
		Vector3 v = vin[i]-centroid;
		// step 2: compute basic angle
		double t1 = v0.Dot(v);
		double t2 = t1/(v0.Length()*v.Length());
		if (t2 <= -1. || t2 >= 1.) theta[i] = (t2 < 0.) ? PI : 0.;
		else theta[i] =  acos(t2);
		// step 3: adjust for angles greater than Pi radians (test changed from
		// less-than to greater-than to compute clockwise angles)
		double tmp = v.Dot(v1);
		if (t1 < 0 && fabs(tmp) < epsilon) theta[i] = PI;
		else if (tmp > 0.) {
			if (fabs(theta[i]) < MIN_DELTA_THETA) theta[i] = 0.;
			else theta[i] = 2.0 * PI - theta[i];
		}
	}
		
	// sort input vertices in order of increasing theta
	// (insertion sort is used)
	for (i=2; i < nin; i++) {
		double ttmp = theta[i];
		Vector3 vtmp = vin[i];
		int j;
		for (j=i-1; j >= 1 && theta[j] > ttmp; j--) {
			theta[j+1] = theta[j];
			vin[j+1] = vin[j];
			}
		theta[j+1] = ttmp;
		vin[j+1] = vtmp;
	}
		
	// copy vertices to vout[] array, removing duplicates
	vout[0] = vin[0];
	*nout = 1;
	double ttmp = theta[0];	// remembers theta for last-copied vertex
	for (i=1; i < nin; i++)
		if (theta[i] - ttmp > MIN_DELTA_THETA) {
			vout[(*nout)++] = vin[i];
			ttmp = theta[i];
		}
}

//--------------------------------------------------------------
// Initialize: establish 6-sided rectangular box with given
// dimensions.
//--------------------------------------------------------------
void PolyhedronModel::Initialize (double width, double height, double depth)
{
	TRACE("PolyhedronModel::Initialize\n");

	tlf = Vector3(-width/2.,-height/2.,-depth/2.);
	trf = Vector3(width/2.,-height/2.,-depth/2.);
	blf = Vector3(-width/2.,height/2.,-depth/2.);
	brf = Vector3(width/2.,height/2.,-depth/2.);
	tlb = Vector3(-width/2.,-height/2.,depth/2.);
	trb = Vector3(width/2.,-height/2.,depth/2.);
	blb = Vector3(-width/2.,height/2.,depth/2.);
	brb = Vector3(width/2.,height/2.,depth/2.);

	m_box.SetFaceCount(6);
	m_box.plane[0].n = Vector3(0.,0.,-1.);	// front
	m_box.plane[0].d = depth/2.;
	m_box.poly[0].SetVertexCount(4);
	m_box.poly[0].v[0] = tlf;
	m_box.poly[0].v[1] = trf;
	m_box.poly[0].v[2] = brf;
	m_box.poly[0].v[3] = blf;
	m_box.plane[1].n = Vector3(0.,0.,1.);	// back
	m_box.plane[1].d = depth/2.;
	m_box.poly[1].SetVertexCount(4);
	m_box.poly[1].v[0] = trb;
	m_box.poly[1].v[1] = tlb;
	m_box.poly[1].v[2] = blb;
	m_box.poly[1].v[3] = brb;
	m_box.plane[2].n = Vector3(-1.,0.,0.);	// l.side
	m_box.plane[2].d = width/2.;
	m_box.poly[2].SetVertexCount(4);
	m_box.poly[2].v[0] = tlb;
	m_box.poly[2].v[1] = tlf;
	m_box.poly[2].v[2] = blf;
	m_box.poly[2].v[3] = blb;
	m_box.plane[3].n = Vector3(1.,0.,0.);	// r.side
	m_box.plane[3].d = width/2.;
	m_box.poly[3].SetVertexCount(4);
	m_box.poly[3].v[0] = trf;
	m_box.poly[3].v[1] = trb;
	m_box.poly[3].v[2] = brb;
	m_box.poly[3].v[3] = brf;
	m_box.plane[4].n = Vector3(0.,-1.,0.);	// top
	m_box.plane[4].d = height/2.;
	m_box.poly[4].SetVertexCount(4);
	m_box.poly[4].v[0] = tlb;
	m_box.poly[4].v[1] = trb;
	m_box.poly[4].v[2] = trf;
	m_box.poly[4].v[3] = tlf;
	m_box.plane[5].n = Vector3(0.,1.,0.);	// bottom
	m_box.plane[5].d = height/2.;
	m_box.poly[5].SetVertexCount(4);
	m_box.poly[5].v[0] = blf;
	m_box.poly[5].v[1] = brf;
	m_box.poly[5].v[2] = brb;
	m_box.poly[5].v[3] = blb;

	m_Up = Vector3(0.,-1.,0.);
	m_bValid = TRUE;
	m_bFaceSelected = FALSE;
}

//--------------------------------------------------------------
// Reinitialize: reestablish 6-sided rectangular box with
// dimensions given in a previous call to Initialize().
//--------------------------------------------------------------
void PolyhedronModel::Reinitialize ()
{
	ASSERT(m_bValid);

	m_box.SetFaceCount(6);
	m_box.plane[0].n = Vector3(0.,0.,-1.);	// front
	m_box.plane[0].d = brb.z;
	m_box.poly[0].SetVertexCount(4);
	m_box.poly[0].v[0] = tlf;
	m_box.poly[0].v[1] = trf;
	m_box.poly[0].v[2] = brf;
	m_box.poly[0].v[3] = blf;
	m_box.plane[1].n = Vector3(0.,0.,1.);	// back
	m_box.plane[1].d = brb.z;
	m_box.poly[1].SetVertexCount(4);
	m_box.poly[1].v[0] = trb;
	m_box.poly[1].v[1] = tlb;
	m_box.poly[1].v[2] = blb;
	m_box.poly[1].v[3] = brb;
	m_box.plane[2].n = Vector3(-1.,0.,0.);	// l.side
	m_box.plane[2].d = brb.x;
	m_box.poly[2].SetVertexCount(4);
	m_box.poly[2].v[0] = tlb;
	m_box.poly[2].v[1] = tlf;
	m_box.poly[2].v[2] = blf;
	m_box.poly[2].v[3] = blb;
	m_box.plane[3].n = Vector3(1.,0.,0.);	// r.side
	m_box.plane[3].d = brb.x;
	m_box.poly[3].SetVertexCount(4);
	m_box.poly[3].v[0] = trf;
	m_box.poly[3].v[1] = trb;
	m_box.poly[3].v[2] = brb;
	m_box.poly[3].v[3] = brf;
	m_box.plane[4].n = Vector3(0.,-1.,0.);	// top
	m_box.plane[4].d = brb.y;
	m_box.poly[4].SetVertexCount(4);
	m_box.poly[4].v[0] = tlb;
	m_box.poly[4].v[1] = trb;
	m_box.poly[4].v[2] = trf;
	m_box.poly[4].v[3] = tlf;
	m_box.plane[5].n = Vector3(0.,1.,0.);	// bottom
	m_box.plane[5].d = brb.y;
	m_box.poly[5].SetVertexCount(4);
	m_box.poly[5].v[0] = blf;
	m_box.poly[5].v[1] = brf;
	m_box.poly[5].v[2] = brb;
	m_box.poly[5].v[3] = blb;

	m_bFaceSelected = FALSE;
}

//--------------------------------------------------------------
// SelectFace: used to select one face of the current model as
// the "current" face for subsequent operations.  The face arrays
// are reorganized so the current face is last.  Parameter face
// may also be negative, to deselect the currently-selected face.
// In all cases, faces indexed 6 and higher having zero vertices
// are culled from the list.
//--------------------------------------------------------------
void PolyhedronModel::SelectFace (int face)
{
	ASSERT(m_bValid);

	// select new face (or none)
	if (face < 0) m_bFaceSelected = FALSE;
	else {
		ASSERT(face >=0 && face < m_box.GetFaceCount());
		m_bFaceSelected = TRUE;
		m_box.SwapFaces(m_box.GetFaceCount()-1,face);
	}

	// cull zero-vertex-count faces from polyhedron
	int i=6;
	while (i < m_box.GetFaceCount()) {
		if (m_box.poly[i].GetVertexCount() == 0) {
			// delete face i
			for (int j=i+1; j < m_box.GetFaceCount(); ++j) {
				m_box.poly[i-1] = m_box.poly[i];
				m_box.plane[i-1] = m_box.plane[i];
			}
			m_box.SetFaceCount(m_box.GetFaceCount()-1);
		}
		else ++i;
	}

	TRACE("%d Faces\n",m_box.GetFaceCount());
}

//--------------------------------------------------------------
// ComputePoly: given a plane, return the polygon formed by the
// intersection of that plane with the current model.  (If there
// is no intersection, the result will have 0 vertices.)
//--------------------------------------------------------------
CPolygon PolyhedronModel::ComputePoly (const Plane& plane)
{
	ASSERT(m_bValid);

	Vector3 intersections[MAXINTVERTS];
	int nints=0;
	for (int side=0; side<m_box.GetFaceCount(); ++side)
		IntersectCPolygonWithPlane(m_box.poly[side],plane,
								  &nints,intersections);
	int nints2;
	Vector3 intersections2[MAXINTVERTS];
	SortVertices(nints,intersections,plane.n,&nints2,intersections2);

	CPolygon result;
	result.SetVertexCount(nints2);
	for (int i=0; i < nints2; ++i) result.v[i] = intersections2[i];
	return result;
}

//--------------------------------------------------------------
// recompute all face polygons following a change to selected face
//--------------------------------------------------------------
void PolyhedronModel::RecomputePolygons ()
{
	ASSERT(m_bValid);
	if (!m_bFaceSelected) return;

	// first, re-establish initial 6 bounding faces
	m_box.poly[0].SetVertexCount(4);
	m_box.poly[0].v[0] = tlf;
	m_box.poly[0].v[1] = trf;
	m_box.poly[0].v[2] = brf;
	m_box.poly[0].v[3] = blf;
	m_box.poly[1].SetVertexCount(4);
	m_box.poly[1].v[0] = trb;
	m_box.poly[1].v[1] = tlb;
	m_box.poly[1].v[2] = blb;
	m_box.poly[1].v[3] = brb;
	m_box.poly[2].SetVertexCount(4);
	m_box.poly[2].v[0] = tlb;
	m_box.poly[2].v[1] = tlf;
	m_box.poly[2].v[2] = blf;
	m_box.poly[2].v[3] = blb;
	m_box.poly[3].SetVertexCount(4);
	m_box.poly[3].v[0] = trf;
	m_box.poly[3].v[1] = trb;
	m_box.poly[3].v[2] = brb;
	m_box.poly[3].v[3] = brf;
	m_box.poly[4].SetVertexCount(4);
	m_box.poly[4].v[0] = tlb;
	m_box.poly[4].v[1] = trb;
	m_box.poly[4].v[2] = trf;
	m_box.poly[4].v[3] = tlf;
	m_box.poly[5].SetVertexCount(4);
	m_box.poly[5].v[0] = blf;
	m_box.poly[5].v[1] = brf;
	m_box.poly[5].v[2] = brb;
	m_box.poly[5].v[3] = blb;

	// compute effect of additional faces one at a time
	for (int face=6; face < m_box.GetFaceCount(); ++face) {
		Vector3 ints[MAXINTVERTS],ints2[MAXINTVERTS];
		int nints=0,nints2;

		for (int side=0; side < face; ++side)
			ClipCPolygonAgainstPlane(m_box.poly[side],
							m_box.plane[face],
							&nints,ints);
		SortVertices(nints,ints,m_box.plane[face].n,&nints2,ints2);
		m_box.poly[face].SetVertexCount(nints2);
		for (int i=0; i < nints2; ++i) m_box.poly[face].v[i] = ints2[i];
	}
}

//--------------------------------------------------------------
// add a new face to the model, initially almost the same as the
// given face (d parameter adjusted so it's just inside it).  The
// new face becomes the currently selected face.
//--------------------------------------------------------------
void PolyhedronModel::CloneFace (int face)
{
	TRACE("CloneFace %d\n",face);
	ASSERT(m_bValid);
	ASSERT(face >=0 && face < m_box.GetFaceCount());
	m_bFaceSelected = TRUE;
	int newFace = m_box.GetFaceCount();
	m_box.SetFaceCount(newFace+1);
	m_box.plane[newFace] = m_box.plane[face];
	m_box.plane[newFace].d *= 0.999;
	RecomputePolygons();
}

//--------------------------------------------------------------
// Translate currently-selected face to new position given by
// new plane parameter D.
//--------------------------------------------------------------
void PolyhedronModel::TranslateSelectedFace (const double newD)
{
	ASSERT(m_bValid);
	int selFace = GetSelectedFace();
	if (selFace < 0) return;
	m_box.plane[selFace].d = newD;
	RecomputePolygons();
}

//--------------------------------------------------------------
// Rotate currently-selected face to new orientation according
// to rotation matrix R about given pivot point in model space
//--------------------------------------------------------------
void PolyhedronModel::RotateSelectedFace (const Matrix4x4& R, const Vector3& pivot)
{
	ASSERT(m_bValid);
	int selFace = GetSelectedFace();
	if (selFace < 0) return;
	m_box.plane[selFace].n = (m_box.plane[selFace].n).Lin(R);
	(m_box.plane[selFace].n).Normalize();
	m_box.plane[selFace].d = m_box.plane[selFace].n.Dot(pivot);
	RecomputePolygons();
}

