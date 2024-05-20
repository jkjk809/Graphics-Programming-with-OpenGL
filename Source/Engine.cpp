#include "Engine.h"

Engine::Engine()
	: camera(glm::vec3(0.0f, 0.5f, 0.0f))
{
}

Engine::~Engine()
{
	for (const auto& model : models)
	{
		model->cleanup();
	}
	glfwTerminate();
}

bool Engine::init()
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
		return false;
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	screen.setParameters();
	// set default uniforms
	setShader("default", defaultVertexShader, defaultFragmentShader);
	defaultShader = getShader("default");

	if (defaultShader) {
		defaultShader->use();
	}
	else {
		std::cerr << "Failed to create default shader" << std::endl;
		return false;
	}

	return true;
}

void Engine::run()
{
	while (!screen.shouldClose())
	{
		screen.update();

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput();

		renderModels();

	}
}
void Engine::addModel(const std::string& modelPath, const glm::vec3& position, const glm::vec3& scale)
{
	// later allow user to change shaders through names.

	auto model = std::make_unique<Model>(modelPath, position, scale);
	model->setShader(defaultShader);
	models.push_back(std::make_unique<Model>(modelPath, position, scale));
}

void Engine::renderModels()
{
	for (const auto& model : models)
	{
		model->render();
	}
}
Shader* Engine::getShader(const std::string& name)
{
	try
	{
		return shaders.at(name).get();
	}
	catch (const std::out_of_range&)
	{
		return nullptr;
	}
}

void Engine::setShader(const std::string& name, const char* vertexPath, const char* fragmentPath)
{
	shaders[name] = std::make_unique<Shader>(vertexPath, fragmentPath);
}

void Engine::processInput()
{
	if (Keyboard::key(GLFW_KEY_ESCAPE))
		screen.setShouldClose(true);
	if (Keyboard::key(GLFW_KEY_M))
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
	if (wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}