#include<dxlib.h>
#include <cmath>
#include <algorithm>
#include"Geometry.h"
const int screen_width = 640;
const int screen_height = 480;

//ヒントになると思って、色々と関数を用意しておりますが
//別にこの関数を使わなければいけないわけでも、これに沿わなければいけないわけでも
//ありません。レイトレーシングができていれば構いません。

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

///レイトレーシング
///@param eye 視点座標
///@param sphere 球オブジェクト(そのうち複数にする)
void RayTracing(const Position3& eye, const Sphere& sphere) {
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
			if (IsHitRayAndObject(eye, ray, sphere,t)) {
				//tの取りうる範囲が300～400と考えて色分けします
				auto C = sphere.pos - eye;
				auto N= ray* t - C;
				N.Normalize();

				float bright = std::clamp(Dot(-light, N),0.0f,1.0f);
				float ambient = 0.2;
				bright = std::max(bright, ambient);
				DrawPixel(x, y, GetColor(255 * bright, 255 * bright, 255 * bright));
			}
			else {
				int b = ((x / 32 + y / 32) % 2) * 255;
				DrawPixel(x, y, GetColor(0, b, 0));
			}
		}
	}
}

int WINAPI WinMain(HINSTANCE , HINSTANCE, LPSTR , int) {
	ChangeWindowMode(true);
	SetGraphMode(screen_width, screen_height, 32);
	SetMainWindowText(_T("2216008_寺師遼"));
	DxLib_Init();

	RayTracing(Vector3(0, 0, 300), Sphere(100, Position3(0, 0, -100)));

	WaitKey();
	DxLib_End();
}