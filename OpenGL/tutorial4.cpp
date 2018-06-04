// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-4-a-colored-cube/

#include "OpenGL.h"

#ifdef TUT4

#include <vmath.h>

GLuint program;
GLuint vao;
GLuint position_buffer;
GLuint color_buffer;
GLint mv_location;
GLint proj_location;

int getWindowWidth()
{
    return 800;
}

int getWindowHeight()
{
    return 600;
}

void onAwake()
{
    static const char * vs_source[] =
    {
        "#version 420 core                                                  \n"
        "                                                                   \n"
        "layout(location = 0) in vec3 position;                             \n"
        "layout(location = 1) in vec3 color;                                \n"
        "                                                                   \n"
        "out vec3 fragmentColor;                                            \n"
        "                                                                   \n"
        "uniform mat4 mv_matrix;                                            \n"
        "uniform mat4 proj_matrix;                                          \n"
        "                                                                   \n"
        "void main(void)                                                    \n"
        "{                                                                  \n"
        "    gl_Position = proj_matrix * mv_matrix * vec4(position, 1);     \n"
        "    fragmentColor = color;                                         \n"
        "}                                                                  \n"
    };

    static const char * fs_source[] =
    {
        "#version 420 core                                                  \n"
        "                                                                   \n"
        "out vec3 color;                                                    \n"
        "                                                                   \n"
        "in vec3 fragmentColor;                                             \n"
        "                                                                   \n"
        "void main(void)                                                    \n"
        "{                                                                  \n"
        "    color = fragmentColor;                                         \n"
        "}                                                                  \n"
    };

    program = glCreateProgram();
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, fs_source, NULL);
    glCompileShader(fs);
    CheckShaderCompileError(fs);

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, vs_source, NULL);
    glCompileShader(vs);
    CheckShaderCompileError(vs);

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);

    mv_location = glGetUniformLocation(program, "mv_matrix");
    proj_location = glGetUniformLocation(program, "proj_matrix");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
        1.0f, 1.0f,-1.0f, // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end
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

    static const GLfloat g_color_buffer_data[] = {
        0.583f,  0.771f,  0.014f,
        0.609f,  0.115f,  0.436f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f,
        0.435f,  0.602f,  0.223f,
        0.310f,  0.747f,  0.185f,
        0.597f,  0.770f,  0.761f,
        0.559f,  0.436f,  0.730f,
        0.359f,  0.583f,  0.152f,
        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,
        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.116f,
        0.676f,  0.977f,  0.133f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,
        0.997f,  0.513f,  0.064f,
        0.945f,  0.719f,  0.592f,
        0.543f,  0.021f,  0.978f,
        0.279f,  0.317f,  0.505f,
        0.167f,  0.620f,  0.077f,
        0.347f,  0.857f,  0.137f,
        0.055f,  0.953f,  0.042f,
        0.714f,  0.505f,  0.345f,
        0.783f,  0.290f,  0.734f,
        0.722f,  0.645f,  0.174f,
        0.302f,  0.455f,  0.848f,
        0.225f,  0.587f,  0.040f,
        0.517f,  0.713f,  0.338f,
        0.053f,  0.959f,  0.120f,
        0.393f,  0.621f,  0.362f,
        0.673f,  0.211f,  0.457f,
        0.820f,  0.883f,  0.371f,
        0.982f,  0.099f,  0.879f
    };

    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glVertexAttribPointer(
        0,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    glGenBuffers(1, &color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void onUpdate(double current_time)
{
    glViewport(0, 0, getWindowWidth(), getWindowHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);

    vmath::mat4 proj_matrix = vmath::perspective(50.0f,
        (float)getWindowWidth() / (float)getWindowHeight(),
        0.1f,
        1000.0f);
    glUniformMatrix4fv(proj_location, 1, GL_FALSE, proj_matrix);

    float f = (float)current_time * 0.3f;
    vmath::mat4 mv_matrix = vmath::translate(0.0f, 0.0f, -4.0f) *
        vmath::translate(sinf(2.1f * f) * 0.5f,
            cosf(1.7f * f) * 0.5f,
            sinf(1.3f * f) * cosf(1.5f * f) * 2.0f) *
        vmath::rotate((float)current_time * 45.0f, 0.0f, 1.0f, 0.0f) *
        vmath::rotate((float)current_time * 81.0f, 1.0f, 0.0f, 0.0f);
    glUniformMatrix4fv(mv_location, 1, GL_FALSE, mv_matrix);

    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
}

void onShutdown()
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
    glDeleteBuffers(1, &position_buffer);
}
#endif
