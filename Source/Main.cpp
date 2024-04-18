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

unsigned int loadTexture(char const* path);
void processInput();

auto currentTime = std::chrono::steady_clock::now();
auto lastToggleTime = std::chrono::steady_clock::now();

float deltaTime = 0.0f;	
float lastFrame = 0.0f; 

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
Screen screen;
Mouse mouse(&camera);

bool setLighting = true;

glm::vec3 lightPos(-2.2f, 5.0f, -20.0f);

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

	Cube cube(glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f));
	cube.init();
	Cube lightCube(lightPos, glm::vec3(0.1f));
	lightCube.init();
	//Verticers for our cube.


	float rectangleVertices[] =
	{
		// Coords    // texCoords
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,

		 1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};

	float ground[] = {
		// Positions          // Texture Coords // normals
		-10.0f,  -0.5f, -10.0f,    0.0f, 10.0f,  0.0f, 1.0f, 0.0f,
		-10.0f, -0.5f, 10.0f,     0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
	     10.0f, -0.5f, 10.0f,    10.0f, 0.0f,    0.0f, 1.0f, 0.0f,

		-10.0f,  -0.5f, -10.0f,    0.0f, 10.0f,    0.0f, 1.0f, 0.0f,
	    10.0f, -0.5f, -10.0f,    10.0f, 10.0f,    0.0f, 1.0f, 0.0f,
	     10.0f,  -0.5f, 10.0f,    10.0f, 0.0f,    0.0f, 1.0f, 0.0f
	};

	unsigned int rectVAO, rectVBO;
	glGenVertexArrays(1, &rectVAO);
	glGenBuffers(1, &rectVBO);

	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

	glBindVertexArray(rectVAO);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	
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
	projection = glm::perspective(glm::radians(70.0f), static_cast<float>(screen.SCR_WIDTH) / static_cast<float> (screen.SCR_HEIGHT), 0.1f, 100.0f);

	lightShader.use();
	lightShader.setMat4("projection", projection);

	objectShader.use();
	objectShader.setMat4("projection", projection);

	objectShader.setVec3("light.ambient", 0.07f, 0.07f, 0.07f);
	objectShader.setVec3("light.diffuse", 1.5f, 1.5f, 1.5f); // darken diffuse light a bit
	objectShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

	objectShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
	objectShader.setFloat("material.shininess", 32.0f);

	objectShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	objectShader.setVec3("lightColor", 0.5f, 0.0f, 0.0f);


	Texture grassTexture("Recources/textures/grass.jpg", "grassTexture");
	grassTexture.load();

	pixelationShader.use();
	pixelationShader.setInt("screenTexture", 0);
	
	GLuint framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	
	unsigned int textureColorBuffer;
	glGenTextures(1, &textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen.SCR_WIDTH, screen.SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screen.SCR_WIDTH, screen.SCR_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureColorBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	
	while (!screen.shouldClose())
	{
		
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput();
		glEnable(GL_DEPTH_TEST);

		glViewport(0, 0, screen.SCR_WIDTH, screen.SCR_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		screen.update();
		
		
		objectShader.use();
		glm::mat4 view = camera.GetViewMatrix();
		objectShader.setBool("enableSpecular", true);
		objectShader.setBool("lighting", setLighting);
		objectShader.setVec3("viewPos", camera.Position);
		objectShader.setMat4("view", view);
		//cube drawing
		lightCube.pos.x = 1.0f + sin(glfwGetTime()) * 10.0f;
		glm::vec3 scaledLightPos = lightCube.pos * lightCube.size;
		objectShader.setVec3("lightPos", scaledLightPos);

		cube.render(objectShader);
		
		//lightPos.x = 1.0f + sin(static_cast<float> (glfwGetTime())) * 2.0f;
		//lightCube.pos.x = lightPos.x;
		
		//objectShader.setVec3("lightPos", lightCube.pos);
		
		
		lightShader.use();
		lightShader.setMat4("view", view);
		
		
		lightCube.render(lightShader);
		
		objectShader.use();

		objectShader.setBool("enableSpecular", false);
		glm::mat4 model = glm::mat4(1.0f);
		objectShader.setMat4("model", model);
		glBindVertexArray(groundVAO);
		glActiveTexture(GL_TEXTURE0);
		grassTexture.bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		glClearColor(0.6f, 0.5f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		pixelationShader.use();
		
		glBindVertexArray(rectVAO);
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		screen.newFrame();
	}
	glDeleteFramebuffers(1, &framebuffer);
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
			setLighting = !setLighting;
			lastToggleTime = currentTime;
		}
	}
	const float cameraSpeed = 0.8f * deltaTime; // adjust accordingly
	if (Keyboard::key(GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, cameraSpeed);

	if (Keyboard::key(GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, cameraSpeed);

	if (Keyboard::key(GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, cameraSpeed);

	if (Keyboard::key(GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, cameraSpeed);
	if(true)
	camera.Position.y = 0.5f;
}
