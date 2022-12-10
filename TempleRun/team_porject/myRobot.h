#pragma once
#include "stdafx.h"
#include "Object.h"
#include "myCamera.h"
#include "userInput.h"

class KebordInput {
public:
	bool A_Down, S_Down, W_Down, D_Down, Space_Down;
};

class MyRobot {
	int North = 0;
	int East = 1;
	int South = 2;
	int West = 3;
#define parts_total 6
#define body box[0]
#define head box[1]
#define rightLeg box[2]
#define leftLeg box[3]
#define rightArm box[4]
#define leftArm box[5]
public:
	int parts = parts_total;
	glm::vec3 Pos = glm::vec3(0.0f, 0.0f, 0.0f); //--- 객체 위치
	glm::vec3 Rot = glm::vec3(0.0f, 0.0f, 0.0f); //--- 기본 방향에서 얼마나 회전해 있는지
	glm::vec3 Scale = glm::vec3(1.0f, 1.0f, 1.0f); //--- 신축률
	//KebordInput userInput;
	MyCamera Camera[2];//0번이 1인칭
	float rot_face_h = 0.0;
	float rot_face_w = 0.0;

	int face_dir = 0;

	float jump_force = 0.2;
	float current_jump_force = 0.0;
	float fall_speed = 0.0;

	float walk_speed = 0.1;

	int leg_rot_dir = 1;
	bool jump_state = false;

	Object box[parts_total];
	GLuint VAO[parts_total];
	GLuint VBO_position[parts_total];
	GLuint VBO_normal[parts_total];
	GLuint VBO_color[parts_total];


	void init() {
		objRead a;
		a.loadObj_normalize_center("cube_ground.obj");
		for (int i = 0; i < parts_total; i++) {
			box[i].init(a);
			box[i].rand_init_color();
		}
		rightLeg.Scale = glm::vec3(0.2, 1.2, 0.2);
		leftLeg.Scale = glm::vec3(0.2, 1.2, 0.2);
		rightArm.Scale = glm::vec3(0.2, 1.2, 0.2);
		leftArm.Scale = glm::vec3(0.2, 1.2, 0.2);
		body.Scale = glm::vec3(1.0, 1.5, 1.0);
		head.Scale = glm::vec3(0.7, 0.7, 0.7);
		reset();

	}
	void reset() {
		for (int i = 0; i < parts_total; i++) {
			box[i].Pos.x = 0.0;
			box[i].Pos.y = 0.0;
			box[i].Pos.z = 0.0;
			box[i].Rot.x = 0.0;
			box[i].Rot.y = 0.0;
			box[i].Rot.z = 0.0;
		}
		body.Pos.y = rightLeg.Scale.y;
		head.Pos.y = body.Pos.y + body.Scale.y;

		rightLeg.Pos.x = +rightLeg.Scale.x;
		leftLeg.Pos.x = -leftLeg.Scale.x;

		rightArm.Pos.y = body.Pos.y + body.Scale.y;
		rightArm.Pos.x = (body.Scale.x / 2 + rightArm.Scale.x / 2);

		leftArm.Pos.y = body.Pos.y + body.Scale.y;
		leftArm.Pos.x = -(body.Scale.x / 2 + leftArm.Scale.x / 2);
		rightArm.Rot.z = -15;
		leftArm.Rot.z = 15;
		Pos = glm::vec3(0.0f, 0.0f, 0.0f);
		Rot = glm::vec3(0.0f, 0.0f, 0.0f); //--- 기본 방향
		Scale = glm::vec3(1.0f, 1.0f, 1.0f); //--- 신축률
		Camera[0].Pos.x = Pos.x;
		Camera[0].Pos.z = Pos.z;
		Camera[0].Pos.y = Pos.y + 3.4;
		Camera[0].Dir = glm::vec4(Camera[0].Pos.x, Camera[0].Pos.y, Camera[0].Pos.z + 1, 1.0f); //--- 카메라가 바라보는 방향
	}
	void show(unsigned int* modelLocation) {
		//glBindVertexArray(VAO[0]);
		for (int i = 0; i < 2; i++) {
			glBindVertexArray(VAO[i]);
			glm::mat4 Init = glm::mat4(1.0f);
			glm::mat4 R_Dir = glm::mat4(1.0f);
			glm::mat4 T_Pos = glm::mat4(1.0f);
			glm::mat4 Convert = glm::mat4(1.0f);//변환행렬 초기화
			Init = glm::translate(Init, glm::vec3(box[i].Pos.x, box[i].Pos.y, box[i].Pos.z));
			R_Dir = glm::rotate(R_Dir, glm::radians(Rot.y), glm::vec3(0.0, 1.0, 0.0));
			T_Pos = glm::translate(T_Pos, glm::vec3(Pos.x, Pos.y, Pos.z));
			Convert = T_Pos * R_Dir * Init;
			box[i].show(modelLocation, Convert);
		}
		for (int i = 2; i < 4; i++) {
			glBindVertexArray(VAO[i]);
			glm::mat4 tr1 = glm::mat4(1.0f);
			glm::mat4 R_leg = glm::mat4(1.0f);
			glm::mat4 tr2 = glm::mat4(1.0f);
			glm::mat4 Init = glm::mat4(1.0f);
			glm::mat4 R_Dir = glm::mat4(1.0f);
			glm::mat4 T_Pos = glm::mat4(1.0f);
			glm::mat4 Convert = glm::mat4(1.0f);//변환행렬 초기화
			tr1 = glm::translate(tr1, glm::vec3(0.0, -box[i].Scale.y, 0.0));
			R_leg = glm::rotate(R_leg, glm::radians(box[i].Rot.x), glm::vec3(1.0, 0.0, 0.0));
			tr2 = glm::translate(tr2, glm::vec3(0.0, box[i].Scale.y, 0.0));
			Init = glm::translate(Init, glm::vec3(box[i].Pos.x, box[i].Pos.y, box[i].Pos.z));
			R_Dir = glm::rotate(R_Dir, glm::radians(Rot.y), glm::vec3(0.0, 1.0, 0.0));
			T_Pos = glm::translate(T_Pos, glm::vec3(Pos.x, Pos.y, Pos.z));
			Convert = T_Pos * R_Dir * Init * tr2 * R_leg * tr1;
			box[i].show(modelLocation, Convert);
		}
		for (int i = 4; i < 6; i++) {
			glBindVertexArray(VAO[i]);
			glm::mat4 rot1 = glm::mat4(1.0f);
			glm::mat4 R_Arm = glm::mat4(1.0f);
			glm::mat4 Init = glm::mat4(1.0f);
			glm::mat4 R_Dir = glm::mat4(1.0f);
			glm::mat4 T_Pos = glm::mat4(1.0f);
			glm::mat4 Convert = glm::mat4(1.0f);//변환행렬 초기화
			rot1 = glm::rotate(rot1, glm::radians(box[i].Rot.z), glm::vec3(0.0, 0.0, 1.0));
			R_Arm = glm::rotate(R_Arm, glm::radians(box[i].Rot.x), glm::vec3(1.0, 0.0, 0.0));
			Init = glm::translate(Init, glm::vec3(box[i].Pos.x, box[i].Pos.y, box[i].Pos.z));
			R_Dir = glm::rotate(R_Dir, glm::radians(Rot.y), glm::vec3(0.0, 1.0, 0.0));
			T_Pos = glm::translate(T_Pos, glm::vec3(Pos.x, Pos.y, Pos.z));
			Convert = T_Pos * R_Dir * Init * R_Arm * rot1;
			box[i].show(modelLocation, Convert);
		}
	}
	void initBuffer(GLuint s_program) {
		glGenVertexArrays(parts_total, VAO);
		glGenBuffers(parts_total, VBO_position);
		glGenBuffers(parts_total, VBO_color);
		GLint pAttribute = glGetAttribLocation(s_program, "vPos");
		GLint cAttribute = glGetAttribLocation(s_program, "in_Color");
		for (int i = 0; i < parts_total; i++) {
			glBindVertexArray(VAO[i]);
			//---위치속성
			glBindBuffer(GL_ARRAY_BUFFER, VBO_position[i]);
			glBufferData(GL_ARRAY_BUFFER, box[i].outvertex.size() * sizeof(glm::vec3), &box[i].outvertex[0], GL_STATIC_DRAW);

			glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glEnableVertexAttribArray(pAttribute);

			//---색상속성
			glBindBuffer(GL_ARRAY_BUFFER, VBO_color[i]);
			glBufferData(GL_ARRAY_BUFFER, box[i].outvertex.size() * sizeof(glm::vec3), &box[i].outcolor[0], GL_STATIC_DRAW);

			glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glEnableVertexAttribArray(cAttribute);
		}
	}
	
	void Camera_update() {
		float P_dx = Pos.x - Camera[0].Pos.x;
		float P_dy = (Pos.y+ 3.4) - Camera[0].Pos.y;
		float P_dz = Pos.z - Camera[0].Pos.z;

		float R_dy = Rot.y - Camera[0].Rot.y;

		Camera[0].Pos.x = Pos.x;
		Camera[0].Pos.z = Pos.z;
		Camera[0].Pos.y = Pos.y + 3.4;
		Camera[0].Rot.y = Rot.y;

		glm::mat4 CT = glm::mat4(1.0f);
		glm::mat4 CR_x = glm::mat4(1.0f);
		glm::mat4 CR_y = glm::mat4(1.0f);
		glm::mat4 CC = glm::mat4(1.0f); 
		glm::vec4 sample = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		CR_x = glm::rotate(CR_x, glm::radians(Camera[0].Rot.x), glm::vec3(1.0, 0.0, 0.0));
		CR_y = glm::rotate(CR_y, glm::radians(Camera[0].Rot.y), glm::vec3(0.0, 1.0, 0.0));
		//CR = glm::rotate(CR, glm::radians(Camera[0].Rot.y), glm::vec3(0.0, 1.0, 0.0));
		CT = glm::translate(CT, glm::vec3(Camera[0].Pos.x, Camera[0].Pos.y, Camera[0].Pos.z));
		Camera[0].Dir = CT * CR_y * CR_x * sample;// *Camera[0].Dir;
	}

	void move() {
		if (userInput.Space_Down) {
			if (jump_state == false) {
				jump_state = true;
				current_jump_force = jump_force;
			}
		}
		bool W_move{}, H_move{};
		if ((userInput.A_Down && !userInput.D_Down) || (!userInput.A_Down && userInput.D_Down))
			W_move = true;
		if ((userInput.W_Down && !userInput.S_Down) || (!userInput.W_Down && userInput.S_Down))
			H_move = true;
		if (!W_move && !H_move)
			return;
		float m_speed = walk_speed;
		if (W_move && H_move)
			m_speed *= 0.707;

		bool left = 0, right = 0, front = 0, back = 0;
		if (W_move)
			if (userInput.A_Down)
				left = true;
			else
				right = true;
		if (H_move)
			if (userInput.W_Down)
				front = true;
			else
				back = true;

		if (right) {
			if (front) {
				Pos.x += m_speed * std::cos(glm::radians(Rot.y));
				Pos.z -= m_speed * std::sin(glm::radians(Rot.y));
				Pos.x += m_speed * std::sin(glm::radians(Rot.y));
				Pos.z += m_speed * std::cos(glm::radians(Rot.y));
			}
			else if (back) {
				Pos.x += m_speed * std::cos(glm::radians(Rot.y));
				Pos.z -= m_speed * std::sin(glm::radians(Rot.y));
				Pos.x -= m_speed * std::sin(glm::radians(Rot.y));
				Pos.z -= m_speed * std::cos(glm::radians(Rot.y));
			}
			else {
				Pos.x += m_speed * std::cos(glm::radians(Rot.y));
				Pos.z -= m_speed * std::sin(glm::radians(Rot.y));
			}
			return;
		}
		if (left) {
			if (front) {
				Pos.x -= m_speed * std::cos(glm::radians(Rot.y));
				Pos.z += m_speed * std::sin(glm::radians(Rot.y));
				Pos.x += m_speed * std::sin(glm::radians(Rot.y));
				Pos.z += m_speed * std::cos(glm::radians(Rot.y));
			}
			else if (back) {
				Pos.x -= m_speed * std::cos(glm::radians(Rot.y));
				Pos.z += m_speed * std::sin(glm::radians(Rot.y));
				Pos.x -= m_speed * std::sin(glm::radians(Rot.y));
				Pos.z -= m_speed * std::cos(glm::radians(Rot.y));
			}
			else {
				Pos.x -= m_speed * std::cos(glm::radians(Rot.y));
				Pos.z += m_speed * std::sin(glm::radians(Rot.y));
			}
			return;
		}
		if (front) {
			Pos.x += m_speed * std::sin(glm::radians(Rot.y));
			Pos.z += m_speed * std::cos(glm::radians(Rot.y));
			return;
		}
		else {
			Pos.x -= m_speed * std::sin(glm::radians(Rot.y));
			Pos.z -= m_speed * std::cos(glm::radians(Rot.y));
			return;
		}
	}

	void update(float gravity_force) {
		fall(gravity_force);
		move();
		walk();
		if (current_jump_force > 0) {
			jump();
		}
		else
			jump_state = false;
		Camera_update();
	}

	void walk() {
		rightLeg.Rot.x += 2 * leg_rot_dir;
		leftLeg.Rot.x -= 2 * leg_rot_dir;
		leftArm.Rot.x += 2 * leg_rot_dir;
		rightArm.Rot.x -= 2 * leg_rot_dir;
		if (rightLeg.Rot.x >= 30 ||rightLeg.Rot.x <= -30)
			leg_rot_dir *= -1;
	}

	void fall(float gravity_force) {
		Pos.y -= fall_speed;
		fall_speed += 0.005;
		if (fall_speed > gravity_force)
			fall_speed = gravity_force;
	}

	void jump() {
		fall_speed = 0.0;
		Pos.y += current_jump_force;
		current_jump_force -= 0.005;
	}

	float left() {
		return Pos.x - 0.3;
	}
	float right() {
		return Pos.x + 0.3;
	}
	float back() {
		return Pos.z - 0.3;
	}
	float front() {
		return Pos.z + 0.3;
	}
	float top() {
		return Pos.y + head.Pos.y + head.Scale.y;
	}
	float bottom() {
		return Pos.y;
	}
};