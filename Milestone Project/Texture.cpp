//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Pulls in a texture image and sets it to a render id. Allows for texture immages to be mapped to texture   //   
//coordinates of the objects in the scene                                                                   //
//                                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Texture.h"
#include "stb_image.h"
//set texture
Texture::Texture(const std::string& path) 
	: m_RendererID(0),m_FilePath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0){

	stbi_set_flip_vertically_on_load(1);
	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);
	// generate textures and set to render ID
	glGenTextures(1, &m_RendererID);
	//bind texture to Render ID
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
	//Set parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//set image
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,m_Width,m_Height, 0, GL_RGBA,GL_UNSIGNED_BYTE, m_LocalBuffer);
	//bind texture
	glBindTexture(GL_TEXTURE_2D, 0);
	// check local buffer
	if (m_LocalBuffer);
			stbi_image_free(m_LocalBuffer);
}	
Texture::~Texture() {
	// delete texture based on render id
	glDeleteTextures(1, &m_RendererID);
}
// bind texture
void Texture::Bind(unsigned int slot /*= 0*/) const {
	// check active texture based on slot
	glActiveTexture(GL_TEXTURE0 + slot);
	// bind texture based on render id
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}
//unbind texture
void Texture::Unbind() const{
	glBindTexture(GL_TEXTURE_2D, 0);
	}

