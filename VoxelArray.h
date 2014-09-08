// Class VoxelArray encapsulates memory management and addressing
// for 3D arrays, as well as the notion of voxels with real-valued
// dimensions.
//
// This approach is NOT type-safe.  Client code is responsible for
// casting BYTE* return values to the appropriate pointer type.
//
//////////////////////////////////////////////////////////////////
#pragma once

class VoxelArray
{
// Attributes
public:
	WORD m_voxBytes;			// bytes per voxel
	WORD m_sX, m_sY, m_sZ;		// array dimensions
	double m_vX, m_vY, m_vZ;	// voxel dimensions
	DWORD m_rowBytes;			// BYTE offset between rows
	DWORD m_sliceBytes;			// BYTE offset between slices
	BYTE* m_array;				// pointer to voxel array

	// accessors
	WORD GetBytesPerVoxel() const { return m_voxBytes; }
	WORD GetArrayWidth() const { return m_sX; }
	WORD GetArrayHeight() const { return m_sY; }
	WORD GetArrayDepth() const { return m_sZ; }
	DWORD GetArrayLength() const { return m_voxBytes*m_sX*m_sY*m_sZ; }
	double GetVoxelWidth() const { return m_vX; }
	double GetVoxelHeight() const { return m_vY; }
	double GetVoxelDepth() const { return m_vZ; }
	double GetWidth() const { return m_vX * (m_sX-1); }
	double GetHeight() const { return m_vY * (m_sY-1); }
	double GetDepth() const { return m_vZ * (m_sZ-1); }

	// get pointer to first pixel in a slice
	LPVOID pSlice (int slice) const { return
		m_array + m_sliceBytes*slice; }

	// get pointer to a specific voxel
	LPVOID pVoxel (short x, short y, short z) const { return
		m_array + m_sliceBytes*z + m_rowBytes*y + (DWORD)m_voxBytes*x; }

	// test voxel coordinates, return true if outside bounds
	BOOL bOutOfBounds (short x, short y, short z) const { return
		(x<0 || y<0 || z<0 || x>=m_sX || y>=m_sY || z>=m_sZ); }

	// compute maximum diagonal
	double Diagonal() const;

// Constructors
public:
	// default constructor establishes 0x0x0 array
	VoxelArray () { m_array = NULL; m_voxBytes = 0; }

	// destructor: free storage pointed to by m_array
	~VoxelArray () { if (m_array) ::GlobalFree((HGLOBAL)m_array); }

// Operations
public:
	// initialization/allocation: returns TRUE if allocation works
	BOOL Allocate (WORD voxBytes, WORD width, WORD height, WORD depth,
		double voxWidth, double voxHeight, double voxDepth);
};
