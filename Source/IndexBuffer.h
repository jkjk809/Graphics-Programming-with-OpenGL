#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>

class IndexBuffer
{
private:
	unsigned int m_RendererID;
	unsigned int m_Count;

public:
	IndexBuffer(const GLuint* data,  GLuint count);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;
    
	GLuint GetCount() const { return m_Count; }
};