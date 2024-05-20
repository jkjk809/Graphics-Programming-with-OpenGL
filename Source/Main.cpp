#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <chrono>

#define STB_IMAGE_IMPLEMENTATION

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "graphics/Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "io/Mouse.h"
#include "io/Keyboard.h"
#include "io/Screen.h"
#include "io/Camera.h"
#include "graphics/Texture.h"

#include "graphics/models/Cube.hpp"
#include "graphics/Model.h"

void processInput();

auto currentTime = std::chrono::steady_clock::now();
auto lastToggleTime = std::chrono::steady_clock::now();

float deltaTime = 0.0f;	
float lastFrame = 0.0f; 

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
Screen screen;
Mouse mouse(&camera);
bool setLighting = false;
bool death = false;
bool wireframe = false;

glm::vec3 lightPos(-2.0f, 2.0f, 10.0f);

int main()
{
	stbi_set_flip_vertically_on_load(true);
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

	if (!screen.init())
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	screen.setParameters();
	

	Shader objectShader("Recources\\shader.vert", "Recources\\shader.frag");
	Shader lightShader("Recources\\shader.vert", "Recources\\light.frag");
	Shader pixelationShader("Recources\\pixelation.vert", "Recources\\pixelation.frag");

	Model m("models/house/whiteHouse.obj",glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(2.0f));
	Model e("models/house/whiteHouse.obj",glm::vec3(1.4f, 0.0f, -2.0f), glm::vec3(0.05f));
	Model l("models/terrain/terrain.obj",glm::vec3(-1.4f, 0.0f, -2.0f), glm::vec3(2.0f));

	m.setShader(&objectShader);
	l.setShader(&objectShader);

	Cube lightCube(lightPos, glm::vec3(1.0f));
	lightCube.init();

	float ground[] = {
		// Positions          // Texture Coords // normals
		-10.0f,  -0.5f, -10.0f,    0.0f, 10.0f,  0.0f, 1.0f, 0.0f,
		-10.0f, -0.5f, 10.0f,     0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
	     10.0f, -0.5f, 10.0f,    10.0f, 0.0f,    0.0f, 1.0f, 0.0f,

		-10.0f,  -0.5f, -10.0f,    0.0f, 10.0f,    0.0f, 1.0f, 0.0f,
	    10.0f, -0.5f, -10.0f,    10.0f, 10.0f,    0.0f, 1.0f, 0.0f,
	     10.0f,  -0.5f, 10.0f,    10.0f, 0.0f,    0.0f, 1.0f, 0.0f
	};

	
	// creation of ground
	unsigned int groundVAO, groundVBO;
	glGenVertexArrays(1, &groundVAO);
	glGenBuffers(1, &groundVBO);

	glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ground), ground, GL_STATIC_DRAW);
	glBindVertexArray(groundVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), static_cast<float>(screen.SCR_WIDTH) / static_cast<float> (screen.SCR_HEIGHT), 0.1f, 100.0f);

	lightShader.use();
	lightShader.setMat4("projection", projection);

	objectShader.use();
	objectShader.setMat4("projection", projection);

	objectShader.setVec3("light.ambient", 0.07f, 0.07f, 0.07f);
	objectShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f); 
	objectShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

	objectShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
	objectShader.setFloat("material.shininess", 32.0f);

	objectShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	objectShader.setVec3("lightColor", 0.5f, 0.0f, 0.0f);
	objectShader.setVec3("fogColor", .631f, 0.553f, 0.66f);
	objectShader.setVec3("light.direction", 0.05f, -1.0f, -2.0f);


	while (!screen.shouldClose())
	{

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput();
		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, screen.SCR_WIDTH, screen.SCR_HEIGHT);
		screen.update();



		objectShader.use();
		if (death)
		{
			screen.setClearColor(0.341f, 0.024f, 0.024f, 1.0f);
			objectShader.setVec3("fogColor", 0.341f, 0.024f, 0.024f);
		}
		else
		{
			screen.setClearColor(.631f, 0.553f, 0.66f, 1.0f);
			objectShader.setVec3("fogColor", .631f, 0.553f, 0.66f);
		}

		//	screen.setClearColor(0.98f, 0.839f, 0.647f, 1.0f);
		//	objectShader.setVec3("fogColor", 0.271f, 0.329f, 0.235f);


		glm::mat4 view = camera.GetViewMatrix();
		objectShader.setBool("enableSpecular", true);
		objectShader.setBool("lighting", setLighting);
		objectShader.setVec3("viewPos", camera.Position);
		objectShader.setMat4("view", view);
		objectShader.setVec3("lightPos", lightPos);

		
		m.render();
		//e.render(objectShader);
		l.render();

	//	lightShader.use();

		//lightShader.setMat4("view", view);

		//lightCube.render(lightShader);


		//objectShader.use();



		//objectShader.setBool("enableSpecular", false);
		//glm::mat4 model = glm::mat4(1.0f);
		//objectShader.setMat4("model", model);
		//glBindVertexArray(groundVAO);
		//glActiveTexture(GL_TEXTURE0);
		//grassTexture.bind();
		//glDrawArrays(GL_TRIANGLES, 0, 6);

		screen.newFrame();
	}

	m.cleanup();
	l.cleanup();
	glDeleteVertexArrays;
	glfwTerminate();
	return 0;
	
}

void processInput()
{

	if (Keyboard::key(GLFW_KEY_ESCAPE))
		screen.setShouldClose(true);

	if (Keyboard::key(GLFW_KEY_M))
	{
		auto currentTime = std::chrono::steady_clock::now();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastToggleTime).count();
		if (elapsedTime > 200) // Cooldown period: 200 milliseconds
		{
			death = !death;
			setLighting = !setLighting;
			screen.setClearColor(0.5, 0.0, 0.0, 1.0);
			lastToggleTime = currentTime;
		}
	} 
	if (Keyboard::key(GLFW_KEY_T))
	{
		auto currentTime = std::chrono::steady_clock::now();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastToggleTime).count();
		if (elapsedTime > 200) // Cooldown period: 200 milliseconds
		{
			wireframe = !wireframe;
			lastToggleTime = currentTime;
		}
	}
	const float cameraSpeed = 1.8f * deltaTime; // adjust accordingly
	if (Keyboard::key(GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, cameraSpeed);

	if (Keyboard::key(GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, cameraSpeed);

	if (Keyboard::key(GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, cameraSpeed);

	if (Keyboard::key(GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, cameraSpeed);
	if(false)
	camera.Position.y = 1.0f;
}
