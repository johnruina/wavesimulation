
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <random>
#include <algorithm>

#include "Camera.h"
#include "shaderClass.h"
#include "Window.h"
#include "VBO.h"
#include "VAO.h"
#include "EBO.h"
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::vec3 hsv_to_rgb(glm::vec3 color) {
    // Translates HSV color to RGB color
    // H: 0.0 - 360.0, S: 0.0 - 100.0, V: 0.0 - 100.0
    // R, G, B: 0.0 - 1.0

    float hue = color.x;
    float saturation = color.y;
    float value = color.z;

    float c = (value / 100) * (saturation / 100);
    float x = c * (1 - abs((hue/60 - 2 * floor(hue/120)) - 1));
    float m = (value / 100) - c;

    float r = 0;
    float g = 0;
    float b = 0;

    if (hue >= 0 && hue < 60) {
        r = c;
        g = x;
        b = 0;
    }
    else if (hue >= 60 && hue < 120) {
        r = x;
        g = c;
        b = 0;
    }
    else if (hue >= 120 && hue < 180) {
        r = 0;
        g = c;
        b = x;
    }
    else if (hue >= 180 && hue < 240) {
        r = 0;
        g = x;
        b = c;
    }
    else if (hue >= 240 && hue < 300) {
        r = x;
        g = 0;
        b = c;
    }
    else if (hue >= 300 && hue < 360) {
        r = c;
        g = 0;
        b = x;
    }

    r += m;
    g += m;
    b += m;

    return glm::vec3(r, g, b);
}

int WinMain() {

    const int x = 256;
    const int y = 64;

    unsigned int timestep = 1;
    unsigned int spatialstep = 1;
    float cellsize = 2;
    

    unsigned int height = 720;
    unsigned int width =1280;

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    std::vector<GLfloat> ys0(x*y,0);
    std::vector<GLfloat> ys1(x * y, 0);
    std::vector<GLfloat> ys2(x * y, 0);

    GLfloat vertices[x * y * 6] = {

    };

    for (int i = 0; i < y; i++) {
        for (int ie = 0; ie < x; ie++) {
            vertices[6 * (i * x + ie) + 0] = ie * cellsize;
            vertices[6 * (i * x + ie) + 1] = 0;
            vertices[6 * (i * x + ie) + 2] = i * cellsize;
            vertices[6 * (i * x + ie) + 3] = float(ie)/ float(x);
            vertices[6 * (i * x + ie) + 4] = float(i)/ float(y);
            vertices[6 * (i * x + ie) + 5] = 100.0f/255.0f;
        }
    }

    GLuint indices[6 * (y - 1) * (x -1)] = {};

    for (int i = 0; i < y - 1; i++) {
        for (int ie = 0; ie < x - 1; ie++) {
            indices[6 * (i * (x-1) + ie) + 0] = ie + (i*x);
            indices[6 * (i * (x - 1) + ie) + 1] = (ie + 1) + (i * x);
            indices[6 * (i * (x - 1) + ie) + 2] = (ie+1) + ((i+1) * x);
            indices[6 * (i * (x - 1) + ie) + 3] = ie + (i * x);
            indices[6 * (i * (x - 1) + ie) + 4] = ie + ((i+1) * x);
            indices[6 * (i * (x - 1) + ie) + 5] = (ie + 1) + ((i + 1) * x);
        }
    }

    GLFWwindow* window = glfwCreateWindow(width,height,"eiifwhrn",NULL,NULL);
    if (window == NULL) {

        glfwTerminate();
        throw -1;
    }
    glfwMakeContextCurrent(window);

    gladLoadGL();
    glViewport(0, 0, width, height);

    Shader ShaderProgram("default.vert", "default.frag");

    VAO vao;
    vao.Bind();

    VBO vbo(vertices, sizeof(vertices));
    EBO ebo(indices, sizeof(indices));
    vao.LinkVBO(vbo, 0, 3 , GL_FLOAT, 6* sizeof(float), (void*)0  );
    vao.LinkVBO(vbo, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3*sizeof(float)) );
    //vao.LinkVBO(vbo, 2, 2, GL_FLOAT, 6 * sizeof(float), (void*)(6 * sizeof(float)));
    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();
    
    //TEXTURE

    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(1);

    Camera camera(width,height,glm::vec3(0.0f,0.0f,2.0f) );

    ys0[ys0.size()/2] = 1;

    int frame = 0;
    while (!glfwWindowShouldClose(window))
    {
        frame++;

        if (frame % 60 == 0) {
            ys0[rand()%ys0.size()] = 1;
        }

        //std::cout << frame << "\n";
        //ys0[ys0.size() / 2] = 1;
        ys2 = ys1;
        ys1 = ys0;

        for (int i = 0; i < ys0.size(); i++) {
            if (i <= x or i )
            ys0[i] = 0.0f;
            //REBUILD

            if (i % x >= 1) {
                ys0[i] += ys1[i - 1];
            }
            if (i % x < x - 1) {
                ys0[i] += ys1[i + 1];
            }

            if (i -x > 0) {
                ys0[i] += ys1[i - x];
            }
            
            if (i + x < ys0.size()) {
                ys0[i] += ys1[i + x];
            }
            ys0[i] -= ys1[i] * 4;
            ys0[i] *= 0.05;
            ys0[i] += 2 * ys1[i];
            ys0[i] -= ys2[i];
        }

        for (int i = 0; i < x*y; i++) {
            vertices[1 + i * 6] *= 0.995;
            vertices[1 + i * 6] = ys1[i] * cellsize * 5;
            glm::vec3 color = hsv_to_rgb({ys1[i] * 720.0f + 180.0f,100.0f,100.0f});
            vertices[3 + i * 6] = color.r;
            vertices[4 + i * 6] = color.g;
            vertices[5 + i * 6] = color.b;
        }

        glBufferSubData(GL_ARRAY_BUFFER,NULL,sizeof(vertices), vertices);

        glBindBuffer(GL_ARRAY_BUFFER, vbo.ID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //RENDER
        glClearColor(0.52f, 0.807f, 0.96f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        ShaderProgram.Activate();
        
        camera.Inputs(window);

        camera.Matrix(90.0f,0.1f,2000.0f,ShaderProgram,"camMatrix");
        
        vao.Bind();
        
        glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    vao.Delete();
    vbo.Delete();
    ebo.Delete();

    ShaderProgram.Delete();


    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;

}