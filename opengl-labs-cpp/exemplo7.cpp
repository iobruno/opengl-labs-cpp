#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shader.hpp"
#include "stb_image.h"

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

	window = glfwCreateWindow(1024, 768, "Exemplo 6", NULL, NULL);
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
		glm::vec3(0, 1, 0)  // Est� virada para cima (mude para 0,-1,0 para olhar de cabeça para baixo)
	);
	// Model matrix : uma matriz identidade (o modelo está na origem)
	glm::mat4 Model = glm::mat4(1.0f);
	// Nossa ModelViewProjection : multiplicação de nossas 3 matrizes
	glm::mat4 MVP = Projection * View * Model; // Lembrem-se, a multiplicação de matrizes é inversa

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

	// Os vértices. Três floats consecutivos formam um vértice 3D; 
	// Três vértices consecutivos formam um triângulo.
	// Um cubo tem 6 faces com 2 triângulos cada, então são 6*2=12 triângulos e 12*3 vértices
	
    // TELA
    static const GLfloat g_vertex_buffer_data[] = {
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f
	};

    
    glFrontFace(GL_CCW);
    
	// Duas cordenadas UV para cada vértice. As coordenadas foram criadas com o Blender.
    // TEXTURA
	static const GLfloat g_uv_buffer_data[] = {
        // 1-1
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,

        // 2-1
        0.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,

        // ???
        0.0f, 0.f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        
        // 2-2
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        
        // 1-2
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // ??
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // 3-1
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        
        // 4-1
        0.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        
        // 4-2
        1.0f, 1.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        
        // topo-1
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        
        //topo-2
        0.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        
        //3-2
        1.0f, 0.0f,
        0.0f, 0.0f,
        1.0f, 1.0f
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

    float i = 0.0;
    
	do {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programID);

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(TextureID, 0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  
			3,                  
			GL_FLOAT,           
			GL_FALSE,           
			0,                  
			(void*)0           
		);


		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		// Desenha o triângulo
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 índices começando em 0 -> 12 triângulos -> 6 quadrados

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glfwSwapBuffers(window);
		glfwPollEvents();

        Model = glm::rotate(Model, 0.005f, vec3(0.0f, 1.0f, 0.0f));
        MVP = Projection * View * Model;
        
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);

	glfwTerminate();

	return 0;
}

