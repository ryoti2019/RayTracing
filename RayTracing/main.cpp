#include<dxlib.h>
#include <cmath>
#include <algorithm>
#include <vector>
#include"Geometry.h"

const int screen_width = 640;
const int screen_height = 480;
using namespace std;
//�q���g�ɂȂ�Ǝv���āA�F�X�Ɗ֐���p�ӂ��Ă���܂���
//�ʂɂ��̊֐����g��Ȃ���΂����Ȃ��킯�ł��A����ɉ���Ȃ���΂����Ȃ��킯�ł�
//����܂���B���C�g���[�V���O���ł��Ă���΍\���܂���B

/// <summary>
/// ���˃x�N�g��������ĕԂ�
/// </summary>
/// <param name="inVec">���˃x�N�g��</param>
/// <param name="N">�@���x�N�g��</param>
/// <returns>���˃x�N�g��</returns>
/// <remarks>�@���x�N�g���͐��K������Ă��邱��</remarks>
Vector3
RefrectVector(const Vector3& inVec, const Vector3& N)
{
	//R=I-2(I�EN)N
	return inVec - N * 2.0f * Dot(inVec, N);
}

/// <summary>
/// ���C�ƕ��ʂ̓����蔻��
/// </summary>
/// <param name="eye"></param>
/// <param name="ray"></param>
/// <param name="plane"></param>
/// <param name="t"></param>
/// <returns></returns>
//bool
//IsHitRayAndObject(const Position3& eye, const Vector3& ray, const Plane& plane, float& t) {
//
//	// ������Ȃ�����
//	// ���C�̃x�N�g���Ɩ@���x�N�g���̂Ȃ��p�x��
//	// 90�������ł��邱��
//	auto dotRayAndN = Dot(ray, plane.N);
//	if (dotRayAndN >= 0.0f)
//	{
//		return false;
//	}
//
//	// ������ꍇ�́A��_�܂ł̋��������߂�
//	// t=(d-P0�EN)/(V�EN)
//	t = (plane.offset - Dot(eye, plane.N)) / dotRayAndN;
//
//	return true;
//
//}

///���C(����)�Ƌ��̂̓����蔻��
///@param ray (���_����X�N���[���s�N�Z���ւ̃x�N�g��)
///@param sphere ��
///@hint ���C�͐��K�����Ƃ����ق����g���₷�����낤
///@param t ��_�܂ł̋��� 
//bool IsHitRayAndObject(const Position3& eye,const Vector3& ray,const Sphere& sp, float& t) {
//	//���C�����K���ς݂ł���O��Łc
//	//
//	//���_���狅�̒��S�ւ̃x�N�g��(����)�����܂�
//	Vector3 C = sp.pos - eye;
//
//	//�A�@�ō����C��ray�̓��ς��Ƃ�܂�
//	//���S���王���ւ̓��ς��Ƃ�܂������x�N�g����
//	float d = Dot(C,ray);
//	t = d;
//
//	//�B�����x�N�g���ƃx�N�g�����������āA���S����̐����������_�����߂܂�
//	Vector3 V = ray * d;
//
//	//�C���S�x�N�g��C����B�ŋ��߂��x�N�g��V���Ђ��āA�����x�N�g��W�����߂܂�
//	Vector3 W = C - V;
//
//	//�E�O�����̒藝�ɂ��
//	//�ΕӁ����a^2������^2�{�߂肱��^2
//	//�m�肽���̂͂߂荞�ݗʁ�s s��sqrtf(���a^2�[����^2�j
//	float s = sqrtf(sp.radius * sp.radius - W.SQLength());
//	t -= s;
//
//	//�DW�̑傫����Length()�֐��Ŏ擾���A��������̔��aradius�Ɣ�r����
//	//W.Length()�����a�ȓ��������瓖�����Ă��邩��true�Ȃ�false��Ԃ�
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

			// �܂����˃x�N�g�������߂܂�
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


///���C�g���[�V���O
///@param eye ���_���W
///@param sphere ���I�u�W�F�N�g(���̂��������ɂ���)
void RayTracing(const Position3& eye,Primitives_t& primitives) {
	Vector3 light = { 1, -1, -1 };
	light.Normalize();
	for (int y = 0; y < screen_height; ++y) {//�X�N���[���c����
		for (int x = 0; x < screen_width; ++x) {//�X�N���[��������
			// 3D�̃X�N���[�����W
			Position3 screenPos = { float(x - screen_width / 2),float(screen_height / 2 - y),0.0f };
			//�@���_�ƃX�N���[�����W���王���x�N�g�������
			Ray ray;
			ray.vec = screenPos - eye;
			ray.pos = eye;
			//�A���K�����Ƃ�
			ray.vec.Normalize();
			//�BIsHitRay�֐���True�������甒���h��Ԃ�
			//���h��Ԃ���DrawPixel�Ƃ����֐����g���B
			float t = 0.0f;
			Color col;
			if (TraceRay(ray, light, primitives, col, nullptr, 5)) {
				DrawPixel(x, y, GetColor(col.x * 255, col.y * 255, col.z * 255));
			}
			else {
				int b = ((x / 32 + y / 32) % 2) * 255;
				DrawPixel(x, y, GetColor(0, b, 0));
			}
			// ���̂Ƃ̓����蔻��
			//if (primitives[0]->IsHit(ray, t)) {
			//	//t�̎�肤��͈͂�300�`400�ƍl���ĐF�������܂�
			//	auto hitPos = ray.pos + ray.vec * t;
			//	auto N = primitives[0]->GetNormalVector(hitPos);

			//	float diffuse = clamp(Dot(-light, N), 0.0f, 1.0f);
			//	float ambient = 0.2;
			//	diffuse = std::max(diffuse, ambient);

			//	// �܂����˃x�N�g�������߂܂�
			//	auto R = RefrectVector(light, N);
			//	float specular = pow(Saturate(Dot(R, -ray.vec)), 20.0f);
			//	auto half = -(light + ray.vec);
			//	//half.Normalize();
			//	//float specular = pow(Saturate(Dot(half, N)), 40.0f);

			//	
			//	Color col = primitives[0]->GetColorAtPosition(hitPos);
			//	col *= diffuse;

			//	// ���̂Ƃ̌�_�����_���A�����̔��˃x�N�g�������̃��C�ɂȂ�܂�
			//	Ray refRay;
			//	refRay.pos = hitPos;
			//	refRay.vec = RefrectVector(ray.vec, N);

			//	constexpr float reflectivity = 0.75;// ���˗�
			//	if (primitives[1]->IsHit(refRay, t))
			//	{
			//		auto pos = refRay.pos + refRay.vec * t;	// ���̍��W
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
			//// ���ʂƂ̓����蔻��
			//else if (primitives[1]->IsHit(ray, t))
			//{
			//	// ��_P��P = P0 + Vt
			//	// P = eye + ray * t
			//	//auto col = GetCheckerColor(pos);

			//	Ray shadowRay;
			//	shadowRay.pos = ray.pos + ray.vec * t;// ��_�̍��W
			//	auto col = primitives[1]->GetColorAtPosition(shadowRay.pos);
			//	shadowRay.vec = -light;
			//	//�@���Ǝ����̌�_�����߂�
			//	//�A��������A�����x�N�g���̋t�x�N�g�����΂�
			//	//�B��_�Ƌt���x�N�g����p���Ă���Ƀ��C�g������
			//	//�@(���̂ƌ�_�������`�F�b�N)
			//	//�C���̂Ɠ������Ă���Â�����
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
	SetMainWindowText(_T("2216008_���t��"));
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