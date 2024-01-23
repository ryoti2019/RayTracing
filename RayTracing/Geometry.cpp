#include"Geometry.h"
#include<DxLib.h>
#include<cmath>


void
Rect::Draw() {
	DxLib::DrawBox(Left()*2, Top()*2, Right()*2, Bottom()*2, 0xffffffff, false);
}

void
Rect::Draw(Vector2& offset) {
	DxLib::DrawBox((Left()+offset.x)*2, (Top()+offset.y)*2, (Right()+offset.x)*2, (Bottom()+offset.y)*2, 0xffffffff, false);
}

void
Vector2::operator*=(float scale) {
	x *= scale;
	y *= scale;
}

Vector2
Vector2::operator*(float scale) {
	return Vector2(x*scale, y*scale);
}

Vector2 operator+(const Vector2& va, const Vector2 vb) {
	return Vector2(va.x + vb.x, va.y + vb.y);
}

Vector2 operator-(const Vector2& va, const Vector2 vb){
	return Vector2(va.x - vb.x, va.y - vb.y);
}

float
Vector2::Magnitude()const {
	return hypot(x, y);
}


void 
Vector2::Normalize() {
	float mag = Magnitude();
	x /= mag;
	y /= mag;
}


Vector2
Vector2::Normalized() {
	float mag = Magnitude();
	return Vector2(x / mag,	y /mag);
}


///���ς�Ԃ�
float
Dot(const Vector2& va, const Vector2& vb) {
	return va.x*vb.x + va.y*vb.y;
}

///�O�ς�Ԃ�
float
Cross(const Vector2& va, const Vector2& vb) {
	return va.x*vb.y - vb.x*va.y;
}

///���ω��Z�q
float 
operator*(const Vector2& va, const Vector2& vb) {
	return Dot(va, vb);
}

///�O�ω��Z�q
float 
operator%(const Vector2& va, const Vector2& vb) {
	return Cross(va, vb);
}


void 
Vector2::operator+=(const Vector2& v) {
	x += v.x;
	y += v.y;
}
void 
Vector2::operator-=(const Vector2& v) {
	x -= v.x;
	y -= v.y;
}


//-------��������3D�̃^�[��------
void
Vector3::operator*=(float scale) {
	x *= scale;
	y *= scale;
	z *= scale;
}

Vector3
Vector3::operator*(float scale)const {
	return Vector3(x*scale, y*scale,z*scale);
}

Vector3 operator+(const Vector3& va, const Vector3 vb) {
	return Vector3(va.x + vb.x, va.y + vb.y,va.z+vb.z);
}

Vector3 operator-(const Vector3& va, const Vector3 vb) {
	return Vector3(va.x - vb.x, va.y - vb.y,va.z-vb.z);
}

float
Vector3::Length()const {
	return sqrt(SQLength());
}

float Vector3::SQLength() const
{
	return x * x + y * y + z * z;
}


void
Vector3::Normalize() {
	float mag = Length();
	x /= mag;
	y /= mag;
	z /= mag;
}


Vector3
Vector3::Normalized() {
	float mag = Length();
	return Vector3(x / mag, y / mag,z/mag);
}


///���ς�Ԃ�
float
Dot(const Vector3& va, const Vector3& vb) {
	return va.x*vb.x + va.y*vb.y+va.z*vb.z;
}

///�O�ς�Ԃ�
Vector3
Cross(const Vector3& va, const Vector3& vb) {
	return Vector3(va.z*vb.y-va.y*vb.z,va.z*vb.x-va.x*vb.z,va.x*vb.y - vb.x*va.y);
}

///���ω��Z�q
float
operator*(const Vector3& va, const Vector3& vb) {
	return Dot(va, vb);
}

///�O�ω��Z�q
Vector3
operator%(const Vector3& va, const Vector3& vb) {
	return Cross(va, vb);
}


void
Vector3::operator+=(const Vector3& v) {
	x += v.x;
	y += v.y;
	z += v.z;
}
void
Vector3::operator-=(const Vector3& v) {
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

bool Sphere::IsHit(const Ray& ray, float& t) const
{
	//���C�����K���ς݂ł���O��Łc
	//
	//���_���狅�̒��S�ւ̃x�N�g��(����)�����܂�
	Vector3 C = pos - ray.pos;

	//�A�@�ō����C��ray�̓��ς��Ƃ�܂�
	//���S���王���ւ̓��ς��Ƃ�܂������x�N�g����
	float d = Dot(C, ray.vec);
	t = d;

	//�B�����x�N�g���ƃx�N�g�����������āA���S����̐����������_�����߂܂�
	Vector3 V = ray.vec * d;

	//�C���S�x�N�g��C����B�ŋ��߂��x�N�g��V���Ђ��āA�����x�N�g��W�����߂܂�
	Vector3 W = C - V;

	//�E�O�����̒藝�ɂ��
	//�ΕӁ����a^2������^2�{�߂肱��^2
	//�m�肽���̂͂߂荞�ݗʁ�s s��sqrtf(���a^2�[����^2�j
	float s = sqrtf(radius * radius - W.SQLength());
	t -= s;

	//�DW�̑傫����Length()�֐��Ŏ擾���A��������̔��aradius�Ɣ�r����
	//W.Length()�����a�ȓ��������瓖�����Ă��邩��true�Ȃ�false��Ԃ�
	return W.Length() <= radius;

}

bool Plane::IsHit(const Ray& ray, float& t) const
{

	// ������Ȃ�����
	// ���C�̃x�N�g���Ɩ@���x�N�g���̂Ȃ��p�x��
	// 90�������ł��邱��
	auto dotRayAndN = Dot(ray.vec, N);
	if (dotRayAndN >= 0.0f)
	{
		return false;
	}

	// ������ꍇ�́A��_�܂ł̋��������߂�
	// t=(d-P0�EN)/(V�EN)
	t = (offset - Dot(ray.pos, N)) / dotRayAndN;

	return true;

}


