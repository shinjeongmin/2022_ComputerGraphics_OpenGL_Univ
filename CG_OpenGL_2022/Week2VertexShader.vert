#version 450 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 Vertex_color;

out vec3 VertexLastColor;

uniform mat4 MVP;

void main(){
//	gl_Position.xyz = vertexPosition_modelspace;
//	gl_Position.w = 1.0f;

	gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0f);

	VertexLastColor = Vertex_color;
}