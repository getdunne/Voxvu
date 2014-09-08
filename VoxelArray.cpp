#include "stdafx.h"

#include <math.h>			// for sqrt()
#include "VoxelArray.h"


//--------------------------------------------------------------
// Allocate allocates the voxel array with ::GlobalAlloc()
//--------------------------------------------------------------
BOOL VoxelArray::Allocate (
		WORD voxBytes, WORD width, WORD height, WORD depth,
		double voxWidth, double voxHeight, double voxDepth)
{
	size_t size = (DWORD)width * height * depth * voxBytes;
	m_array = (BYTE*)::GlobalAlloc(0,size);
	ASSERT(m_array);
	if (m_array == NULL) return FALSE;

	m_voxBytes = voxBytes;
	m_sX = width; m_sY = height; m_sZ = depth;
	m_vX = voxWidth; m_vY = voxHeight; m_vZ = voxDepth;
	m_rowBytes = (DWORD)voxBytes * width;
	m_sliceBytes = m_rowBytes * height;
	return TRUE;
}

//--------------------------------------------------------------
// Diagonal() returns the diagonal dimension of the voxel block
//--------------------------------------------------------------
double VoxelArray::Diagonal() const
{
	double w = m_sX * m_vX;
	double h = m_sY * m_vY;
	double d = m_sZ * m_vZ;
	return sqrt(w*w + h*h + d*d);
}