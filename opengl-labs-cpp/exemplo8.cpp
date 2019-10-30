#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shader.hpp"
#include "stb_image.h"
#include "objloader.hpp"

int main(void)
{
	if (!glfwInit())
	{
		fprintf(stderr, "Falha ao iniciar o GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1024, 768, "Exemplo 8", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Falha ao abrir a janela GLFW.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Falha ao incializar a GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint programID = LoadShaders("TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Ortho camera :
	//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(4, 3, 3), // Camera está em (4,3,3), no World Space
		glm::vec3(0, 0, 0), // e olhando para a origem
		glm::vec3(0, 1, 0)  // Está virada para cima (mude para 0,-1,0 para olhar de cabeça para baixo)
	);
	// Model matrix : uma matriz identidade (o modelo está na origem)
	glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, vec3(0.3f));
    
	// Nossa ModelViewProjection : multiplicação de nossas 3 matrizes
	glm::mat4 MVP = Projection * View * Model; // Lembrem-se, a multiplicação de matrizes é inversa


    // Another Model
    glm::mat4 anotherModel = glm::mat4(1.0f);
    anotherModel = glm::translate(anotherModel, vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 anotherMVP = Projection * View * anotherModel;
    

    
	// Carrega e cria uma textura
	// -------------------------
	GLuint Texture;
	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture); // todas as próximas operaçções em GL_TEXTURE_2D afetam este objeto
	// parametros de envelopamento (wrapping)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// envelopamento por GL_REPEAT (que é o método padrão)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// parametros de filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// carrega a imagem, cria textura e gera o mipmaps
	int width, height, nrChannels;
	unsigned char* data = stbi_load("uvtemplate.bmp", &width, &height, &nrChannels, 0);
    
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		fprintf(stderr, "Falha ao carregar a textura\n");
	}
	stbi_image_free(data);

	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

	// FIRST OBJECT - CUBO
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normais; // Não está sendo usado
	bool res = loadOBJ("esfera.obj", vertices, uvs, normais);
    
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    
    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
    
    
    // SECOND OBJECT - SPACESHIP
    std::vector<glm::vec3> anotherVertices;
    std::vector<glm::vec2> anotherUvs;
    std::vector<glm::vec3> anotherNormals;
    bool anotherRes = loadOBJ("cruiser.obj", anotherVertices, anotherUvs, anotherNormals);

    GLuint anotherVertexbuffer;
    glGenBuffers(1, &anotherVertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, anotherVertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, anotherVertices.size() * sizeof(glm::vec3), &anotherVertices[0], GL_STATIC_DRAW);
    
    GLuint anotherUvbuffer;
    glGenBuffers(1, &anotherUvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, anotherUvbuffer);
    glBufferData(GL_ARRAY_BUFFER, anotherUvs.size() * sizeof(glm::vec2), &anotherUvs[0], GL_STATIC_DRAW);

    float counter = 0.0f;
    
	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(programID);

        // FIRST OBJECT
        Model = glm::rotate(Model, 0.001f, vec3(0.0f, 1.0f, 0.0f));
        MVP = Projection * View * Model;

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glActiveTexture(GL_TEXTURE0);
        
        glBindTexture(GL_TEXTURE_2D, Texture);
        glUniform1i(TextureID, 0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());

        
        // SECOND OBJECT
        anotherModel = glm::translate(glm::mat4(1.0f), vec3(3.0f * sin(counter), 0.0f, 3.0f * cos(counter)));
        anotherMVP = Projection * View * anotherModel;
        
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &anotherMVP[0][0]);
        glActiveTexture(GL_TEXTURE0);
        
        glBindTexture(GL_TEXTURE_2D, Texture);
        glUniform1i(TextureID, 0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, anotherVertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, anotherUvbuffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_TRIANGLES, 0, anotherVertices.size());


		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glfwSwapBuffers(window);
		glfwPollEvents();
        counter = counter + 0.01f;

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);

	glfwTerminate();

	return 0;
}

