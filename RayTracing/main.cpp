#include<dxlib.h>
#include <cmath>
#include <algorithm>
#include"Geometry.h"
const int screen_width = 640;
const int screen_height = 480;

//�q���g�ɂȂ�Ǝv���āA�F�X�Ɗ֐���p�ӂ��Ă���܂���
//�ʂɂ��̊֐����g��Ȃ���΂����Ȃ��킯�ł��A����ɉ���Ȃ���΂����Ȃ��킯�ł�
//����܂���B���C�g���[�V���O���ł��Ă���΍\���܂���B

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

///���C�g���[�V���O
///@param eye ���_���W
///@param sphere ���I�u�W�F�N�g(���̂��������ɂ���)
void RayTracing(const Position3& eye, const Sphere& sphere) {
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
			if (IsHitRayAndObject(eye, ray, sphere,t)) {
				//t�̎�肤��͈͂�300�`400�ƍl���ĐF�������܂�
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
	SetMainWindowText(_T("2216008_���t��"));
	DxLib_Init();

	RayTracing(Vector3(0, 0, 300), Sphere(100, Position3(0, 0, -100)));

	WaitKey();
	DxLib_End();
}