#include<dxlib.h>
#include <cmath>
#include <algorithm>
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
bool
IsHitRayAndObject(const Position3& eye, const Vector3& ray, const Plane& plane, float& t) {

	// ������Ȃ�����
	// ���C�̃x�N�g���Ɩ@���x�N�g���̂Ȃ��p�x��
	// 90�������ł��邱��
	auto dotRayAndN = Dot(ray, plane.N);
	if (dotRayAndN >= 0.0f)
	{
		return false;
	}

	// ������ꍇ�́A��_�܂ł̋��������߂�
	// t=(d-P0�EN)/(V�EN)
	t = (plane.offset - Dot(eye, plane.N)) / dotRayAndN;

	return true;

}

///���C(����)�Ƌ��̂̓����蔻��
///@param ray (���_����X�N���[���s�N�Z���ւ̃x�N�g��)
///@param sphere ��
///@hint ���C�͐��K�����Ƃ����ق����g���₷�����낤
///@param t ��_�܂ł̋��� 
bool IsHitRayAndObject(const Position3& eye,const Vector3& ray,const Sphere& sp, float& t) {
	//���C�����K���ς݂ł���O��Łc
	//
	//���_���狅�̒��S�ւ̃x�N�g��(����)�����܂�
	Vector3 C = sp.pos - eye;

	//�A�@�ō����C��ray�̓��ς��Ƃ�܂�
	//���S���王���ւ̓��ς��Ƃ�܂������x�N�g����
	float d = Dot(C,ray);
	t = d;

	//�B�����x�N�g���ƃx�N�g�����������āA���S����̐����������_�����߂܂�
	Vector3 V = ray * d;

	//�C���S�x�N�g��C����B�ŋ��߂��x�N�g��V���Ђ��āA�����x�N�g��W�����߂܂�
	Vector3 W = C - V;

	//�E�O�����̒藝�ɂ��
	//�ΕӁ����a^2������^2�{�߂肱��^2
	//�m�肽���̂͂߂荞�ݗʁ�s s��sqrtf(���a^2�[����^2�j
	float s = sqrtf(sp.radius * sp.radius - W.SQLength());
	t -= s;

	//�DW�̑傫����Length()�֐��Ŏ擾���A��������̔��aradius�Ɣ�r����
	//W.Length()�����a�ȓ��������瓖�����Ă��邩��true�Ȃ�false��Ԃ�
	return W.Length() <= sp.radius;

}

int imgHandle;

float Saturate(float val)
{
	return clamp(val, 0.0f, 1.0f);
}

// Color�͐F��0�`1�ŕ\��
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

///���C�g���[�V���O
///@param eye ���_���W
///@param sphere ���I�u�W�F�N�g(���̂��������ɂ���)
void RayTracing(const Position3& eye, const Sphere& sphere, const Plane& plane) {
	Vector3 light = { 1, -1, -1 };
	light.Normalize();
	for (int y = 0; y < screen_height; ++y) {//�X�N���[���c����
		for (int x = 0; x < screen_width; ++x) {//�X�N���[��������
			// 3D�̃X�N���[�����W
			Position3 screenPos = { float(x - screen_width / 2),float(screen_height / 2 - y),0.0f };
			//�@���_�ƃX�N���[�����W���王���x�N�g�������4
			Vector3 ray = screenPos - eye;
			//�A���K�����Ƃ�
			ray.Normalize();
			//�BIsHitRay�֐���True�������甒���h��Ԃ�
			//���h��Ԃ���DrawPixel�Ƃ����֐����g���B
			float t = 0.0f;
			if (IsHitRayAndObject(eye, ray, sphere, t)) {
				//t�̎�肤��͈͂�300�`400�ƍl���ĐF�������܂�
				auto C = sphere.pos - eye;
				auto N = ray * t - C;
				N.Normalize();

				float diffuse = clamp(Dot(-light, N), 0.0f, 1.0f);
				float ambient = 0.2;
				diffuse = std::max(diffuse, ambient);

				// �܂����˃x�N�g�������߂܂�
				auto R = RefrectVector(light, N);
				float specular = pow(Saturate(Dot(R, -ray)), 20.0f);
				auto half = -(light + ray);
				//half.Normalize();
				//float specular = pow(Saturate(Dot(half, N)), 40.0f);

				Color col = { 1.0f,0.0f,0.0f };
				col *= diffuse;


				// ���̂Ƃ̌�_�����_���A�����̔��˃x�N�g�������̃��C�ɂȂ�܂�
				auto hitPos = eye + ray * t;
				auto refRay = RefrectVector(ray, N);
				constexpr float reflectivity = 0.75;// ���˗�

				if (IsHitRayAndObject(hitPos, refRay, plane, t))
				{
					auto pos = hitPos + refRay * t;	// ���̍��W
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
			// ���ʂƂ̓����蔻��
			else if (IsHitRayAndObject(eye, ray, plane, t))
			{
				// ��_P��P = P0 + Vt
				// P = eye + ray * t
				auto pos = eye + ray * t;// ��_�̍��W
				//auto col = GetCheckerColor(pos);
				auto col = GetImageColor(pos,imgHandle);

				//�@���Ǝ����̌�_�����߂�
				//�A��������A�����x�N�g���̋t�x�N�g�����΂�
				//�B��_�Ƌt���x�N�g����p���Ă���Ƀ��C�g������
				//�@(���̂ƌ�_�������`�F�b�N)
				//�C���̂Ɠ������Ă���Â�����
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
	SetMainWindowText(_T("2216008_���t��"));
	DxLib_Init();
	imgHandle = LoadSoftImage("img/dog.png");
	Plane plane = { {0.0f,1.0f,0.0f},-100.0f };

	RayTracing(Vector3(0, 0, 300), Sphere(100, Position3(0, 0, -100)), plane);

	WaitKey();
	DxLib_End();
}