#pragma once
//basic includes
#include <iostream>
#include <string>
#include <fstream>
#include <string>
#include <sstream>
#include<unordered_map>
#include <vector>

//opengl Includes
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// image header
#include <stb_image.h> 
//shader sources
struct shaderSources {
	std::string VertexSource;
	std::string FragmentSource;
	
};
class Shader
{
private:
	// variables
	std::string m_FilePath;
	unsigned int m_RendererID;
	std::unordered_map<std::string, int>m_UniformLocationCache;
public:

	Shader(const std::string& filepath);
	~Shader();
	// sets bind to constant
	void Bind()const;
	// sets unbind to constant
	void Unbind()const;


	//set uniforms
	// uniform for a single int value
	void SetUniform1i(const std::string& name, int value);
	//uniform for a single float value
	void SetUniform1f(const std::string& name, float value);
	//uniform for 4 float values
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v4);
	// uniform for a 4 float matrix
	void SetUniformMat4f(const std::string& name, const glm::mat4 matrix);
	
private:
	//sharder parser
	shaderSources ParseShader(const std::string(filepath));
	unsigned int CreateShader(std::string vtxSource, std::string fragSource);
	 int GetUniformLocation(const std::string& name);
};