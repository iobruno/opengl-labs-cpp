#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.hpp"
#include "stb_image.h"
#include "objloader.hpp"

using namespace glm;

GLFWwindow* window;

int main(void) {
    
	if (!glfwInit()) {
		fprintf(stderr, "Falha ao iniciar o GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1920, 1080, "Exemplo 8", NULL, NULL);
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

	GLuint programID = LoadShaders(
       "../resources/shaders/TransformVertexShader.vertexshader",
       "../resources/shaders/TextureFragmentShader.fragmentshader"
    );
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Projection matrix : 45° Field of View, 16:9 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(
		glm::vec3(0, 0, 7),
		glm::vec3(0, 0, 0),
		glm::vec3(0, -1, 0)
	);
        
    // 3D OBJECT: SUN
    glm::mat4 sunModel = glm::mat4(1.0f);
    glm::mat4 sunMVP = Projection * View * sunModel;

	std::vector<glm::vec3> sunVertex;
	std::vector<glm::vec2> sunUV;
	std::vector<glm::vec3> sunNormal;
	loadOBJ("../resources/objects/sun.obj", sunVertex, sunUV, sunNormal);
    
    GLuint sunVertexBuffer;
    glGenBuffers(1, &sunVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, sunVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sunVertex.size() * sizeof(glm::vec3), &sunVertex[0], GL_STATIC_DRAW);
    
    GLuint sunUVBuffer;
    glGenBuffers(1, &sunUVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, sunUVBuffer);
    glBufferData(GL_ARRAY_BUFFER, sunUV.size() * sizeof(glm::vec2), &sunUV[0], GL_STATIC_DRAW);
    
    
    // 3D OBJECT: EARTH
    glm::mat4 earthModel = glm::mat4(1.0f);
    glm::mat4 earthMVP = Projection * View * earthModel;

    std::vector<glm::vec3> earthVertices;
    std::vector<glm::vec2> earthUVs;
    std::vector<glm::vec3> earthNormals;
    loadOBJ("../resources/objects/earth_night.obj", earthVertices, earthUVs, earthNormals);
    
    GLuint earthVertexBuffer;
    glGenBuffers(1, &earthVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, earthVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, earthVertices.size() * sizeof(glm::vec3), &earthVertices[0], GL_STATIC_DRAW);
    
    GLuint earthUVBuffer;
    glGenBuffers(1, &earthUVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, earthUVBuffer);
    glBufferData(GL_ARRAY_BUFFER, earthUVs.size() * sizeof(glm::vec2), &earthUVs[0], GL_STATIC_DRAW);
    
    
    // 3D OBJECT: MOON
    glm::mat4 moonModel = glm::mat4(1.0f);
    moonModel = glm::scale(moonModel, vec3(0.1f));
    glm::mat4 moonMVP = Projection * View * moonModel;
    
    std::vector<glm::vec3> moonVertices;
    std::vector<glm::vec2> moonUVs;
    std::vector<glm::vec3> moonNormals;
    loadOBJ("../resources/objects/moon.obj", moonVertices, moonUVs, moonNormals);

    GLuint moonVertexBuffer;
    glGenBuffers(1, &moonVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, moonVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, moonVertices.size() * sizeof(glm::vec3), &moonVertices[0], GL_STATIC_DRAW);
    
    GLuint moonUVBuffer;
    glGenBuffers(1, &moonUVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, moonUVBuffer);
    glBufferData(GL_ARRAY_BUFFER, moonUVs.size() * sizeof(glm::vec2), &moonVertices[0], GL_STATIC_DRAW);

    // TEXTURE LOADER
    GLuint *textures = new GLuint[3];
    glGenTextures(3, textures);
    
    
    // TEXTURE LOADER: SUN
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    int sunWidth, sunHeight, sunNRChannels;
    unsigned char* sunData = stbi_load("../resources/textures/2k_sun.jpg", &sunWidth, &sunHeight, &sunNRChannels, 0);
    if (sunData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sunWidth, sunHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, sunData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(sunData);


    // TEXTURE LOADER: EARTH
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    int earthWidth, earthHeight, earthNRChannels;
    unsigned char* earthData = stbi_load("../resources/textures/2k_earth_nightmap.jpg", &earthWidth, &earthHeight, &earthNRChannels, 0);
    if (earthData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, earthWidth, earthHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, earthData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(earthData);


    // TEXTURE LOADER: MOON
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    int moonWidth, moonHeight, moonNRChannels;
    unsigned char* moonData = stbi_load("../resources/textures/2k_moon.jpg", &moonWidth, &moonHeight, &moonNRChannels, 0);
    if (moonData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, moonWidth, moonHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, moonData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(moonData);

        
    float counter = 0.0f;
    
	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(programID);

        // FIRST OBJECT
        sunModel = glm::rotate(sunModel, 0.001f, vec3(0.0f, 1.0f, 0.0f));
        sunMVP = Projection * View * sunModel;

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &sunMVP[0][0]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glUniform1i(TextureID, 0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, sunVertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, sunUVBuffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_TRIANGLES, 0, sunVertex.size());

        
        // SECOND OBJECT
        earthModel = glm::translate(sunModel, vec3(5.0f * sin(counter * 0.1f), 0.0f, 5.0f * cos(counter * 0.1f)));
        earthMVP = Projection * View * earthModel;
        
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &earthMVP[0][0]);
        glActiveTexture(GL_TEXTURE0);
        
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glUniform1i(TextureID, 0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, earthVertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, earthUVBuffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_TRIANGLES, 0, earthVertices.size());

        
        // THIRD OBJECT
        moonModel = glm::translate(earthModel, vec3(2.0f * sin(counter), 0.0f, 2.0f * cos(counter)));
        moonModel = glm::scale(moonModel, vec3(0.2f));
        moonMVP = Projection * View * moonModel;
        
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &moonMVP[0][0]);
        glActiveTexture(GL_TEXTURE0);
        
        glBindTexture(GL_TEXTURE_2D, textures[2]);
        glUniform1i(TextureID, 0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, moonVertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, moonUVBuffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_TRIANGLES, 0, moonVertices.size());
                
        glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glfwSwapBuffers(window);
		glfwPollEvents();
        counter = counter + 0.01f;

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glDeleteBuffers(1, &sunVertexBuffer);
	glDeleteBuffers(1, &sunUVBuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	glfwTerminate();

	return 0;
}
