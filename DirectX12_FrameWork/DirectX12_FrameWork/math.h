#pragma once
#include<DirectXMath.h>

constexpr float PI = 3.1415926535;

/**
 * ��brief �x���@�\�L�̊p�x�����W�A���ɕϊ�
 * \param �ϊ����������(�x���@)
 * \return �ϊ��������(���W�A��)
 */
float ToRad(float radius) {
	float rad = radius * PI / 180;

	return rad;
}


class Quaternion {
public:

private:
};
