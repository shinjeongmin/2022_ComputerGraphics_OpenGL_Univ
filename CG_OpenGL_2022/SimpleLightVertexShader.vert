#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

out vec2 UV;								// 텍스처 좌표
out vec3 Position_worldspace;				// 월드 좌표계에서의 위치
out vec3 Normal_worldspace;					// 월드 좌표계에서의 노말
out vec3 EyeDirection_worldspace;		    // 월드 좌표계에서의 버텍스->눈 방향
out vec3 LightDirection_worldspace;		    // 월드 좌표계에서의 버텍스->빛 방향

// Values that stay constant for the whole mesh.
uniform mat4 MVP;							// MVP 행렬
uniform mat4 M;								// Model 행렬. 모델을 월드 좌표계로 옮겨준다.
uniform vec3 CameraPosition_worldspace;		// 월드 좌표계에서의 카메라 위치
uniform vec3 LightPosition_worldspace;		// 월드 좌표계에서의 빛의 위치

void main(){

	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
	
	UV = vertexUV;

	// 버텍스의 월드 좌표계 상 위치 계산
	Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;

	// 노말의 월드 좌표계 상 값을 계산
	Normal_worldspace = ( M * vec4(vertexNormal_modelspace,0)).xyz;
	
	// 버텍스 -> 눈 방향의 벡터를 계산
	EyeDirection_worldspace = CameraPosition_worldspace - Position_worldspace;

	// 버텍스 -> 빛 방향의 벡터를 계산
	LightDirection_worldspace = vec4(LightPosition_worldspace,1).xyz - Position_worldspace;

}

