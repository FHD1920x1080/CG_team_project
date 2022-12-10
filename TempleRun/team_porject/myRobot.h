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
	int Run = 0;
	int Sliding = 1;
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
	MyCamera Camera[2];//0번이 1인칭, 1은 3인칭
	bool cur_camera_mode = 0;//0이 1인칭, 1이면 3인칭
	float rot_face_h = 0.0;
	float rot_face_w = 0.0;

	int face_dir = 0;//바라보는 방향

	float max_up_speed = 0.5;
	float cur_up_speed = 0.0;
	float max_fall_speed = 0.5;
	float cur_fall_speed = 0.0;
	float gravity_accel = 0.01;
	float run_speed = 0.3;//걷는속도
	int max_sliding_time = 100;//슬라이딩 지속시간
	int cur_sliding_time = 0;//남은 지속시간

	int leg_rot_dir = 1;
	int cur_state = 0;//Run, Sliding
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
		Camera[0].Pos = glm::vec4(Pos.x, Pos.y + 3.4, Pos.z, 1.0f);
		Camera[0].Dir = glm::vec4(Camera[0].Pos.x, Camera[0].Pos.y, Camera[0].Pos.z + 1, 1.0f); //--- 카메라가 바라보는 방향
		Camera[1].Pos = glm::vec4(Pos.x, Camera[0].Pos.y, Pos.z - 20, 1.0f);
		Camera[1].Dir = glm::vec4(Camera[0].Pos.x, Camera[0].Pos.y, Camera[0].Pos.z, 1.0f); //--- 카메라가 바라보는 방향
	}
	void show(unsigned int* modelLocation) {
		//glBindVertexArray(VAO[0]);
		glm::mat4 R_Sliding = glm::mat4(1.0f);
		if (cur_state == Sliding)
			R_Sliding = glm::rotate(R_Sliding, glm::radians(-80.0f), glm::vec3(1.0, 0.0, 0.0));
		for (int i = 0; i < 2; i++) {//바디 헤드
			glBindVertexArray(VAO[i]);
			glm::mat4 Init = glm::mat4(1.0f);
			glm::mat4 R_Dir = glm::mat4(1.0f);
			glm::mat4 T_Pos = glm::mat4(1.0f);
			glm::mat4 Convert = glm::mat4(1.0f);//변환행렬 초기화
			Init = glm::translate(Init, glm::vec3(box[i].Pos.x, box[i].Pos.y, box[i].Pos.z));
			R_Dir = glm::rotate(R_Dir, glm::radians(Rot.y), glm::vec3(0.0, 1.0, 0.0));
			T_Pos = glm::translate(T_Pos, glm::vec3(Pos.x, Pos.y, Pos.z));
			Convert = T_Pos * R_Dir * R_Sliding * Init;
			box[i].show(modelLocation, Convert);
		}
		for (int i = 2; i < 4; i++) {//다리
			glm::mat4 R_Jump = glm::mat4(1.0f);
			if (jump_state) {
				if (i == 2)
					R_Jump = glm::rotate(R_Jump, glm::radians(10.0f), glm::vec3(0.0, 0.0, 1.0));
				else
					R_Jump = glm::rotate(R_Jump, glm::radians(-10.0f), glm::vec3(0.0, 0.0, 1.0));
			}
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
			Convert = T_Pos * R_Dir * R_Sliding * Init * tr2 * R_Jump * R_leg * tr1;
			box[i].show(modelLocation, Convert);
		}
		for (int i = 4; i < 6; i++) {//팔
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
			Convert = T_Pos * R_Dir * R_Sliding * Init * R_Arm * rot1;
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

		CT = glm::mat4(1.0f);
		
		CT = glm::translate(CT, glm::vec3(Pos.x, Pos.y, Pos.z));
		sample = glm::vec4(0.0, 3.4, - 20, 1.0f);//카메라1 디폴트값
		Camera[1].Pos = CT * CR_y * CR_x * sample;
		Camera[1].Dir = Camera[0].Pos; //--- 카메라가 바라보는 방향
	}

	void move() {
		if (userInput.Space_Down) {
			if (cur_state == Run && jump_state == false) {
				cur_fall_speed = 0.0;
				jump_state = true;
				cur_up_speed = max_up_speed;
			}
		}
		if (userInput.C_Down) {
			if (cur_state == Run && jump_state == false) {
				cur_state = Sliding;
				cur_sliding_time = max_sliding_time;
			}
		}
		bool W_move{}, H_move{};
		if ((userInput.A_Down && !userInput.D_Down) || (!userInput.A_Down && userInput.D_Down))
			W_move = true;
		if ((userInput.W_Down && !userInput.S_Down) || (!userInput.W_Down && userInput.S_Down))
			H_move = true;
		if (!W_move && !H_move)
			return;
		float m_speed = run_speed;
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

	void update() {
		move();
		if (jump_state) {
			jump();
		}
		else {
			fall();
		}
		if (cur_state == Sliding)
			sliding();
		else //Run
			run();
		Camera_update();
	}

	void run() {
		if (jump_state) {
			rightLeg.Rot.x += 0.5 * leg_rot_dir;
			leftLeg.Rot.x -= 0.5 * leg_rot_dir;
			leftArm.Rot.x += 0.5 * leg_rot_dir;
			rightArm.Rot.x -= 0.5 * leg_rot_dir;
		}
		else {
			rightLeg.Rot.x += 3 * leg_rot_dir;
			leftLeg.Rot.x -= 3 * leg_rot_dir;
			leftArm.Rot.x += 3 * leg_rot_dir;
			rightArm.Rot.x -= 3 * leg_rot_dir;
		}
		if (rightLeg.Rot.x >= 30 ||rightLeg.Rot.x <= -30)
			leg_rot_dir *= -1;
	}

	void fall() {
		Pos.y -= cur_fall_speed;
		cur_fall_speed += gravity_accel;
		if (cur_fall_speed > max_fall_speed)
			cur_fall_speed = max_fall_speed;
		if (Pos.y < 0) {//바닥면
			Pos.y = 0.0;
			cur_fall_speed = 0.0;
		}
	}

	void jump() {
		if (cur_up_speed > - max_fall_speed) {
			Pos.y += cur_up_speed;
			cur_up_speed -= gravity_accel;
		}
		else {
			cur_fall_speed = 0.0;
			jump_state = false;
		}
	}

	void sliding() {
		if (cur_sliding_time > 0) {
			cur_sliding_time--;
		}
		else
			cur_state = Run;

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