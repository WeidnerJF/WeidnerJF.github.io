//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pulls shader information from Basic.Shader and parses the information and populates the vertex
// and fragment shader source variables. This allows for shader information to be removed from source.cpp
// 
// //////////////////////////////////////////////////////////////////////////////////////////////////////


#include "shader.h"


//shader source parser
//shader source file basic.shader is shader program code and will not have any comments
 shaderSources Shader::ParseShader(const std::string(filepath)) {
	std::ifstream stream(filepath);
	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1

	};
    std::stringstream ss[2];
	std::string line;
	ShaderType type = ShaderType::NONE;
	// retrieve data from basic.shader and set to either fragment or vertext clasee
	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			//set vertex shader
			if (line.find("vertex") != std::string::npos) {
				type = ShaderType::VERTEX;
			}
			// set fragment shader
			else if (line.find("fragment") != std::string::npos) {
				type = ShaderType::FRAGMENT;
			}
		}
		else {
			ss[(int)type] << line << '\n';

		}
	}
	return { ss[0].str(), ss[1].str() };
}
 // create shader m_RendererID for both Vertex and Fragment source
Shader::Shader(const std::string& filepath) :m_FilePath(filepath), m_RendererID(0) {
	shaderSources source = ParseShader(filepath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
	
}
Shader::~Shader() {
	glDeleteProgram(m_RendererID);
}

unsigned int Shader::CreateShader(std::string vtxSource, std::string fragSource) {
	
	//Source variables
	const char* vtxShaderSource = vtxSource.c_str();
	const char* fragShaderSource = fragSource.c_str();
	
	// create a shader object
	 unsigned int ProgramId = glCreateProgram();

	// create vertex and fragment objects
	unsigned int vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	// Shader source
	glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
	glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

	// compile vertex shader, print error if they occur
	glCompileShader(vertexShaderId);
	
	// compile fragment shader, print error if they occur
	glCompileShader(fragmentShaderId);
	
	// Attach Shaders
	glAttachShader(ProgramId, vertexShaderId);
	glAttachShader(ProgramId, fragmentShaderId);

	//LinkProgram
	glLinkProgram(ProgramId);
	
	//Use shader program
	glUseProgram(ProgramId);

	return ProgramId;
}
//bind shader
void Shader::Bind()const{
	glUseProgram(m_RendererID);
}
//unbind shader
void Shader::Unbind()const{
	glUseProgram(0);
}


//set uniforms
void Shader::SetUniform1i(const std::string& name, int value) {
	glUniform1i(GetUniformLocation(name),value);
}
void Shader::SetUniform1f(const std::string& name, float value) {
	glUniform1f(GetUniformLocation(name), value);
}
void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}
void Shader::SetUniformMat4f(const std::string& name, const glm::mat4 matrix) {
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}
int Shader::GetUniformLocation(const std::string& name) {
	 if(m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];
	
	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
		//std::cout << "Warning: Uniform" << name <<"not found" << std::endl;
	
		m_UniformLocationCache[name] = location;
		return location;
}