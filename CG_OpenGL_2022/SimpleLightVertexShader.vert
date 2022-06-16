#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

out vec2 UV;								// �ؽ�ó ��ǥ
out vec3 Position_worldspace;				// ���� ��ǥ�迡���� ��ġ
out vec3 Normal_worldspace;					// ���� ��ǥ�迡���� �븻
out vec3 EyeDirection_worldspace;		    // ���� ��ǥ�迡���� ���ؽ�->�� ����
out vec3 LightDirection_worldspace;		    // ���� ��ǥ�迡���� ���ؽ�->�� ����

// Values that stay constant for the whole mesh.
uniform mat4 MVP;							// MVP ���
uniform mat4 M;								// Model ���. ���� ���� ��ǥ��� �Ű��ش�.
uniform vec3 CameraPosition_worldspace;		// ���� ��ǥ�迡���� ī�޶� ��ġ
uniform vec3 LightPosition_worldspace;		// ���� ��ǥ�迡���� ���� ��ġ

void main(){

	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
	
	UV = vertexUV;

	// ���ؽ��� ���� ��ǥ�� �� ��ġ ���
	Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;

	// �븻�� ���� ��ǥ�� �� ���� ���
	Normal_worldspace = ( M * vec4(vertexNormal_modelspace,0)).xyz;
	
	// ���ؽ� -> �� ������ ���͸� ���
	EyeDirection_worldspace = CameraPosition_worldspace - Position_worldspace;

	// ���ؽ� -> �� ������ ���͸� ���
	LightDirection_worldspace = vec4(LightPosition_worldspace,1).xyz - Position_worldspace;

}

