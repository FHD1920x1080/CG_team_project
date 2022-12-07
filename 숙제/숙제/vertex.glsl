#version 330 core

layout (location = 0) in vec3 vPos;   // 위치 변수는 attribute position 0을 가집니다.
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 in_Color;

uniform mat4 modelTransform; //--- 모델링 변환 행렬: uniform 변수로 선언

uniform mat4 viewTransform; //--- 카메라 변환 행렬: uniform 변수로 선언
uniform mat4 projectionTransform;
out vec3 out_Color;

void main()
{
	gl_Position = projectionTransform * viewTransform * modelTransform * vec4(vPos, 1.0); //--- 좌표값에 modelTransform 변환을 적용한다.
	out_Color = in_Color;
}  
