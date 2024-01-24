#include<dxlib.h>
#include <cmath>
#include <algorithm>
#include <vector>
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
//bool
//IsHitRayAndObject(const Position3& eye, const Vector3& ray, const Plane& plane, float& t) {
//
//	// 当たらない条件
//	// レイのベクトルと法線ベクトルのなす角度が
//	// 90°未満であること
//	auto dotRayAndN = Dot(ray, plane.N);
//	if (dotRayAndN >= 0.0f)
//	{
//		return false;
//	}
//
//	// 当たる場合は、交点までの距離を求める
//	// t=(d-P0・N)/(V・N)
//	t = (plane.offset - Dot(eye, plane.N)) / dotRayAndN;
//
//	return true;
//
//}

///レイ(光線)と球体の当たり判定
///@param ray (視点からスクリーンピクセルへのベクトル)
///@param sphere 球
///@hint レイは正規化しといたほうが使いやすいだろう
///@param t 交点までの距離 
//bool IsHitRayAndObject(const Position3& eye,const Vector3& ray,const Sphere& sp, float& t) {
//	//レイが正規化済みである前提で…
//	//
//	//視点から球体中心へのベクトル(視線)を作ります
//	Vector3 C = sp.pos - eye;
//
//	//②①で作ったCとrayの内積をとります
//	//中心から視線への内積をとります＝＞ベクトル長
//	float d = Dot(C,ray);
//	t = d;
//
//	//③視線ベクトルとベクトル長をかけて、中心からの垂線下した点を求めます
//	Vector3 V = ray * d;
//
//	//④中心ベクトルCから③で求めたベクトルVをひいて、垂線ベクトルWを求めます
//	Vector3 W = C - V;
//
//	//⑥三平方の定理により
//	//斜辺＝半径^2＝垂線^2＋めりこみ^2
//	//知りたいのはめり込み量＝s s＝sqrtf(半径^2ー垂線^2）
//	float s = sqrtf(sp.radius * sp.radius - W.SQLength());
//	t -= s;
//
//	//⑤Wの大きさをLength()関数で取得し、それを球の半径radiusと比較して
//	//W.Length()が半径以内だったら当たっているからtrueならfalseを返す
//	return W.Length() <= sp.radius;
//
//}

int imgHandle;

float Saturate(float val)
{
	return clamp(val, 0.0f, 1.0f);
}

using Primitives_t = std::vector<Primitive*>;

bool TraceRay(const Ray& ray, const Vector3& light, Primitives_t& primitives, Color& retCol, Primitive* self = nullptr, int limit = 5) {

	for (auto& prim : primitives) {
		float t;
		if (prim == self) {
			continue;
		}
		if (prim->IsHit(ray, t)) {

			if (t < 0.0f) {
				continue;
			}
			auto hitPos = ray.pos + ray.vec * t;
			auto N = prim->GetNormalVector(hitPos);
			retCol = prim->GetColorAtPosition(hitPos);

			if (prim->material.isNotShading){
				return true;
			}

			float diffuse = clamp(Dot(-light, N), 0.0f, 1.0f);
			float ambient = prim->material.ambient;
			diffuse = std::max(diffuse, ambient);

			// まず反射ベクトルを求めます
			auto R = RefrectVector(light, N);

			float specular = pow(Saturate(Dot(R, -ray.vec)), 20.0f);
			retCol *= diffuse;
			retCol.x = std::clamp(retCol.x + specular,0.0f,1.0f);
			retCol.y = std::clamp(retCol.y + specular,0.0f,1.0f);
			retCol.z = std::clamp(retCol.z + specular,0.0f,1.0f);

			if (prim->material.reflectivity > 0.0f) {
				Ray refRay;
				refRay.pos = hitPos;
				refRay.vec = RefrectVector(ray.vec, N);
				Color refCol = {};
				if (TraceRay(refRay, light, primitives, refCol, prim, limit - 1)) {
					retCol = retCol * (1.0f - prim->material.reflectivity) +
						refCol * prim->material.reflectivity;
				}
			}
			return true;
		}
	}
	return false;
}


///レイトレーシング
///@param eye 視点座標
///@param sphere 球オブジェクト(そのうち複数にする)
void RayTracing(const Position3& eye,Primitives_t& primitives) {
	Vector3 light = { 1, -1, -1 };
	light.Normalize();
	for (int y = 0; y < screen_height; ++y) {//スクリーン縦方向
		for (int x = 0; x < screen_width; ++x) {//スクリーン横方向
			// 3Dのスクリーン座標
			Position3 screenPos = { float(x - screen_width / 2),float(screen_height / 2 - y),0.0f };
			//①視点とスクリーン座標から視線ベクトルを作る
			Ray ray;
			ray.vec = screenPos - eye;
			ray.pos = eye;
			//②正規化しとく
			ray.vec.Normalize();
			//③IsHitRay関数がTrueだったら白く塗りつぶす
			//※塗りつぶしはDrawPixelという関数を使う。
			float t = 0.0f;
			Color col;
			if (TraceRay(ray, light, primitives, col, nullptr, 5)) {
				DrawPixel(x, y, GetColor(col.x * 255, col.y * 255, col.z * 255));
			}
			else {
				int b = ((x / 32 + y / 32) % 2) * 255;
				DrawPixel(x, y, GetColor(0, b, 0));
			}
			// 球体との当たり判定
			//if (primitives[0]->IsHit(ray, t)) {
			//	//tの取りうる範囲が300～400と考えて色分けします
			//	auto hitPos = ray.pos + ray.vec * t;
			//	auto N = primitives[0]->GetNormalVector(hitPos);

			//	float diffuse = clamp(Dot(-light, N), 0.0f, 1.0f);
			//	float ambient = 0.2;
			//	diffuse = std::max(diffuse, ambient);

			//	// まず反射ベクトルを求めます
			//	auto R = RefrectVector(light, N);
			//	float specular = pow(Saturate(Dot(R, -ray.vec)), 20.0f);
			//	auto half = -(light + ray.vec);
			//	//half.Normalize();
			//	//float specular = pow(Saturate(Dot(half, N)), 40.0f);

			//	
			//	Color col = primitives[0]->GetColorAtPosition(hitPos);
			//	col *= diffuse;

			//	// 球体との交点が視点かつ、視線の反射ベクトルが次のレイになります
			//	Ray refRay;
			//	refRay.pos = hitPos;
			//	refRay.vec = RefrectVector(ray.vec, N);

			//	constexpr float reflectivity = 0.75;// 反射率
			//	if (primitives[1]->IsHit(refRay, t))
			//	{
			//		auto pos = refRay.pos + refRay.vec * t;	// 床の座標
			//		//auto refCol = GetCheckerColor(pos);
			//		auto refCol = primitives[1]->GetColorAtPosition(pos);
			//		col = col * (1.0f - reflectivity) + refCol * reflectivity;
			//	}



			//	DrawPixel(x, y, GetColor(
			//		std::clamp((int)(col.x * 255 + specular * 255), 0, 255),
			//		std::clamp((int)(col.y * 255 + specular * 255), 0, 255),
			//		std::clamp((int)(col.z * 255 + specular * 255), 0, 255)
			//	));

			//}
			//// 平面との当たり判定
			//else if (primitives[1]->IsHit(ray, t))
			//{
			//	// 交点PはP = P0 + Vt
			//	// P = eye + ray * t
			//	//auto col = GetCheckerColor(pos);

			//	Ray shadowRay;
			//	shadowRay.pos = ray.pos + ray.vec * t;// 交点の座標
			//	auto col = primitives[1]->GetColorAtPosition(shadowRay.pos);
			//	shadowRay.vec = -light;
			//	//①床と視線の交点を求める
			//	//②そこから、光線ベクトルの逆ベクトルを飛ばす
			//	//③交点と逆光ベクトルを用いてさらにレイトレする
			//	//　(球体と交点を持つかチェック)
			//	//④球体と当たってたら暗くする
			//	//color *= 0.5

			//	if (primitives[0]->IsHit(ray, t))
			//	{
			//		col *= 0.5f;
			//	}

			//	DrawPixel(x, y, GetColor(col.x * 255, col.y * 255, col.z * 255));
			//}
			//else {
			//	int b = ((x / 32 + y / 32) % 2) * 255;
			//	DrawPixel(x, y, GetColor(0, b, 0));
			//}
		}
	}
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	ChangeWindowMode(true);
	SetGraphMode(screen_width, screen_height, 32);
	SetMainWindowText(_T("2216008_寺師遼"));
	DxLib_Init();
	imgHandle = LoadSoftImage("img/dog.png");
	Sphere sphere (Position3(0, 0, -100),100);
	sphere.material.color = {1.0f,0.0f,0.0f};
	sphere.material.ambient = 0.2f;
	sphere.material.specular = 1.0f;
	sphere.material.specularity = 20.0f;
	sphere.material.reflectivity = 0.5f;

	Sphere sphere2(Position3(200, 0, -100), 100);
	sphere2.material.color = { 0.0f,1.0f,0.0f };
	sphere2.material.ambient = 0.2f;
	sphere2.material.specular = 1.0f;
	sphere2.material.specularity = 20.0f;
	sphere2.material.reflectivity = 0.5f;

	Sphere sphere3(Position3(-200, 0, -100), 100);
	sphere3.material.color = { 0.0f,0.0f,1.0f };
	sphere3.material.ambient = 0.2f;
	sphere3.material.specular = 1.0f;
	sphere3.material.specularity = 20.0f;
	sphere3.material.reflectivity = 0.5f;

	Plane plane = { {0.0f,1.0f,0.0f},-100.0f };
	plane.material.color = { 1.0f,1.0f,0.0f };
	plane.material.subColor = { 1.0f,0.5f,0.0f };
	plane.material.pattern = Pattern::texture;
	plane.material.patternSize = { 2.0f,2.0f };
	plane.material.textHandle = imgHandle;
	plane.material.isNotShading = true;
	
	Primitives_t primitives;
	primitives.push_back(&sphere);
	primitives.push_back(&sphere2);
	primitives.push_back(&sphere3);
	primitives.push_back(&plane);

	RayTracing(Vector3(0, 0, 300), primitives);

	WaitKey();
	DxLib_End();
}