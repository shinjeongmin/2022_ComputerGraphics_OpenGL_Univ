#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#define GLEW_STATIC
#include <GL\glew.h>

#include <GLFW\/glfw3.h>
GLFWwindow* window;

#include <GLM\glm.hpp>
#include <GLM\gtc\matrix_transform.hpp>

#include "shader.h"


// .obj 파일에서 버텍스 위치, 텍스처 좌표, 노멀 벡터 데이터를 가져오는 함수
bool loadOBJ(
	const char* path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
)
{
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	FILE* file;
	fopen_s(&file, path, "r");
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf_s(file, "%s", lineHeader, sizeof(lineHeader));
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				fclose(file);
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}
	fclose(file);
	return true;
}
// 함수 정의 끝

int main()
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1024, 768, "Playground", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);


	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// 버텍스 데이터(위치, 텍스처 좌표, 노멀 벡터) 객체를 생성합니다.
	std::vector<glm::vec3> cube_vertex_position_array;
	std::vector<glm::vec2> cube_vertex_uv_array;
	std::vector<glm::vec3> cube_vertex_normal_array;
	// 생성 끝

	// .obj 파일에서 필요한 데이터를 불러오는 함수를 호출합니다.
	loadOBJ("cube.obj", cube_vertex_position_array, cube_vertex_uv_array, cube_vertex_normal_array);
	// 호출 완료

	static const GLfloat g_vertex_buffer_data2[] = {
	-1.0f, -1.0f, 0.0f,
	 1.0f, -1.0f, 0.0f,
	 0.0f, 1.0f, 0.0f
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// std::vector 형태에 맞게, 파라미터를 조정해 줍시다.
	glBufferData(GL_ARRAY_BUFFER, cube_vertex_position_array.size() * sizeof(glm::vec3), &cube_vertex_position_array[0], GL_STATIC_DRAW);
	// 조정 완료

	// 텍스처를 아직 사용하지 않으므로, 색상 데이터를 랜덤으로 생성해 줍시다.
	std::vector<glm::vec3> cube_vertex_color_data;
	std::random_device rd;
	std::mt19937_64 mt(rd());
	std::uniform_real_distribution<float> range(0.0f, 1.0f);
	for (int i = 0; i < cube_vertex_position_array.size(); i++)
	{
		cube_vertex_color_data.push_back(glm::vec3(range(mt), range(mt), range(mt)));
	}
	// 색상 데이터 랜덤 생성 완료


	GLuint vertexColorBuffer;
	glGenBuffers(1, &vertexColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexColorBuffer);
	// std::vector 형태에 맞게 파라미터를 조정해 줍시다.
	glBufferData(GL_ARRAY_BUFFER, cube_vertex_color_data.size() * sizeof(glm::vec3), &cube_vertex_color_data[0], GL_STATIC_DRAW);
	// 조정 완료

	GLuint ProgramID = LoadShaders("Week2VertexShader.vert",
		"Week2FragmentShader.frag");

	GLuint MatrixID = glGetUniformLocation(ProgramID, "MVP");

	glm::vec3 CamPosition = glm::vec3(4, 3, 3);
	glm::vec3 InitialDirPoint = glm::vec3(0, 0, 0);
	glm::mat4 View = glm::lookAt(CamPosition,
		InitialDirPoint,
		glm::vec3(0, 1, 0));

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f),
		1024.0f / 768.0f,
		0.1f, 1000.0f);


	glm::mat4 Translation = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 1));
	glm::mat4 Rotation = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0, 1, 0));
	glm::mat4 Scaling = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
	glm::mat4 Model = Translation * Rotation * Scaling;

	glm::mat4 MVP = Projection * View * Model;

	float lastTime = glfwGetTime();
	float currentTime = lastTime;
	glfwPollEvents();
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	glm::vec3 InitialDirection = InitialDirPoint - CamPosition;
	float horizontalAngle = atan2(InitialDirection.x, InitialDirection.z);
	float VerticalAngle = atan(InitialDirection.y / sqrt(pow(InitialDirection.x, 2) + pow(InitialDirection.z, 2)));
	if (VerticalAngle > 1.5f) VerticalAngle = 1.5f;
	if (VerticalAngle < -1.5f) VerticalAngle = -1.5f;
	float mouseSpeed = 0.01f;
	float CamMoveSpeed = 0.02f;

	glfwSetCursorPos(window, 1024 / 2, 768 / 2);
	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(ProgramID);

		currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime);

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		glfwSetCursorPos(window, 1024 / 2, 768 / 2);

		horizontalAngle += mouseSpeed * float(1024 / 2 - xpos);
		VerticalAngle += mouseSpeed * float(768 / 2 - ypos);
		if (VerticalAngle > 1.5f) VerticalAngle = 1.5f;
		if (VerticalAngle < -1.5f) VerticalAngle = -1.5f;

		glm::vec3 direction = glm::vec3(cos(VerticalAngle) * sin(horizontalAngle),
			sin(VerticalAngle),
			cos(VerticalAngle) * cos(horizontalAngle));

		glm::vec3 right = glm::vec3(sin(horizontalAngle - 3.14 / 2.0f),
			0,
			cos(horizontalAngle - 3.14 / 2.0f));
		glm::vec3 up = glm::cross(right, direction);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			CamPosition += direction * deltaTime * CamMoveSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			CamPosition += -right * deltaTime * CamMoveSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			CamPosition += -direction * deltaTime * CamMoveSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			CamPosition += right * deltaTime * CamMoveSpeed;
		}

		View = glm::lookAt(CamPosition, CamPosition + direction, up);

		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, cube_vertex_position_array.size() * sizeof(glm::vec3), &cube_vertex_position_array[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glBindBuffer(GL_ARRAY_BUFFER, vertexColorBuffer);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0,
			(void*)0);

		Model = Translation * Rotation * Scaling;
		MVP = Projection * View * Model;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 버텍스 개수를 변경해 줍니다.
		glDrawArrays(GL_TRIANGLES, 0, cube_vertex_position_array.size());
		// 변경 완료
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glfwSetCursorPos(window, 1024 / 2, 768 / 2);
		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glfwTerminate();

	return 0;
}