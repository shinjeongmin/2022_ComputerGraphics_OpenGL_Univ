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
	vec3 MaterialDiffuseColor = texture( myTextureSampler, UV ).rgb;		// 분산광
	vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;   // 주변광
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);							// 정반사광

	float distance = length( LightDirection_worldspace );					// 거리에 의한 감쇠를 위해 사용

	// ambient 부분은 계산 없이 그대로 ambient color를 사용

	// diffuse 부분 계산
	vec3 n = normalize( Normal_worldspace );
	vec3 l = normalize( LightDirection_worldspace );
	float cosTheta = clamp( dot( n,l ), 0,1 );
	
	// specular 부분 계산
	vec3 V = normalize(EyeDirection_worldspace);
	vec3 R = reflect(-l,n);
	float cosAlpha = clamp( dot( V,R ), 0,1 );
	
	// 최종 계산
	color = 
		MaterialAmbientColor +
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);

}