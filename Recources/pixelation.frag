#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D screenTexture; // The texture containing the scene


void main()
{
	float Pixels = 2560.0;
        float dx = 10.0 * (1.0 / Pixels);
        float dy = 5.0 * (1.0 / Pixels);
        vec2 Coord = vec2(dx * floor(TexCoord.x / dx),
                          dy * floor(TexCoord.y / dy));
        FragColor = texture(screenTexture, Coord);
}