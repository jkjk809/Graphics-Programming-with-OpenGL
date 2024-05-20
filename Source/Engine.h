#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <chrono>

#define STB_IMAGE_IMPLEMENTATION

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "graphics/Shader.h"
#include "io/Mouse.h"
#include "io/Keyboard.h"
#include "io/Screen.h"
#include "io/Camera.h"
#include "graphics/Texture.h"

#include "graphics/models/Cube.hpp"
#include "graphics/Model.h"

const char* defaultVertexShader = R"glsl(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)glsl";

const char* defaultFragmentShader = R"glsl(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main() {
    vec3 color = texture(texture_diffuse1, TexCoords).rgb;
    vec3 ambient = 0.1 * color;
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * color;
    
    FragColor = vec4(ambient + diffuse, 1.0);
}
)glsl";

class Engine
{
public:
     Engine();
	~Engine();

	bool init();
	void run();

	void setShader(const std::string& vertexPath, const std::string& fragmentPath);
	void loadModel(const std::string& modelPath);

private:
	Screen screen;
	std::unique_ptr<Shader> shader;
	std::vector<std::unique_ptr<Model>> models;
};