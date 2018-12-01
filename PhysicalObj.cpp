// PhysicalObj.cpp: implementation of the PhysicalObj class.
//
//////////////////////////////////////////////////////////////////////

#include "PhysicalObj.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


PhysicalObj::PhysicalObj(float x, float y, float z)
{
	_pos.x = _pos.y = _pos.z = 0.f;
	_vel.x = _vel.y = _vel.z = 0.f;
	_accel.x = _accel.y = _accel.z = 0.f;
	//_accel.z = -9.8f;

	_clock = 0;
	_scale = 50;
	_min = _max = D3DXVECTOR3(0.f, 0.f, 0.f);
}

PhysicalObj::~PhysicalObj()
{

}


void PhysicalObj::SetPosition(float x, float y, float z)
{
	_pos.x = x;
	_pos.y = y;
	_pos.z = z;
}

void PhysicalObj::SetVelocity(float x, float y, float z)
{
	_vel.x = x;
	_vel.y = y;
	_vel.z = z;
}

void PhysicalObj::AddVelocity(float x, float y, float z)
{
	_vel.x += x;
	_vel.y += y;
	_vel.z += z;
}

void PhysicalObj::SetAcceleration(float x, float y, float z)
{
	_accel.x = x;
	_accel.y = y;
	_accel.z = z;
}

void PhysicalObj::BoundCheck()
{
	float e = 1.0f;

	if (_pos.y + _min.y < 0) {
		_pos.y = -_min.y;
		_vel.y = (float)fabs(_vel.y) * e;
	}

	if (_pos.x + _min.x < -200.f) {
		_pos.x = -200 - _min.x;
		_vel.x = (float)fabs(_vel.x) * e;
	}
	if (_pos.x + _max.x > 200.f) {
		_pos.x = 200 - _max.x;
		_vel.x = (float)-fabs(_vel.x) * e;
	}

	// z
	if (_pos.z + _min.z < -200.f) {
		_pos.z = -200 - _min.z;
		_vel.z = (float)fabs(_vel.z) * e;
	}
	if (_pos.z + _max.z > 200.f) {
		_pos.z = 200 - _max.z;
		_vel.z = (float)-fabs(_vel.z) * e;
	}

}

void PhysicalObj::collide(PhysicalObj& target)
{
	D3DXVECTOR3 distance = (_pos + _center) - (target._pos + target._center); // 엄밀하게는 scale도 포함!
	float length = D3DXVec3Length(&distance);
	float rsum = _radius + target._radius;
	
	if (rsum >= length) { // collision!
		D3DXVECTOR3 d = target._pos - _pos; // normal
		D3DXVec3Normalize(&d, &d);

		D3DXVECTOR3 mv1 = d * D3DXVec3Dot(&d, &_vel);
		D3DXVECTOR3 mv2 = d * D3DXVec3Dot(&d, &target._vel);

		_vel = (_vel - mv1) + mv2;
		target._vel = (target._vel - mv2) + mv1;
	}
}

void PhysicalObj::SetBoundingBox(D3DXVECTOR3 m, D3DXVECTOR3 M)
{
	_min = m * _scale;
	_max = M * _scale;
}

void PhysicalObj::SetBoundingSphere(D3DXVECTOR3 c, float r)
{
	_center = c * _scale;
	_radius = r * _scale;
}

void PhysicalObj::setModelMatrix(D3DXMATRIX& matWorld)
{
	_modelMatrix = matWorld;
	D3DXMATRIX matScale;
	D3DXMatrixTranslation(&_modelMatrix, _pos.x, _pos.y, _pos.z);
	D3DXMatrixScaling(&matScale, _scale, _scale, _scale);
	D3DXMatrixMultiply(&_modelMatrix, &matScale, &_modelMatrix);
}

D3DXMATRIX PhysicalObj::getWorldMatrix()
{
	return _modelMatrix;
}

void PhysicalObj::update(const float& dt)
{
	_vel += _accel * dt;
	_vel.y += -.8f * dt;
	_pos += _vel * dt;
	BoundCheck();
	_vel *= coef_friction;
}
