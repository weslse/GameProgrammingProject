// PhysicalObj.h: interface for the PhysicalObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHYSICALOBJ_H__49719AF0_6560_45FB_A928_78DD371377F2__INCLUDED_)
#define AFX_PHYSICALOBJ_H__49719AF0_6560_45FB_A928_78DD371377F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "d3dx9.h"
#include "constants.h"

class PhysicalObj
{
public:
	D3DXVECTOR3 _pos, _vel, _accel;
	float _clock;
	float _scale;

	D3DXVECTOR3 _min, _max;
	D3DXVECTOR3 _center;
	float _radius;

	// ¸¶Âû°è¼ö
	const float coef_friction = 0.99f;
	D3DXVECTOR3 friction;

	D3DXMATRIX _modelMatrix;

public:
	PhysicalObj(float x = 0, float y = 0, float z = 0);
	void SetPosition(float x, float y, float z);
	void SetVelocity(float x, float y, float z);
	void SetAcceleration(float x, float y, float z);
	void AddVelocity(float x, float y, float z);
	void BoundCheck();
	void collide(PhysicalObj& target);
	void SetBoundingBox(D3DXVECTOR3 m, D3DXVECTOR3 M);
	void SetBoundingSphere(D3DXVECTOR3 c, float r);
	void setModelMatrix(D3DXMATRIX& matWorld);
	D3DXMATRIX getWorldMatrix();
	void update(const float& dt);
	virtual ~PhysicalObj();

};

#endif // !defined(AFX_PHYSICALOBJ_H__49719AF0_6560_45FB_A928_78DD371377F2__INCLUDED_)
