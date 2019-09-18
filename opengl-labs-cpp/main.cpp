#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.hpp"

GLFWwindow* window;

using namespace glm;

float zAxis = 0.0;
float xCam = 0.0;
float yCam = 0.0;

double mouseInitialPosX;
double mouseInitialPosY;


void scrollCallBack(GLFWwindow *window, double xOffset, double yOffset)
{
    if (yOffset > 0) {
        zAxis += 0.05;
    } else {
        zAxis -= 0.05;
    }
}

void mouseCallback(GLFWwindow *window, double xOffset, double yOffset)
{
    double deslocamentoX = xOffset - mouseInitialPosX;
    mouseInitialPosX = xOffset;

    double deslocamentoY = yOffset - mouseInitialPosY;
    mouseInitialPosY = yOffset;
    
    xCam += (deslocamentoX * 0.05);
    yCam += (deslocamentoY * 0.05);
}


int main( void )
{
    if( !glfwInit() )
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
    
    window = glfwCreateWindow( 1024, 768, "Hello World", NULL, NULL);
    if( window == NULL ){
        fprintf(stderr, "Falha ao abrir a janela GLFW.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwGetCursorPos(window, &mouseInitialPosX, &mouseInitialPosY); // Me da a posicao initial do mouse na tela

    // Scroll callback
    glfwSetScrollCallback(window, scrollCallBack);
    // Cursor callback
    glfwSetCursorPosCallback(window, mouseCallback);
    
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
    GLuint VertexArrayID;
    
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    GLuint programID = LoadShaders(
       "SimpleTransform.vertexshader",
       "SingleColor.fragmentshader"
   );
    
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    
    // Projection matrix : 45∞ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    // Ortho camera :
    //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
    
    // Camera matrix
    glm::mat4 View = glm::lookAt(
       glm::vec3(0,0,3), // Camera est· em (4,3,3), no World Space
       glm::vec3(0,0,0), // e olhando para a origem
       glm::vec3(0,1,0)  // Est· virada para cima (mude para 0,-1,0 para olhar de cabeÁa para baixo)
    );
    
    // Model matrix : uma matriz identidade (o modelo esta· na origem)
    glm::mat4 Model = glm::mat4(1.0f);
    // Nossa ModelViewProjection : multiplicaÁ„o de nossas 3 matrizes

    
    glm::mat4 MVP = Projection * View * Model; // Lembrem-se, a multiplicaÁ„o de matrizes È inversa
    
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f,
    };
    
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    
    float modelXAxis = 0.0;
    float modelYAxis = 0.0;
    
    do{
        
        glClear( GL_COLOR_BUFFER_BIT );
        glUseProgram(programID);
        
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            
            modelXAxis += 0.05;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            modelXAxis -= 0.05;

        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            modelYAxis += 0.05;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            modelYAxis -= 0.05;

        }

        
        
        // Trasladar no eixo Z
        Model = glm::translate(glm::mat4(1.0f), glm::vec3(modelXAxis, modelYAxis, zAxis));
        
        View = glm::lookAt(
           glm::vec3(xCam, yCam, 3),
           glm::vec3(0,0,0),
           glm::vec3(0,1,0)
        );
        
        MVP = Projection * View * Model;
    
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                              0,3,
                              GL_FLOAT,
                              GL_FALSE,
                              0,
                              (void*)0
                              );
        
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glDisableVertexAttribArray(0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    }
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0 );
    
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);
    
    glfwTerminate();
    
    return 0;
}

