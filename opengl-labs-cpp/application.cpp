#include <iostream>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "objloader.hpp"
#include "shader.hpp"
#include "stb_image.h"

using namespace glm;
using namespace std;

GLFWwindow* window;

void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 6.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, -1.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = 512;
float lastY = 384;

float yaw = -90.0f;
float pitch = 0.0f;


void loadTexture(GLuint texture, char const *filepath) {
    glBindTexture(GL_TEXTURE_2D, texture);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);
}

void processInput(GLFWwindow *window)
{
    float cameraSpeed = 1.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    yaw   += xoffset;
    pitch += yoffset;
    
    if(pitch > 89.0f) {
      pitch =  89.0f;
    }
    if(pitch < -89.0f) {
      pitch = -89.0f;
    }
    
    glm::vec3 front;
    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    cameraFront = glm::normalize(front);
}

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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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

    // Setting up Callbacks
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Projection matrix : 45° Field of View, 16:9 ratio, display range : 0.1 unit <-> 100 units
    mat4 Projection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    mat4 View = glm::lookAt(
        cameraPos,
        cameraPos+cameraFront,
        cameraUp
    );

    // 3D OBJECT: SUN
    mat4 sunModel = mat4(1.0f);
    mat4 sunMVP = Projection * View * sunModel;

	vector<vec3> sunVertex;
	vector<vec2> sunUV;
	vector<vec3> sunNormal;
	loadOBJ("../resources/objects/sun.obj", sunVertex, sunUV, sunNormal);
    
    GLuint sunVertexBuffer;
    glGenBuffers(1, &sunVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, sunVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sunVertex.size() * sizeof(vec3), &sunVertex[0], GL_STATIC_DRAW);
    
    GLuint sunUVBuffer;
    glGenBuffers(1, &sunUVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, sunUVBuffer);
    glBufferData(GL_ARRAY_BUFFER, sunUV.size() * sizeof(vec2), &sunUV[0], GL_STATIC_DRAW);
    
    
    // 3D OBJECT: EARTH
    mat4 earthModel = mat4(1.0f);
    mat4 earthMVP = Projection * View * earthModel;

    vector<vec3> earthVertices;
    vector<vec2> earthUVs;
    vector<vec3> earthNormals;
    loadOBJ("../resources/objects/earth_day.obj", earthVertices, earthUVs, earthNormals);
    
    GLuint earthVertexBuffer;
    glGenBuffers(1, &earthVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, earthVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, earthVertices.size() * sizeof(vec3), &earthVertices[0], GL_STATIC_DRAW);
    
    GLuint earthUVBuffer;
    glGenBuffers(1, &earthUVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, earthUVBuffer);
    glBufferData(GL_ARRAY_BUFFER, earthUVs.size() * sizeof(vec2), &earthUVs[0], GL_STATIC_DRAW);
    
    
    // 3D OBJECT: MOON
    mat4 moonModel = mat4(1.0f);
    mat4 moonMVP = Projection * View * moonModel;
    
    vector<vec3> moonVertices;
    vector<vec2> moonUVs;
    vector<vec3> moonNormals;
    loadOBJ("../resources/objects/moon.obj", moonVertices, moonUVs, moonNormals);

    GLuint moonVertexBuffer;
    glGenBuffers(1, &moonVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, moonVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, moonVertices.size() * sizeof(vec3), &moonVertices[0], GL_STATIC_DRAW);
    
    GLuint moonUVBuffer;
    glGenBuffers(1, &moonUVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, moonUVBuffer);
    glBufferData(GL_ARRAY_BUFFER, moonUVs.size() * sizeof(vec2), &moonVertices[0], GL_STATIC_DRAW);


    // TEXTURE LOADER
    GLuint *textures = new GLuint[3];
    glGenTextures(3, textures);
    loadTexture(textures[0], "../resources/textures/2k_sun.jpg");
    loadTexture(textures[1], "../resources/textures/2k_earth_daymap.jpg");
    loadTexture(textures[2], "../resources/textures/2k_moon.jpg");


    float counter = 0.0f;
    
	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(programID);

        // FIRST OBJECT
        sunModel = rotate(sunModel, 0.001f, vec3(0.0f, 1.0f, 0.0f));
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
        earthModel = translate(sunModel, vec3(5.0f * sin(counter * 0.1f), 0.0f, 5.0f * cos(counter * 0.1f)));
        earthModel = scale(earthModel, vec3(0.8f));
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
        moonModel = translate(earthModel, vec3(2.0f * sin(counter), 0.0f, 2.0f * cos(counter)));
        moonModel = scale(moonModel, vec3(1.0f));
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
                
        
        // PORQUE SIM
        processInput(window);
        View = glm::lookAt(
            cameraPos,
            cameraPos+cameraFront,
            cameraUp
        );
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwSetCursorPosCallback(window, mouse_callback);

        
        glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glfwSwapBuffers(window);
		glfwPollEvents();
        counter = counter + 0.01f;

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);

	glDeleteBuffers(1, &sunVertexBuffer);
	glDeleteBuffers(1, &sunUVBuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	glfwTerminate();

	return 0;
}



