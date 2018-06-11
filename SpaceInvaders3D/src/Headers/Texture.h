
#pragma once

#include <string>
#include <GL/glew.h>
#include <FreeImage.h>

class Texture {
public:

	Texture(GLenum TextureTarget, const std::string& FileName);

	bool load();
	void bind(GLenum TextureUnit);
	virtual ~Texture();

	/*
	Estos setters y getters se agregan para el cargador de modelos
	Type: Representa el tipo de material.
	FileName: Se utiliza para recuperar la textura por nombre.
	*/
	std::string getType() {
		return type;
	}
	void setType(std::string type) {
		this->type = type;
	}
	std::string getFileName() {
		return m_fileName;
	}

private:
	std::string m_fileName;
	GLenum m_textureTarget;
	GLuint m_textureObj;
	std::string type;
};