#pragma once
#include "stdafx.h"

class MyCamera {
public:
	static int total;
	glm::vec3 Rot = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라가 기본 방향에서 얼마나 회전해 있는지
	glm::vec4 Pos = glm::vec4(0.0f, 15.0f, -15.0f, 1.0f); //--- 카메라 위치
	glm::vec4 Dir = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); //--- 카메라가 바라보는 방향
	glm::vec4 Up = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); //--- 카메라 위쪽 방향

};
int	MyCamera::total = 2;