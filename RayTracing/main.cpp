#include<dxlib.h>
#include <cmath>
#include <algorithm>
#include"Geometry.h"
const int screen_width = 640;
const int screen_height = 480;
using namespace std;
//ヒントになると思って、色々と関数を用意しておりますが
//別にこの関数を使わなければいけないわけでも、これに沿わなければいけないわけでも
//ありません。レイトレーシングができていれば構いません。

/// <summary>
/// 反射ベクトルを作って返す
/// </summary>
/// <param name="inVec">入射ベクトル</param>
/// <param name="N">法線ベクトル</param>
/// <returns>反射ベクトル</returns>
/// <remarks>法線ベクトルは正規化されていること</remarks>
Vector3
RefrectVector(const Vector3& inVec, const Vector3& N)
{
	//R=I-2(I・N)N
	return inVec - N * 2.0f * Dot(inVec, N);
}

/// <summary>
/// レイと平面の当たり判定
/// </summary>
/// <param name="eye"></param>
/// <param name="ray"></param>
/// <param name="plane"></param>
/// <param name="t"></param>
/// <returns></returns>
bool
IsHitRayAndObject(const Position3& eye, const Vector3& ray, const Plane& plane, float& t) {

	// 当たらない条件
	// レイのベクトルと法線ベクトルのなす角度が
	// 90°未満であること
	auto dotRayAndN = Dot(ray, plane.N);
	if (dotRayAndN >= 0.0f)
	{
		return false;
	}

	// 当たる場合は、交点までの距離を求める
	// t=(d-P0・N)/(V・N)
	t = (plane.offset - Dot(eye, plane.N)) / dotRayAndN;

	return true;

}

///レイ(光線)と球体の当たり判定
///@param ray (視点からスクリーンピクセルへのベクトル)
///@param sphere 球
///@hint レイは正規化しといたほうが使いやすいだろう
///@param t 交点までの距離 
bool IsHitRayAndObject(const Position3& eye,const Vector3& ray,const Sphere& sp, float& t) {
	//レイが正規化済みである前提で…
	//
	//視点から球体中心へのベクトル(視線)を作ります
	Vector3 C = sp.pos - eye;

	//②①で作ったCとrayの内積をとります
	//中心から視線への内積をとります＝＞ベクトル長
	float d = Dot(C,ray);
	t = d;

	//③視線ベクトルとベクトル長をかけて、中心からの垂線下した点を求めます
	Vector3 V = ray * d;

	//④中心ベクトルCから③で求めたベクトルVをひいて、垂線ベクトルWを求めます
	Vector3 W = C - V;

	//⑥三平方の定理により
	//斜辺＝半径^2＝垂線^2＋めりこみ^2
	//知りたいのはめり込み量＝s s＝sqrtf(半径^2ー垂線^2）
	float s = sqrtf(sp.radius * sp.radius - W.SQLength());
	t -= s;

	//⑤Wの大きさをLength()関数で取得し、それを球の半径radiusと比較して
	//W.Length()が半径以内だったら当たっているからtrueならfalseを返す
	return W.Length() <= sp.radius;

}

int imgHandle;

float Saturate(float val)
{
	return clamp(val, 0.0f, 1.0f);
}

// Colorは色を0～1で表す
using Color = Vector3;
Color
GetCheckerColor(const Position3& pos) {

	int sign = 1;

	if (((int)(pos.x / 30.0f) + (int)(pos.z / 30.0f)) % 2 == 0)
	{
		sign *= -1;
	}
	if (pos.x < 0.0f)
	{
		sign *= -1;
	}
	if (pos.z < 0.0f)
	{
		sign *= -1;
	}
	if (sign > 0)
	{
		return{ 1.0f,1.0f,0.0f };
	}
	else {
		return{ 0.5f,0.5f,1.0f };
	}
}

Color
GetImageColor(const Position3& pos, int handle, float scale = 3.0f) {

	int x = pos.x / scale;
	int y = pos.z / scale;

	bool minus = y < 0;

	int w, h;
	GetSoftImageSize(handle, &w, &h);

	float r, g, b, a;
	x = abs(x);
	y = abs(y);
	if (minus) {
		y = h - y % h;
	}
	GetPixelSoftImageF(handle, x % w, y % h, &r, &g, &b, &a);

	return{ r,g,b };

}

///レイトレーシング
///@param eye 視点座標
///@param sphere 球オブジェクト(そのうち複数にする)
void RayTracing(const Position3& eye, const Sphere& sphere, const Plane& plane) {
	Vector3 light = { 1, -1, -1 };
	light.Normalize();
	for (int y = 0; y < screen_height; ++y) {//スクリーン縦方向
		for (int x = 0; x < screen_width; ++x) {//スクリーン横方向
			// 3Dのスクリーン座標
			Position3 screenPos = { float(x - screen_width / 2),float(screen_height / 2 - y),0.0f };
			//①視点とスクリーン座標から視線ベクトルを作る4
			Vector3 ray = screenPos - eye;
			//②正規化しとく
			ray.Normalize();
			//③IsHitRay関数がTrueだったら白く塗りつぶす
			//※塗りつぶしはDrawPixelという関数を使う。
			float t = 0.0f;
			if (IsHitRayAndObject(eye, ray, sphere, t)) {
				//tの取りうる範囲が300～400と考えて色分けします
				auto C = sphere.pos - eye;
				auto N = ray * t - C;
				N.Normalize();

				float diffuse = clamp(Dot(-light, N), 0.0f, 1.0f);
				float ambient = 0.2;
				diffuse = std::max(diffuse, ambient);

				// まず反射ベクトルを求めます
				auto R = RefrectVector(light, N);
				float specular = pow(Saturate(Dot(R, -ray)), 20.0f);
				auto half = -(light + ray);
				//half.Normalize();
				//float specular = pow(Saturate(Dot(half, N)), 40.0f);

				Color col = { 1.0f,0.0f,0.0f };
				col *= diffuse;


				// 球体との交点が視点かつ、視線の反射ベクトルが次のレイになります
				auto hitPos = eye + ray * t;
				auto refRay = RefrectVector(ray, N);
				constexpr float reflectivity = 0.75;// 反射率

				if (IsHitRayAndObject(hitPos, refRay, plane, t))
				{
					auto pos = hitPos + refRay * t;	// 床の座標
					//auto refCol = GetCheckerColor(pos);
					auto refCol = GetImageColor(pos,imgHandle);
					col = col * (1.0f - reflectivity) + refCol * reflectivity;
				}



				DrawPixel(x, y, GetColor(
					std::clamp((int)(col.x * 255 + specular * 255), 0, 255),
					std::clamp((int)(col.y * 255 + specular * 255), 0, 255),
					std::clamp((int)(col.z * 255 + specular * 255), 0, 255)
				));

			}
			// 平面との当たり判定
			else if (IsHitRayAndObject(eye, ray, plane, t))
			{
				// 交点PはP = P0 + Vt
				// P = eye + ray * t
				auto pos = eye + ray * t;// 交点の座標
				//auto col = GetCheckerColor(pos);
				auto col = GetImageColor(pos,imgHandle);

				//①床と視線の交点を求める
				//②そこから、光線ベクトルの逆ベクトルを飛ばす
				//③交点と逆光ベクトルを用いてさらにレイトレする
				//　(球体と交点を持つかチェック)
				//④球体と当たってたら暗くする
				//color *= 0.5

				if (IsHitRayAndObject(pos, -light, sphere, t))
				{
					col *= 0.5f;
				}

				DrawPixel(x, y, GetColor(col.x * 255, col.y * 255, col.z * 255));
			}
			else {
				int b = ((x / 32 + y / 32) % 2) * 255;
				DrawPixel(x, y, GetColor(0, b, 0));
			}
		}
	}
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	ChangeWindowMode(true);
	SetGraphMode(screen_width, screen_height, 32);
	SetMainWindowText(_T("2216008_寺師遼"));
	DxLib_Init();
	imgHandle = LoadSoftImage("img/dog.png");
	Plane plane = { {0.0f,1.0f,0.0f},-100.0f };

	RayTracing(Vector3(0, 0, 300), Sphere(100, Position3(0, 0, -100)), plane);

	WaitKey();
	DxLib_End();
}