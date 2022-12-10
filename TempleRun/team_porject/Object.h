#pragma once
#include "objRead.h"

class Object {
public:
	glm::vec3 Pos = glm::vec3(0.0f, 0.0f, 0.0f); //--- 객체 위치
	glm::vec3 Rot = glm::vec3(0.0f, 0.0f, 0.0f); //--- 기본 방향에서 얼마나 회전해 있는지
	glm::vec3 Scale = glm::vec3(1.0f, 1.0f, 1.0f); //--- 신축률
	std::vector< glm::vec3 > outvertex, outcolor;
	int vertex_size = 0;

	void show(unsigned int* modelLocation, glm::mat4 Convert = glm::mat4(1.0f)) {
		glm::mat4 result = glm::scale(Convert, glm::vec3(Scale.x, Scale.y, Scale.z));
		glUniformMatrix4fv(*modelLocation, 1, GL_FALSE, glm::value_ptr(result));
		glDrawArrays(GL_TRIANGLES, 0, vertex_size);
	}

	float left() {
		return Pos.x -  Scale.x / 2;
	}
	float right() {
		return Pos.x + Scale.x / 2;
	}
	float back() {
		return Pos.z - Scale.z / 2;
	}
	float front() {
		return Pos.z + Scale.z / 2;
	}
	float top() {
		return Pos.y + Scale.y;
	}
	float bottom() {
		return Pos.y;
	}
	void init(objRead origin) {
		outvertex = origin.outvertex;
		vertex_size = origin.outvertex.size();
	}
	void rand_init_color() {
		glm::vec3 input;
		double max = 32767;
		input[0] = rand() / max;
		input[1] = rand() / max;
		input[2] = rand() / max;
		input[0] += 0.6;
		if (input[0] > 1.0)
			input[0] = 1.0;
		for (int i = 0; i < 6; i++) {//육면체 기준 각면마다 색 달라짐.
			for (int j = 0; j < 6; j++) {
				outcolor.push_back(input);
			}
			input[0] -= 0.1;
		}
	}
};