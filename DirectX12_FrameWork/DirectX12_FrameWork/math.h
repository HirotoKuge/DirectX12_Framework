#pragma once
#include<DirectXMath.h>

constexpr float PI = 3.1415926535;

/**
 * ￥brief 度数法表記の角度をラジアンに変換
 * \param 変換したいやつ(度数法)
 * \return 変換したやつ(ラジアン)
 */
float ToRad(float radius) {
	float rad = radius * PI / 180;

	return rad;
}


class Quaternion {
public:

private:
};
