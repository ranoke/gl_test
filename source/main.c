#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


const char* vertex_src = "\n"
"#version 330 core\n"
"layout(location = 0) in vec3 a_pos;\n"
"layout(location = 1) in vec2 a_uv;\n"
"uniform mat4 u_projection;\n"
"uniform mat4 u_view;\n"
"out vec3 u_col;\n"
"out vec2 uv;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(a_pos, 1.0);\n"
"	uv = a_uv;\n"
"   u_col = a_pos;\n"
"}";

const char* fragment_src = "\n"
"#version 330 core\n"
"uniform sampler2D u_tex;\n"
"in vec3 u_col;\n"
"in vec2 uv;\n"
"out vec4 frag_color;\n"
"void main()\n"
"{\n"
"	frag_color = texture(u_tex, uv);//vec4(u_col, 1.f);\n"
"}";

float vertices[] = {
    0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f, 0.0f,  0.0f, 1.0f
};
uint32_t indices[] = {
    0, 1, 3,
    1, 2, 3
};

typedef struct color_t{
    GLubyte r,g,b,a;
}color_t; 

uint32_t shader_compile(const char* vertex_src, const char* fragment_src);

void p_d(uint32_t* d)
{
    for(int i = 0; i < 25; i++)
    {
        if(i%5==0){puts(""); continue;}
        printf("%d ", d[i*3]);
    }
}

int main()
{
    if(!glfwInit())
    {
        assert(false);
        return -1;
    }
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(600, 600, "Texture", 0, 0);
    glfwMakeContextCurrent(window);

    if (!window)
    {
        assert(false);
        glfwTerminate();
        return -1;
    }

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        assert(false);
        return -1;
    }

    uint32_t program = shader_compile(vertex_src, fragment_src);

    uint32_t vao, vbo, ibo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);

    // would like to use glNamed... over this 2 function calls  but it
    // is supported in 4.5version
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);  
    
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);;

    glUseProgram(program);

    uint32_t w = 5, h = 5;
    color_t* data = malloc(w*h*sizeof(color_t));
    for(int i = 0; i < 25; i++)
        if(i%2==0) data[i] = (color_t){255,0,0};

    

    glUseProgram(program);
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // uint32_t color_loc = glGetUniformLocation(program, "u_col");
    // glUniform3f(color_loc, 0.4f, 0.f, 0.f);
    uint32_t texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(0,0);

    glClearColor(0.25, 0.25, 0.25, 1.f);
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        
        for(int i = 0; i < 25; i++)
            data[i] = (color_t){sin(glfwGetTime())*255,0,0, 100};
        //memset(data, 0, w*h*sizeof(color_t));

        glBindTexture(GL_TEXTURE_2D, texture);    //A texture you have already created storage for
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_BGRA, GL_UNSIGNED_BYTE, data);

     

        // glDrawArrays(GL_TRIANGLES, 0, 6);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}


uint32_t shader_compile(const char* vertex_src, const char* fragment_src)
{
    uint32_t vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_src, 0);
    glCompileShader(vertex); 

    // check if the shader was compiled
    {
        uint32_t success = 0;
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if(success == GL_FALSE)
        {
            uint32_t maxLength = 0;
            glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &maxLength);

            char log[1024];
            glGetShaderInfoLog(vertex, maxLength, &maxLength, log);
            printf("%s\n", log);
            assert(false && "vertex shader compilation error");
            glDeleteShader(vertex);
            return 0;
        }
    }

    uint32_t fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_src, 0);
    glCompileShader(fragment);
    {
        uint32_t success = 0;
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if(success == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &maxLength);

            char log[1024];
            glGetShaderInfoLog(vertex, maxLength, &maxLength, log);
            printf("%s\n", log);
            glDeleteShader(vertex);
            assert(false && "fragment compilation error");
            return 0;
        }
    }

    uint32_t program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);

    glLinkProgram(program);
    {
        // Note the different functions here: glGetProgram* instead of glGetShader*.
        uint32_t success = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (success == GL_FALSE)
        {
            uint32_t maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            char log[1024];
            glGetProgramInfoLog(program, maxLength, &maxLength, log);
            
            printf("%s\n", log);
            assert(false && "failed to link shader");
            glDeleteProgram(program);
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            return program;
        }
    }


    return program;
}