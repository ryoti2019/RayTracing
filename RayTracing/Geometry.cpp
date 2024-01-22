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


///内積を返す
float
Dot(const Vector2& va, const Vector2& vb) {
	return va.x*vb.x + va.y*vb.y;
}

///外積を返す
float
Cross(const Vector2& va, const Vector2& vb) {
	return va.x*vb.y - vb.x*va.y;
}

///内積演算子
float 
operator*(const Vector2& va, const Vector2& vb) {
	return Dot(va, vb);
}

///外積演算子
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


//-------ここから3Dのターン------
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


///内積を返す
float
Dot(const Vector3& va, const Vector3& vb) {
	return va.x*vb.x + va.y*vb.y+va.z*vb.z;
}

///外積を返す
Vector3
Cross(const Vector3& va, const Vector3& vb) {
	return Vector3(va.z*vb.y-va.y*vb.z,va.z*vb.x-va.x*vb.z,va.x*vb.y - vb.x*va.y);
}

///内積演算子
float
operator*(const Vector3& va, const Vector3& vb) {
	return Dot(va, vb);
}

///外積演算子
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
	//レイが正規化済みである前提で…
	//
	//視点から球体中心へのベクトル(視線)を作ります
	Vector3 C = pos - ray.pos;

	//②①で作ったCとrayの内積をとります
	//中心から視線への内積をとります＝＞ベクトル長
	float d = Dot(C, ray.vec);
	t = d;

	//③視線ベクトルとベクトル長をかけて、中心からの垂線下した点を求めます
	Vector3 V = ray.vec * d;

	//④中心ベクトルCから③で求めたベクトルVをひいて、垂線ベクトルWを求めます
	Vector3 W = C - V;

	//⑥三平方の定理により
	//斜辺＝半径^2＝垂線^2＋めりこみ^2
	//知りたいのはめり込み量＝s s＝sqrtf(半径^2ー垂線^2）
	float s = sqrtf(radius * radius - W.SQLength());
	t -= s;

	//⑤Wの大きさをLength()関数で取得し、それを球の半径radiusと比較して
	//W.Length()が半径以内だったら当たっているからtrueならfalseを返す
	return W.Length() <= radius;

}

bool Plane::IsHit(const Ray& ray, float& t) const
{

	// 当たらない条件
	// レイのベクトルと法線ベクトルのなす角度が
	// 90°未満であること
	auto dotRayAndN = Dot(ray.vec, N);
	if (dotRayAndN >= 0.0f)
	{
		return false;
	}

	// 当たる場合は、交点までの距離を求める
	// t=(d-P0・N)/(V・N)
	t = (offset - Dot(ray.pos, N)) / dotRayAndN;

	return true;

}


