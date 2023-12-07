#ifndef MATERIAL_H_DEFINED
#define MATERIAL_H_DEFINED
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <stdexcept>

struct Material {
	// Shader program
	GLuint m_program;
	GLuint m_program_SkyBox;
	GLuint m_program_Mesh;

	// Material parameters
	glm::vec4 m_color;
	GLint m_texture;
	GLint m_normal;
	glm::vec3 m_lightPos;
	glm::vec3 m_lightColor;

	unsigned int m_cubemapTexture;
	unsigned int skyboxVAO, skyboxVBO;


	inline void check() {
		if (m_program_SkyBox == 0 || m_program_Mesh == 0) {
			throw std::runtime_error("Shader program not initialized");
		}
	}

	Material(): m_program() {

	}

	virtual ~Material();

	virtual void init();

	virtual void clear();

	void bind(int i);

	virtual void internalBind(int i);

	void setMatrices(glm::mat4& projectionMatrix, glm::mat4& viewMatrix, glm::mat4& modelMatrix);

	GLint getAttribute(const std::string& in_attributeName);

	GLint getUniform(const std::string& in_uniformName);
};

#endif
