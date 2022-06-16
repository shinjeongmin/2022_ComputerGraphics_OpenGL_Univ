#version 450 core

in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_worldspace;
in vec3 EyeDirection_worldspace;
in vec3 LightDirection_worldspace;

out vec3 color;

uniform sampler2D myTextureSampler;
uniform vec3 LightPosition_worldspace;

void main(){

	vec3 LightColor = vec3(1,1,1);
	float LightPower = 50.0f;
	
	// Material properties
	vec3 MaterialDiffuseColor = texture( myTextureSampler, UV ).rgb;		// �л걤
	vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;   // �ֺ���
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);							// ���ݻ籤

	float distance = length( LightDirection_worldspace );					// �Ÿ��� ���� ���踦 ���� ���

	// ambient �κ��� ��� ���� �״�� ambient color�� ���

	// diffuse �κ� ���
	vec3 n = normalize( Normal_worldspace );
	vec3 l = normalize( LightDirection_worldspace );
	float cosTheta = clamp( dot( n,l ), 0,1 );
	
	// specular �κ� ���
	vec3 V = normalize(EyeDirection_worldspace);
	vec3 R = reflect(-l,n);
	float cosAlpha = clamp( dot( V,R ), 0,1 );
	
	// ���� ���
	color = 
		MaterialAmbientColor +
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);

}