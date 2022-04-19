#pragma once


//basic includes
#include <vector>
#include <stb_image.h> 

// shader header file
#include "shader.h"

// opengl headers
#include <GL/glew.h>
#include <GLFW/glfw3.h>



//texture class
class Texture {

private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;

public:
	Texture(const std::string& path);
	~Texture();
	// texture bind
	void Bind(unsigned int slot = 0) const;
	// texture unbind
	void Unbind() const;

	inline int GetWidth() const { return m_Width;}
	inline int GetHeight() const { return m_Height;}
};