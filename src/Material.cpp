// Local includes
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "Context.h"
#include "stb_image.h"
// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
// OPENGL includes
#include <GL/glew.h>
#include <GL/glut.h>

#include <vector>




Material::~Material() {
	if (m_program != 0) {
		glDeleteProgram(m_program);
	}
}



void Material::init() {
	// TODO : Change shader by your
	m_program_Mesh = load_shaders("shaders/unlit/vertex.glsl", "shaders/unlit/fragment.glsl");
	m_program_SkyBox = load_shaders("shaders/unlit/vertexSky.glsl", "shaders/unlit/fragmentSky.glsl");

	check();
	// TODO : set initial parameters
	m_color = {1.0, 1.0, 1.0, 1.0};
	m_lightPos = { 0.0, 1.0, 0.0 };
	m_lightColor = { 1.0, 1.0, 1.0 };

	m_texture = loadTexture2DFromFilePath("./textures/WaterBottle_baseColor.png");
	m_normal = loadTexture2DFromFilePath("./textures/WaterBottle_normal.png");

	std::vector<std::string> faces = {
		"./textures/skybox/right.jpg",
		"./textures/skybox/left.jpg",
		"./textures/skybox/top.jpg",
		"./textures/skybox/bottom.jpg",
		"./textures/skybox/front.jpg",
		"./textures/skybox/back.jpg"
	};


	float skyboxVertices[] = {
		// positions          
		-20.0f,  20.0f, -20.0f,
		-20.0f, -20.0f, -20.0f,
		20.0f, -20.0f, -20.0f,
		20.0f, -20.0f, -20.0f,
		20.0f,  20.0f, -20.0f,
		-20.0f,  20.0f, -20.0f,

		-20.0f, -20.0f,  20.0f,
		-20.0f, -20.0f, -20.0f,
		-20.0f,  20.0f, -20.0f,
		-20.0f,  20.0f, -20.0f,
		-20.0f,  20.0f,  20.0f,
		-20.0f, -20.0f,  20.0f,

		20.0f, -20.0f, -20.0f,
		20.0f, -20.0f,  20.0f,
		20.0f,  20.0f,  20.0f,
		20.0f,  20.0f,  20.0f,
		20.0f,  20.0f, -20.0f,
		20.0f, -20.0f, -20.0f,

		-20.0f, -20.0f,  20.0f,
		-20.0f,  20.0f,  20.0f,
		20.0f,  20.0f,  20.0f,
		20.0f,  20.0f,  20.0f,
		20.0f, -20.0f,  20.0f,
		-20.0f, -20.0f,  20.0f,

		-20.0f,  20.0f, -20.0f,
		20.0f,  20.0f, -20.0f,
		20.0f,  20.0f,  20.0f,
		20.0f,  20.0f,  20.0f,
		-20.0f,  20.0f,  20.0f,
		-20.0f,  20.0f, -20.0f,

		-20.0f, -20.0f, -20.0f,
		-20.0f, -20.0f,  20.0f,
		20.0f, -20.0f, -20.0f,
		20.0f, -20.0f, -20.0f,
		-20.0f, -20.0f,  20.0f,
		20.0f, -20.0f,  20.0f
	};


	m_cubemapTexture = loadCubemap(faces);


	// Création du VAO et du VBO
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	
	// Liaison du VAO
	glBindVertexArray(skyboxVAO);

	// Liaison du VBO
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


}


void Material::clear() {
	// nothing to clear
	// TODO: Add the texture or buffer you want to destroy for your material
}

void Material::bind(int i) {
	check();
	
	if(i == 0){
        m_program = m_program_SkyBox;
    } else {
        m_program = m_program_Mesh;
    }
	glUseProgram(m_program);
	internalBind(i);
}

void Material::internalBind(int i) {
	// bind parameters

	if(i == 0){

        glDepthFunc(GL_LEQUAL);
        glBindVertexArray(skyboxVAO);
		if (m_cubemapTexture != -1){

        	glActiveTexture(GL_TEXTURE0);
        	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapTexture);
			glUniform1i(getUniform("skybox"), 0);
		}
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
	} 
	else {
		
		GLint color = getUniform("color");
		glUniform4fv(color, 1, glm::value_ptr(m_color));


		if (m_texture != -1) {
			glActiveTexture(GL_TEXTURE0+1);
			glBindTexture(GL_TEXTURE_2D, m_texture);
			glUniform1i(getUniform("colorTexture"), 1);
			
		}
		if(m_normal != -1){
			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_2D, m_normal);
			glUniform1i(getUniform("normalTexture"), 2);
		}

		
	}

}

void Material::setMatrices(glm::mat4& projectionMatrix, glm::mat4& viewMatrix, glm::mat4& modelMatrix)
{
	check();
	glUniformMatrix4fv(getUniform("projection"), 1, false, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(getUniform("view"), 1, false, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(getUniform("model"), 1, false, glm::value_ptr(modelMatrix));
	glUniform3fv(getUniform("lightColor"), 1, glm::value_ptr(m_lightColor));
	glUniform3fv(getUniform("lightPos"), 1, glm::value_ptr(m_lightPos));
	glUniform3fv(getUniform("viewPos"), 1, glm::value_ptr(Context::camera.position));
}

GLint Material::getAttribute(const std::string& in_attributeName) {
	check();
	return glGetAttribLocation(m_program, in_attributeName.c_str());
}

GLint Material::getUniform(const std::string& in_uniformName) {
	check();
	return glGetUniformLocation(m_program, in_uniformName.c_str());
}

