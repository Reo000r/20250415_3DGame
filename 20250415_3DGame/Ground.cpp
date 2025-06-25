#include "Ground.h"
#include "Geometry.h"
#include <DxLib.h>

namespace {
	constexpr int kMaxXZ = 1000;
	constexpr int kDivNum = 30;
	constexpr int kPerOneDist = (kMaxXZ * 2) / kDivNum;
}

Ground::Ground() {
}

Ground::~Ground() {
}

void Ground::Init() {
}

void Ground::Update() {
}

void Ground::Draw() {
	unsigned int color = 0xffffff;
	Vector3 pos1, pos2;
	pos1 = { 0,0,0 };
	pos2 = { 0,400,0 };
	DrawSphere3D(pos1, 10, 16, 0x888888, 0xff00ff, true);
	DrawLine3D(pos1, pos2, color);
	pos1 = { -kMaxXZ, 0,  kMaxXZ };
	pos2 = { -kMaxXZ, 0, -kMaxXZ };
	color = 0x00cc00;
	for (int i = 0; i < kDivNum + 1; ++i) {
		DrawLine3D(pos1, pos2, color);
		pos1.x += kPerOneDist;
		pos2.x = pos1.x;
	}
	pos1 = { kMaxXZ, 0, -kMaxXZ };
	pos2 = { -kMaxXZ, 0, -kMaxXZ };
	color = 0x0000cc;
	for (int i = 0; i < kDivNum + 1; ++i) {
		DrawLine3D(pos1, pos2, color);
		pos1.z += kPerOneDist;
		pos2.z = pos1.z;
	}
}
