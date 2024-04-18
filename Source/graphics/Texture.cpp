#include "Texture.h"
#include <iostream>

int Texture::currentId = 0;

Texture::Texture() {};


Texture::Texture(const char* path, const char* name, bool defaultParams)
	: path(path), name(name), id(currentId++) {
	generate();

	if (defaultParams) {
		setFilters(GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST);
		setWrap(GL_REPEAT);
	}
}
void Texture::generate()
{
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
}

void Texture::load(bool flip)
{
	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(path, &width, &height, &nChannels, 0);

	if (data)
	{
		GLenum format;
		if (nChannels == 1)
			format = GL_RED;
		else if (nChannels == 3)
			format = GL_RGB;
		else if (nChannels == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
	}
	stbi_image_free(data);
}

void Texture::setFilters(GLenum all)
{
	setFilters(all, all);
}

void Texture::setFilters(GLenum mag, GLenum min)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
}

void Texture::setWrap(GLenum all)
{
	setWrap(all, all);
}

void Texture::setWrap(GLenum s, GLenum t)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t);
}

void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, id);
}