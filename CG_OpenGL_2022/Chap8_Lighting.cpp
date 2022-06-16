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

// BMP 파일을 텍스처 형태로 가져오는 매우 간단한 함수
GLuint loadBMP_custom(const char* imagepath)
{

	printf("Reading image %s\n", imagepath);

	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	// Actual RGB data
	unsigned char* data;

	// Open the file
	FILE* file;
	fopen_s(&file, imagepath, "rb");
	if (!file) {
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath);
		getchar();
		return 0;
	}

	// Read the header, i.e. the 54 first bytes

	// If less than 54 bytes are read, problem
	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		fclose(file);
		return 0;
	}
	// A BMP files always begins with "BM"
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		fclose(file);
		return 0;
	}
	// Make sure this is a 24bpp file
	if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    fclose(file); return 0; }
	if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    fclose(file); return 0; }

	// Read the information about the image
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)    imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	// Everything is in memory now, the file can be closed.
	fclose(file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	// OpenGL has now copied the data. Free our own version
	delete[] data;

	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

	// ... nice trilinear filtering ...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// ... which requires mipmaps. Generate them automatically.
	glGenerateMipmap(GL_TEXTURE_2D);

	// Return the ID of the texture we just created
	return textureID;
}
// 함수 끝


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

	std::vector<glm::vec3> cube_vertex_position_array;
	std::vector<glm::vec2> cube_vertex_uv_array;
	std::vector<glm::vec3> cube_vertex_normal_array;

	loadOBJ("cylinder.obj", cube_vertex_position_array, cube_vertex_uv_array, cube_vertex_normal_array);

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, cube_vertex_position_array.size() * sizeof(glm::vec3), &cube_vertex_position_array[0], GL_STATIC_DRAW);

	GLuint vertexUVBuffer;
	glGenBuffers(1, &vertexUVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, cube_vertex_uv_array.size() * sizeof(glm::vec2), &cube_vertex_uv_array[0], GL_STATIC_DRAW);

	// 1. .obj 파일에서 가져온 노말 데이터를 가지고 버텍스 버퍼를 만들어 줍니다.
	GLuint vertexNormalBuffer;
	glGenBuffers(1, &vertexNormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexNormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, cube_vertex_normal_array.size() * sizeof(glm::vec3), &cube_vertex_normal_array[0], GL_STATIC_DRAW);
	// 노말 데이터로 버텍스 버퍼 생성 끝

	GLuint ProgramID = LoadShaders("SimpleLightVertexShader.vert",
		"SimpleLightFragmentShader.frag");

	GLuint MatrixID = glGetUniformLocation(ProgramID, "MVP");
	GLuint TextureID = glGetUniformLocation(ProgramID, "TextureSampler");
	// 2. 라이팅에 사용할 추가적인 유니폼 변수들의 위치를 가져옵니다.
	GLuint ModelID = glGetUniformLocation(ProgramID, "M");
	GLuint CameraID = glGetUniformLocation(ProgramID, "CameraPosition_worldspace");
	GLuint LightID = glGetUniformLocation(ProgramID, "LightPosition_worldspace");
	// 추가적인 유니폼 변수 위치 가져오기 끝
		
	GLuint Texture = loadBMP_custom("cubeTex.bmp");

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

	// 3. 빛의 위치를 정해줍니다.
	glm::vec3 LightPosition = glm::vec3(3.0f, 3.0f, 3.0f);
	// 빛의 위치 설정 완료

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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, vertexUVBuffer);
		glBufferData(GL_ARRAY_BUFFER, cube_vertex_uv_array.size() * sizeof(glm::vec2), &cube_vertex_uv_array[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 3. 노말 데이터를 수정하고, VAO에 버텍스 노말에 대한 설정을 입력해줍니다. location은 2 입니다.
		glBindBuffer(GL_ARRAY_BUFFER, vertexNormalBuffer);
		glBufferData(GL_ARRAY_BUFFER, cube_vertex_normal_array.size() * sizeof(glm::vec3), &cube_vertex_normal_array[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		Model = Translation * Rotation * Scaling;
		MVP = Projection * View * Model;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniform1i(TextureID, 0);
		// 4. 조명에 사용할 추가 유니폼 변수들의 위치에 데이터를 넣어줍니다.
		glUniform3f(CameraID, CamPosition.x, CamPosition.y, CamPosition.z);
		glUniformMatrix4fv(ModelID, 1, GL_FALSE, &Model[0][0]);
		glUniform3f(LightID, LightPosition.x, LightPosition.y, LightPosition.z);
		// 추가 유니폼 변수들의 데이터 입력 완료			

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);

		glDrawArrays(GL_TRIANGLES, 0, cube_vertex_position_array.size());

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