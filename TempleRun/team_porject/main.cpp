// Reading Obj file
#include "stdafx.h"
#include "shader.h"
#include "myRobot.h"
#include "userInput.h"
#include <time.h>

using namespace std;

GLuint g_window_w = 800;
GLuint g_window_h = 800;
GLuint VAO[4];
GLuint VBO_position[4];
GLuint VBO_normal[4];
GLuint VBO_color[4];

MyRobot myRobot;

void drawScene();
void Reshape(int w, int h);
void KeyDown(unsigned char key, int x, int y);
void KeyUp(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void MousePassiveDrag(int x, int y);
void MouseDrag(int x, int y);
void InitBuffer();
void enter();
void reset();
void FPS100(int value);
float* cheak_collision(float x, float y, float z, Object Rect);
bool check_collision_min_move(MyRobot* unit1, Object* unit2);

struct remote_control {
	bool depth_test = true;
	bool polygon_mode = true;
	bool perspective_projection = true;
	bool first_person = false;
};

remote_control remocon;
float fall_speed = 0;
float gravity_force = 0.2;
MyCamera myCamera[2];


typedef struct MyMode {
	bool updown_anim = 0;
	bool meteor = 0;
	bool jump = 0;
	bool make_maze = 0;
	bool low_height = 0;
}MyMode;

MyMode Anim;

int main(int argc, char** argv)
{	
	enter();
	// create window using freeglut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(g_window_w, g_window_h);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("Computer Graphics");
	//////////////////////////////////////////////////////////////////////////////////////
	//// initialize GLEW
	//////////////////////////////////////////////////////////////////////////////////////
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "GLEW OK\n";
	}


	GLuint vShader[4];
	GLuint fShader[4];

	vShader[0] = MakeVertexShader("vertex.glsl", 0);			// Sun
	fShader[0] = MakeFragmentShader("fragment.glsl", 0);

	// shader Program
	s_program[0] = glCreateProgram();
	glAttachShader(s_program[0], vShader[0]);
	glAttachShader(s_program[0], fShader[0]);
	glLinkProgram(s_program[0]);
	checkCompileErrors(s_program[0], "PROGRAM");

	
	InitBuffer();

	// callback functions
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(KeyDown);
	glutKeyboardUpFunc(KeyUp);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseDrag);
	glutTimerFunc(10, FPS100, 1);
	// freeglut 윈도우 이벤트 처리 시작. 윈도우가 닫힐때까지 후한루프 실행.
	glutMainLoop();

	return 0;
}

GLuint center_vao, center_vbo_pos, center_vbo_color;

GLfloat center_vPos[6][3] = {
	-1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	0.0, -1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 0.0, -1.0,
	0.0, 0.0, 1.0
};

GLfloat center_vColor[6][3] = {
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0
};

GLuint floor_vao, floor_vbo_pos, floor_vbo_color;

GLfloat floor_vPos[6][3] = {
	-3.0, -0.001, -3.0,
	-3.0, -0.001, 3.0,
	3.0, -0.001, -3.0,
	3.0, -0.001, -3.0,
	-3.0, -0.001, 3.0,
	3.0, -0.001, 3.0
};

GLfloat floor_vColor[6][3] = {
	0.3, 0.5, 0.3,
	0.3, 0.5, 0.3,
	0.3, 0.5, 0.3,
	0.3, 0.5, 0.3,
	0.3, 0.5, 0.3,
	0.3, 0.5, 0.3
};

void InitBuffer()
{
	GLint pAttribute = glGetAttribLocation(s_program[0], "vPos");
	GLint cAttribute = glGetAttribLocation(s_program[0], "in_Color");
	//// 5.1. VAO 객체 생성 및 바인딩
	glGenVertexArrays(1, &center_vao);
	glGenBuffers(1, &center_vbo_pos);
	glGenBuffers(1, &center_vbo_color);
	glBindVertexArray(center_vao);

	glBindBuffer(GL_ARRAY_BUFFER, center_vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), center_vPos, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, center_vbo_color);
	glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), center_vColor, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(2);

	glGenVertexArrays(1, &floor_vao);
	glGenBuffers(1, &floor_vbo_pos);
	glGenBuffers(1, &floor_vbo_color);
	glBindVertexArray(floor_vao);

	glBindBuffer(GL_ARRAY_BUFFER, floor_vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), floor_vPos, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, floor_vbo_color);
	glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), floor_vColor, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(2);

	glGenVertexArrays(1, VAO);
	glGenBuffers(1, VBO_position);
	glGenBuffers(1, VBO_color);
	glBindVertexArray(VAO[0]);
	//---위치속성
	//glBindBuffer(GL_ARRAY_BUFFER, VBO_position[0]);
	//glBufferData(GL_ARRAY_BUFFER, originBox.outvertex.size() * sizeof(glm::vec3), &originBox.outvertex[0], GL_STATIC_DRAW);

	//glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	//glEnableVertexAttribArray(pAttribute);

	////---색상속성
	//glBindBuffer(GL_ARRAY_BUFFER, VBO_color[0]);
	//glBufferData(GL_ARRAY_BUFFER, originBox.outvertex.size() * sizeof(glm::vec3), &originBox.outcolor[0], GL_STATIC_DRAW);

	//glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	//glEnableVertexAttribArray(cAttribute);

	myRobot.initBuffer(s_program[0]);

}
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);

unsigned int modelLocation;
unsigned int viewLocation;
unsigned int projectionLocation;

void drawWorld() {

	modelLocation = glGetUniformLocation(s_program[0], "modelTransform");//--- 모델 변환 설정
	glm::mat4 WC = glm::mat4(1.0f);//변환행렬 초기화

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(WC));
	glBindVertexArray(center_vao);//중앙 좌표 표시

	glDrawArrays(GL_LINES, 0, 2);
	glDrawArrays(GL_LINES, 2, 2);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(floor_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//stone.draw();
	glm::mat4 Dir;
	glm::mat4 Pos;
	glm::mat4 Scale = glm::mat4(1.0f); //--- 단위 행렬로 초기화
	glm::mat4 Init = glm::mat4(1.0f);
	glm::mat4 Convert = glm::mat4(1.0f);

	myRobot.show(&modelLocation);
}

void MyViewport0() {
	glViewport(0, 0, g_window_w, g_window_h);

	//MyCamera[0].Dir = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);//카메라 보는 방향
	if (remocon.first_person) {
		projection = glm::perspective(glm::radians(45.0f), -1.0f, 0.1f, 200.0f);// 맨뒤 두개는 카메라로부터 근평면 원평면 거리
		view = glm::lookAt(glm::vec3(myRobot.Camera[0].Pos), glm::vec3(myRobot.Camera[0].Dir), glm::vec3(myRobot.Camera[0].Up));
	}
	else {
		if (remocon.perspective_projection)
			projection = glm::perspective(glm::radians(45.0f), -1.0f, 0.1f, 200.0f);// 맨뒤 두개는 카메라로부터 근평면 원평면 거리
		else
			projection = glm::ortho(40.0f, -40.0f, -40.0f, 40.0f, 0.1f, 200.0f);
		view = glm::lookAt(glm::vec3(myCamera[0].Pos), glm::vec3(myCamera[0].Dir), glm::vec3(myCamera[0].Up));
	}
	viewLocation = glGetUniformLocation(s_program[0], "viewTransform"); //--- 뷰잉 변환 설정
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	projectionLocation = glGetUniformLocation(s_program[0], "projectionTransform"); //--- 투영 변환 값 설정
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	drawWorld();
}
void MyViewport1() {
	glViewport(g_window_w * 3 / 4, g_window_h / 2, g_window_w / 4, g_window_h / 2);
	//if (remocon.perspective_projection) {//원근 투영
	//	projection = glm::perspective(glm::radians(45.0f), -1.0f, 0.1f, 200.0f);// 맨뒤 두개는 카메라로부터 근평면 원평면 거리
	//	//projection = glm::translate(projection, glm::vec3(0.0, 0.0, -2.0));
	//}
	//else//직각(직교,평행) 투영
		projection = glm::ortho(22.0f, -22.0f, -22.0f, 22.0f, 0.1f, 200.0f);

	//MyCamera[0].Dir = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);//카메라 보는 방향
	view = glm::lookAt(glm::vec3(myCamera[1].Pos), glm::vec3(myCamera[1].Dir), glm::vec3(myCamera[1].Up));

	viewLocation = glGetUniformLocation(s_program[0], "viewTransform"); //--- 뷰잉 변환 설정
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	projectionLocation = glGetUniformLocation(s_program[0], "projectionTransform"); //--- 투영 변환 값 설정
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	drawWorld();
}
void drawScene()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(s_program[0]);

	if (remocon.polygon_mode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//면
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//외곽선
	if (remocon.depth_test)
		glEnable(GL_DEPTH_TEST);//깊이테스트
	else
		glDisable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	MyViewport0();
	//MyViewport1();
	glutSwapBuffers();
}

void Reshape(int w, int h)
{
	g_window_w = w;
	g_window_h = h;
	glViewport(0, 0, w, h);
}

void KeyDown(unsigned char key, int x, int y)
{	
	switch (key) {
	case 'Q':
	case 'q':
		exit(0);
		break;
	case '1':
		remocon.first_person = true;
		break;
	case '3':
		remocon.first_person = false;
		break;
	case 'I':
	case 'i':
		if (remocon.polygon_mode == true)
			remocon.polygon_mode = false;
		else
			remocon.polygon_mode = true;
		break;
	case 'O':
	case 'o':
		if (remocon.depth_test == true)
			remocon.depth_test = false;
		else
			remocon.depth_test = true;
		break;
	case 'P':
	case 'p':
		if (remocon.perspective_projection == true)
			remocon.perspective_projection = false;
		else
			remocon.perspective_projection = true;
		break;
	case 'L':
	case 'l':
		reset();
		break;
	case '[':
		glm::mat4 CR;
		glm::mat4 CT;
		glm::mat4 CC;
		glm::mat4 CTr;
		glm::mat4 CRr;
		float Px, Py, Pz;
		float Dx, Dy, Dz;
		for (int i = 0; i < 1; i++) {
			CR = glm::mat4(1.0f);
			CT = glm::mat4(1.0f);
			CC = glm::mat4(1.0f);
			CTr = glm::mat4(1.0f);//변환행렬 초기화
			Dx = myCamera[i].Dir.x, Dy = myCamera[i].Dir.y, Dz = myCamera[i].Dir.z;
			CT = glm::translate(CT, glm::vec3(-Dx, -Dy, -Dz));
			CR = glm::rotate(CR, glm::radians(1.0f), glm::vec3(0.0, 1.0, 0.0));
			myCamera[i].Rot.y += 1.0;
			CTr = glm::translate(CTr, glm::vec3(Dx, Dy, Dz));
			CC = CTr * CR * CT;
			myCamera[i].Pos = CC * myCamera[i].Pos;
		}
		break;
	case ']':
		for (int i = 0; i < 1; i++) {
			CR = glm::mat4(1.0f);
			CT = glm::mat4(1.0f);
			CC = glm::mat4(1.0f);
			CTr = glm::mat4(1.0f);
			Dx = myCamera[i].Dir.x, Dy = myCamera[i].Dir.y, Dz = myCamera[i].Dir.z;
			CT = glm::translate(CT, glm::vec3(-Dx, -Dy, -Dz));
			CR = glm::rotate(CR, glm::radians(-1.0f), glm::vec3(0.0, 1.0, 0.0));
			myCamera[i].Rot.y -= 1.0;
			CTr = glm::translate(CTr, glm::vec3(Dx, Dy, Dz));
			CC = CTr * CR * CT;
			myCamera[i].Pos = CC * myCamera[i].Pos;
		}
		break;
	case '9':
		for (int i = 0; i < 1; i++) {
			CR = glm::mat4(1.0f);
			CT = glm::mat4(1.0f);
			CC = glm::mat4(1.0f);
			CTr = glm::mat4(1.0f);
			Px = myCamera[i].Pos.x, Py = myCamera[i].Pos.y, Pz = myCamera[i].Pos.z;
			CT = glm::translate(CT, glm::vec3(-Px, -Py, -Pz));
			CR = glm::rotate(CR, glm::radians(2.0f), glm::vec3(0.0, 1.0, 0.0));
			myCamera[i].Rot.y += 2.0;
			CTr = glm::translate(CTr, glm::vec3(Px, Py, Pz));
			CC = CTr * CR * CT;
			myCamera[i].Dir = CC * myCamera[i].Dir;
		}
		break;
	case '7':
		for (int i = 0; i < 1; i++) {
			CR = glm::mat4(1.0f);
			CT = glm::mat4(1.0f);
			CC = glm::mat4(1.0f);
			CTr = glm::mat4(1.0f);
			Px = myCamera[i].Pos.x, Py = myCamera[i].Pos.y, Pz = myCamera[i].Pos.z;
			CT = glm::translate(CT, glm::vec3(-Px, -Py, -Pz));
			CR = glm::rotate(CR, glm::radians(-2.0f), glm::vec3(0.0, 1.0, 0.0));
			myCamera[i].Rot.y -= 2.0;
			CTr = glm::translate(CTr, glm::vec3(Px, Py, Pz));
			CC = CTr * CR * CT;
			myCamera[i].Dir = CC * myCamera[i].Dir;
		}
		break;
	case '8':
		glm::mat4 CT1;
		for (int i = 0; i < 1; i++) {
			/*MyCamera[i].Pos.z += 0.1;
			MyCamera[i].Dir.z += 0.1;*/
			CT1 = glm::mat4(1.0f);
			CR = glm::mat4(1.0f);
			CT = glm::mat4(1.0f);
			CC = glm::mat4(1.0f);
			CTr = glm::mat4(1.0f);
			CRr = glm::mat4(1.0f);
			Px = myCamera[i].Pos.x, Py = myCamera[i].Pos.y, Pz = myCamera[i].Pos.z;
			CT = glm::translate(CT, glm::vec3(-Px, -Py, -Pz));
			CR = glm::rotate(CR, glm::radians(-myCamera[i].Rot.y), glm::vec3(0.0, 1.0, 0.0));
			CT1 = glm::translate(CT1, glm::vec3(0.0f, 0.0f, 0.5f));
			CRr = glm::rotate(CRr, glm::radians(myCamera[i].Rot.y), glm::vec3(0.0, 1.0, 0.0));
			CTr = glm::translate(CTr, glm::vec3(Px, Py, Pz));
			CC = CTr * CRr * CT1 * CR * CT;
			myCamera[i].Dir = CC * myCamera[i].Dir;
			myCamera[i].Pos = CC * myCamera[i].Pos;
		}
		break;
	case '5':
		for (int i = 0; i < 1; i++) {
			CT1 = glm::mat4(1.0f);
			CR = glm::mat4(1.0f);
			CT = glm::mat4(1.0f);
			CC = glm::mat4(1.0f);
			CTr = glm::mat4(1.0f);
			CRr = glm::mat4(1.0f);
			Px = myCamera[i].Pos.x, Py = myCamera[i].Pos.y, Pz = myCamera[i].Pos.z;
			CT = glm::translate(CT, glm::vec3(-Px, -Py, -Pz));
			CR = glm::rotate(CR, glm::radians(-myCamera[i].Rot.y), glm::vec3(0.0, 1.0, 0.0));
			CT1 = glm::translate(CT1, glm::vec3(0.0f, 0.0f, -0.5f));
			CRr = glm::rotate(CRr, glm::radians(myCamera[i].Rot.y), glm::vec3(0.0, 1.0, 0.0));
			CTr = glm::translate(CTr, glm::vec3(Px, Py, Pz));
			CC = CTr * CRr * CT1 * CR * CT;
			myCamera[i].Dir = CC * myCamera[i].Dir;
			myCamera[i].Pos = CC * myCamera[i].Pos;
		}
		break;
	case '4':
		for (int i = 0; i < 1; i++) {
			CT1 = glm::mat4(1.0f);
			CR = glm::mat4(1.0f);
			CT = glm::mat4(1.0f);
			CC = glm::mat4(1.0f);
			CTr = glm::mat4(1.0f);
			CRr = glm::mat4(1.0f);
			Px = myCamera[i].Pos.x, Py = myCamera[i].Pos.y, Pz = myCamera[i].Pos.z;
			CT = glm::translate(CT, glm::vec3(-Px, -Py, -Pz));
			CR = glm::rotate(CR, glm::radians(-myCamera[i].Rot.y), glm::vec3(0.0, 1.0, 0.0));
			CT1 = glm::translate(CT1, glm::vec3(-0.5f, 0.0f, 0.0f));
			CRr = glm::rotate(CRr, glm::radians(myCamera[i].Rot.y), glm::vec3(0.0, 1.0, 0.0));
			CTr = glm::translate(CTr, glm::vec3(Px, Py, Pz));
			CC = CTr * CRr * CT1 * CR * CT;
			myCamera[i].Dir = CC * myCamera[i].Dir;
			myCamera[i].Pos = CC * myCamera[i].Pos;
		}
		break;
	case '6':
		for (int i = 0; i < 1; i++) {
			CT1 = glm::mat4(1.0f);
			CR = glm::mat4(1.0f);
			CT = glm::mat4(1.0f);
			CC = glm::mat4(1.0f);
			CTr = glm::mat4(1.0f);
			CRr = glm::mat4(1.0f);
			Px = myCamera[i].Pos.x, Py = myCamera[i].Pos.y, Pz = myCamera[i].Pos.z;
			CT = glm::translate(CT, glm::vec3(-Px, -Py, -Pz));
			CR = glm::rotate(CR, glm::radians(-myCamera[i].Rot.y), glm::vec3(0.0, 1.0, 0.0));
			CT1 = glm::translate(CT1, glm::vec3(0.5f, 0.0f, 0.0f));
			CRr = glm::rotate(CRr, glm::radians(myCamera[i].Rot.y), glm::vec3(0.0, 1.0, 0.0));
			CTr = glm::translate(CTr, glm::vec3(Px, Py, Pz));
			CC = CTr * CRr * CT1 * CR * CT;
			myCamera[i].Dir = CC * myCamera[i].Dir;
			myCamera[i].Pos = CC * myCamera[i].Pos;
		}
		break;
	}
	//myRobot.Keyboard(key);
	switch (key) {
	case 'W':
	case 'w':
		userInput.W_Down = true;
		break;
	case 'A':
	case 'a':
		userInput.A_Down = true;
		break;
	case 'S':
	case 's':
		userInput.S_Down = true;
		break;
	case 'D':
	case 'd':
		userInput.D_Down = true;
		break;
	case 'Z':
	case 'z':
		myRobot.Rot.y--;
		break;
	case 'C':
	case 'c':
		myRobot.Rot.y++;
		break;
	case ' ':
		userInput.Space_Down = true;
		break;
	}
	glutPostRedisplay();
}
void KeyUp(unsigned char key, int x, int y) {
	switch (key) {
	case 'W':
	case 'w':
		userInput.W_Down = false;
		break;
	case 'A':
	case 'a':
		userInput.A_Down = false;
		break;
	case 'S':
	case 's':
		userInput.S_Down = false;
		break;
	case 'D':
	case 'd':
		userInput.D_Down = false;
		break;
	case ' ':
		userInput.Space_Down = false;
		break;
	}
	glutPostRedisplay();
}
void Mouse(int button, int state, int x, int y) {
	userInput.cursor_x = x;
	userInput.cursor_y = y;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		userInput.left_down = true;
		userInput.cursor_x1 = userInput.cursor_x;
		userInput.cursor_y1 = userInput.cursor_y;
		myRobot.rot_face_h = myRobot.Rot.y;
		myRobot.rot_face_w = myRobot.Camera[0].Rot.x;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		userInput.left_down = false;
	}
}

void MousePassiveDrag(int x, int y) {

}
void MouseDrag(int x, int y) {
	if (userInput.left_down) {
		userInput.cursor_x2 = x;
		userInput.cursor_y2 = y;
		myRobot.Rot.y = myRobot.rot_face_h + (userInput.cursor_x2 - userInput.cursor_x1) * 0.2;
		myRobot.Camera[0].Rot.x = myRobot.rot_face_w + (userInput.cursor_y2 - userInput.cursor_y1) * 0.2;
		if (myRobot.Camera[0].Rot.x > 85)
			myRobot.Camera[0].Rot.x = 85;
		if (myRobot.Camera[0].Rot.x < -85)
			myRobot.Camera[0].Rot.x = -85;
		glutPostRedisplay();
	}
}

void anim_play() {
	
}

void map_move() {

}

void FPS100(int value) {
	anim_play();
	
	myRobot.update(gravity_force);

	if (myRobot.Pos.y < 0) {//바닥면
		myRobot.Pos.y = 0.0;
		myRobot.fall_speed = 0.00;
	}
	glutPostRedisplay();
	glutTimerFunc(10, FPS100, 1);
}

	int input_x, input_y;
void enter() {
	srand((unsigned int)time(NULL));
	double max = 32767;
	myRobot.init();

	myCamera[0].Pos = glm::vec4(0.0f, 80.0f, -40.0f, 1.0f); //--- 카메라 위치
	myCamera[0].Dir = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); //--- 카메라가 바라보는 점
	myCamera[0].Rot = glm::vec3(0.0f, 0.0f, 0.0f); //--- 회전 각도

	remote_control remocon_sample;
	remocon = remocon_sample;
	MyMode Anim_sample;
	Anim = Anim_sample;
}

void reset() {
	double max = 32767;
	myRobot.reset();
	myCamera[0].Pos = glm::vec4(0.0f, 80.0f, -40.0f, 1.0f); //--- 카메라 위치
	myCamera[0].Dir = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); //--- 카메라가 바라보는 점
	myCamera[0].Rot = glm::vec3(0.0f, 0.0f, 0.0f); //--- 회전 각도
	
	remote_control remocon_sample;
	remocon = remocon_sample;
	MyMode Anim_sample;
	Anim = Anim_sample;
}


float* cheak_collision(float x, float y, float z, Object Box) {
	float vec[3] = { 0.0, 0.0, 0.0 };
	//printf("%f %f %f %f %f %f\n", Box.left(), Box.right(), Box.back(), Box.front(), Box.top(), Box.bottom());
	if (x + 0.1 > Box.left() && x - 0.1 < Box.right() && z + 0.1 > Box.back() && z - 0.1 < Box.front() && y >= Box.bottom() && y < Box.top()) {
		//printf("collision!!\n");
		float left = Box.left() - x - 0.1;//전부 음수
		float right = x - 0.1 - Box.right();
		//float bottom = Box.bottom() - y;
		float top = y - Box.top();
		float back = Box.back() - z - 0.1;
		float front = z - 0.1 - Box.front();
		float max = left;//가장 조금 밀리는 값 찾기
		if (max < right)
			max = right;
		if (max < top)
			max = top;
		if (max < front)
			max = front;
		if (max < back)
			max = back;
		//if (max < bottom)
		//	max = bottom;
		//printf("%f %f %f %f %f %f %f\n", left, right, front, back, top, bottom, max);

		if (max == right) {
			vec[0] = -(right - 0);
		}
		else if (max == left) {
			vec[0] = (left - 0);
		}
		else if (max == front) {
			vec[2] = -(front - 0);
		}
		else if (max == back) {
			vec[2] = (back - 0);
		}
		else if (max == top) {
			vec[1] = -(top - 0);
		}
		//else {
		//	vec[1] = bottom;
		//}
		//printf("%f\t%f\n", x, y);
		//printf("%f\t %f\t %f\t %f\n", Rect.left, Rect.right, Rect.top, Rect.bot);
		//printf("%f\t %f\t %f\t %f\n", left, right, top, bottom);
	}
	return vec;
}


bool check_collision_min_move(MyRobot* unit1, Object* unit2) { // unit1이 움직인놈
	float left = unit1->left() - unit2->right();
	if (left > 0)
		return false;
	float max = left;
	float right = unit2->left() - unit1->right();
	if (right > 0)
		return false;
	if (max < right)
		max = right;

	float back = unit1->back() - unit2->front();
	if (back > 0)
		return false;
	if (max < back)
		max = back;
	float front = unit2->back() - unit1->front();
	if (front > 0)
		return false;
	if (max < front)
		max = front;

	float bottom = unit1->bottom() - unit2->top();
	if (bottom > 0)
		return false;
	if (max < bottom)
		max = bottom;
	float top = unit2->bottom() - unit1->top();
	if (top > 0)
		return false;
	if (max < top)
		max = top;
	////printf("%f %f %f %f %f %f %f\n", left, right, front, back, top, bottom, max);
	// top, bottom, right, left가 모두 음수이며 최대값인 놈으로 밀어냄, (가장 조금만 밀어도 되는 쪽으로 밀기 위함)
	if (max == right)
		unit1->Pos.x += right;
	else if (max == left)
		unit1->Pos.x -= left;
	else if (max == front)
		unit1->Pos.z += front;
	else if (max == back)
		unit1->Pos.z -= back;
	else if (max == top) {
		unit1->Pos.y += top;
	}
	else {
		unit1->Pos.y -= bottom;
		myRobot.fall_speed = 0.08;
	}
	return true;
}