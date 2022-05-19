#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <iostream>

#define GLEW_STATIC
#include <GL\glew.h>

#include <GLFW\/glfw3.h>
GLFWwindow* window;

#include <GLM\glm.hpp>
#include <GLM\gtc\matrix_transform.hpp>

#include "shader.h"

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
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	// 깊이 테스트
	glEnable(GL_DEPTH_TEST);
	// 앞에 있는 오브젝트가 화면에 그려진다
	glDepthFunc(GL_LESS);
	// 카메라를 바라보지 않는 면은 그려지지 않는다
	glEnable(GL_CULL_FACE);
	// 커서를 숨긴다
	ShowCursor(false);


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);

	GLuint ProgramID = LoadShaders("Week2VertexShader.vert", "Week2FragmentShader.frag");

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// 큐브의 버텍스 위치
	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// 큐브의 버텍스 색상
	static const GLfloat g_vertex_color_buffer_data[] = {
		0.583f,  0.771f,  0.014f,
		0.609f,  0.115f,  0.436f,
		0.327f,  0.483f,  0.844f,
		0.822f,  0.569f,  0.201f,
		0.435f,  0.602f,  0.223f,
		0.310f,  0.747f,  0.185f,
		0.597f,  0.770f,  0.761f,
		0.559f,  0.436f,  0.730f,
		0.359f,  0.583f,  0.152f,
		0.483f,  0.596f,  0.789f,
		0.559f,  0.861f,  0.639f,
		0.195f,  0.548f,  0.859f,
		0.014f,  0.184f,  0.576f,
		0.771f,  0.328f,  0.970f,
		0.406f,  0.615f,  0.116f,
		0.676f,  0.977f,  0.133f,
		0.971f,  0.572f,  0.833f,
		0.140f,  0.616f,  0.489f,
		0.997f,  0.513f,  0.064f,
		0.945f,  0.719f,  0.592f,
		0.543f,  0.021f,  0.978f,
		0.279f,  0.317f,  0.505f,
		0.167f,  0.620f,  0.077f,
		0.347f,  0.857f,  0.137f,
		0.055f,  0.953f,  0.042f,
		0.714f,  0.505f,  0.345f,
		0.783f,  0.290f,  0.734f,
		0.722f,  0.645f,  0.174f,
		0.302f,  0.455f,  0.848f,
		0.225f,  0.587f,  0.040f,
		0.517f,  0.713f,  0.338f,
		0.053f,  0.959f,  0.120f,
		0.393f,  0.621f,  0.362f,
		0.673f,  0.211f,  0.457f,
		0.820f,  0.883f,  0.371f,
		0.982f,  0.099f,  0.879f
	};

	GLuint vertexColorbuffer;
	glGenBuffers(1, &vertexColorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexColorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_color_buffer_data), g_vertex_color_buffer_data, GL_STATIC_DRAW);

	GLuint MatrixID = glGetUniformLocation(ProgramID, "MVP");

	glm::mat4 Projection = glm::perspective(
		glm::radians(45.0f),
		4.0f / 3.0f,
		0.1f,
		100.0f
	);
	/*
	glm::mat4 View = glm::lookAt(
		glm::vec3(4, 3, 3),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);
	*/

	glm::mat4 View = glm::lookAt(
		glm::vec3(4, 3, 3),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);

	glm::vec3 CamPosition = glm::vec3(4, 3, 3);

	// 이동, 회전, 크기 변환 행렬의 생성 및 초기화
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 Model2 = glm::mat4(1.0f);

	glm::mat4 Translation = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	glm::mat4 Rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
	glm::mat4 Scaling = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));

	glm::mat4 Translation2 = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 2));
	glm::mat4 Rotation2 = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 1, 0));
	glm::mat4 Scaling2 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	// 마우스 입력 관련 초기화
	float lastTime = glfwGetTime();
	float currentTime = lastTime;
	glfwPollEvents();
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// 마우스 입력 2 관련
	float horizontalAngle = 0.0f;
	float VerticalAngle = 0.0f;
	float mouseSpeed = 0.01f;
	float CamMoveSpeed = 1.0f;
	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(ProgramID);

		// 키보드 입력
		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		{
			Rotation = glm::rotate(Rotation, glm::radians(0.1f), glm::vec3(0, 1, 0));
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			Rotation = glm::rotate(Rotation, glm::radians(-0.1f), glm::vec3(0, 1, 0));
		}
		//

		// 마우스 입력 2

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

		// 카메라 업 벡터 정의 1
		/*
		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 right = glm::cross(direction, up);
		*/

		// 카메라 업 벡터 정의 2
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


		// 이동, 회전, 크기 변환의 렌더링 루프
		//Model = Translation;
		//Model = Rotation;
		//Model = Scaling;

		Model = Translation * Rotation * Scaling;

		//

		glm::mat4 MVP = Projection * View * Model;

		glUniformMatrix4fv(MatrixID,
			1,
			GL_FALSE,
			&MVP[0][0]
		);

		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glBindBuffer(GL_ARRAY_BUFFER, vertexColorbuffer);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, 3 * 12);

		// 오브젝트를 여러 개 그리는 방법

		// 마우스 입력 1
		/*
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		Rotation2 = glm::rotate(Rotation2, (float)glm::radians(xpos - 1024 / 2), glm::vec3(0, 1, 0));
		glfwSetCursorPos(window, 1024 / 2, 768 / 2);
		*/

		Model2 = Translation2 * Rotation2 * Scaling2;
		glm::mat4 MVP2 = Projection * View * Model2;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP2[0][0]); // 유니폼 변수의 데이터를 새로 넣어준다.
		glDrawArrays(GL_TRIANGLES, 0, 3 * 12);
		//

		// 마우스 입력 2
		lastTime = glfwGetTime();
		//

		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glfwTerminate();

	return 0;
}