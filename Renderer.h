// class Renderer encapsulates a specific rendering algorithm, which
// renders a given geometry (specific to the sub-class) to a given
// pixel buffer, using a given voxel array as texture data.  All the
// details are in sub-classes.  Subclasses will generally require
// additional attributes and access functions, and some will require
// additional pointers to working storage, e.g. a Z-buffer.
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DrawBuffer.h"
#include "VoxelArray.h"

class Renderer
{
// Attributes
public:
	DrawBuffer* m_pDB;
	VoxelArray* m_pVoxels;

	void SetDrawBuffer (DrawBuffer* pDB) { m_pDB = pDB; }
	void SetVoxelArray (VoxelArray* pV) { m_pVoxels = pV; }

// Overridables
public:
	// interactive rendering algorithm (fastest)
	virtual void RenderQuick () = 0;
	// background algorithm (best quality): monitors *pbContinue and
	// returns FALSE immediately if it ever becomes FALSE; returns
	// TRUE if runs to completion.
	virtual BOOL RenderBest (BOOL* pbContinue) = 0;
};
