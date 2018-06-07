// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/

#include "OpenGL.h"

#ifdef TUT7

#include <vector>
#include <vmath.h>

GLuint program;
GLuint vao;
GLuint position_buffer;
GLuint uv_buffer;
GLuint mv_location;
GLuint proj_location;
GLuint tex_location;

GLuint loadBMP(const char *imagepath);
bool loadOBJ(
    const char * path,
    std::vector <vmath::vec3> & out_vertices,
    std::vector <vmath::vec2> & out_uvs,
    std::vector <vmath::vec3> & out_normals
);

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
    static const char *vs_source[] =
    {
        "#version 420 core                                                  \n"
        "                                                                   \n"
        "layout(location = 0) in vec3 position;                             \n"
        "layout(location = 1) in vec2 uv;                                   \n"
        "                                                                   \n"
        "out vec2 fragmentUV;                                               \n"
        "                                                                   \n"
        "uniform mat4 mv_matrix;                                            \n"
        "uniform mat4 proj_matrix;                                          \n"
        "                                                                   \n"
        "void main(void)                                                    \n"
        "{                                                                  \n"
        "    gl_Position = proj_matrix * mv_matrix * vec4(position, 1);     \n"
        "    fragmentUV = uv;                                               \n"
        "}                                                                  \n"
    };

    static const char *fs_source[] =
    {
        "#version 420 core                                                  \n"
        "                                                                   \n"
        "out vec3 color;                                                    \n"
        "                                                                   \n"
        "in vec2 fragmentUV;                                                \n"
        "                                                                   \n"
        "uniform sampler2D texSampler;                                      \n"
        "                                                                   \n"
        "void main(void)                                                    \n"
        "{                                                                  \n"
        "    color = texture(texSampler, fragmentUV).rgb;                   \n"
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
    glUseProgram(program);

    mv_location = glGetUniformLocation(program, "mv_matrix");
    proj_location = glGetUniformLocation(program, "proj_matrix");
    tex_location = glGetUniformLocation(program, "texSampler");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    std::vector<vmath::vec3> vertices;
    std::vector<vmath::vec2> uvs;
    std::vector<vmath::vec3> normals; // Won't be used at the moment.
    bool res = loadOBJ("cube.obj", vertices, uvs, normals);

    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vmath::vec3), &vertices[0], GL_STATIC_DRAW);

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

    glGenBuffers(1, &uv_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vmath::vec2), &uvs[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    GLuint image = loadBMP("./uvtemplate.bmp");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image);
    glUniform1i(tex_location, 0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void onUpdate(double current_time)
{
    glViewport(0, 0, getWindowWidth(), getWindowHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

GLuint loadBMP(const char *imagepath)
{
    // Data read from the header of the BMP file
    unsigned char header[54]; // Each BMP file begins by a 54-bytes header
    unsigned int dataPos;     // Position in the file where the actual data begins
    unsigned int width, height;
    unsigned int imageSize;   // = width * height * 3
                              // Actual RGB data
    unsigned char *data;

    FILE *file = new FILE;
    if (fopen_s(&file, imagepath, "rb"))
    {
        printf("Image could not be opened\n");
        return 0;
    }

    if (fread(header, 1, 54, file) != 54)
    {
        // If not 54 bytes read : problem
        printf("Not a correct BMP file\n");
        return false;
    }

    if (header[0] != 'B' || header[1] != 'M')
    {
        printf("Not a correct BMP file\n");
        return 0;
    }

    // Read ints from the byte array
    dataPos = *(int*)&(header[0x0A]);
    imageSize = *(int*)&(header[0x22]);
    width = *(int*)&(header[0x12]);
    height = *(int*)&(header[0x16]);

    // Some BMP files are misformatted, guess missing information
    if (imageSize == 0)    imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

    data = new unsigned char[imageSize]; // Create a buffer

                                         // Read the actual data from the file into the buffer
    fread(data, 1, imageSize, file);

    //Everything is in memory now, the file can be closed
    fclose(file);

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return textureID;
}

bool loadOBJ(
    const char * path,
    std::vector <vmath::vec3> & out_vertices,
    std::vector <vmath::vec2> & out_uvs,
    std::vector <vmath::vec3> & out_normals
)
{
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<vmath::vec3> temp_vertices;
    std::vector<vmath::vec2> temp_uvs;
    std::vector<vmath::vec3> temp_normals;

    FILE *file = new FILE;
    if (fopen_s(&file, path, "r"))
    {
        printf("Impossible to open the file !\n");
        return false;
    }

    char lineHeader[128];

    while (fscanf_s(file, "%s", lineHeader, 128) != EOF)
    {
        float x, y, z;

        if (strcmp(lineHeader, "v") == 0)
        {
            fscanf_s(file, "%f %f %f\n", &x, &y, &z);
            vmath::vec3 vertex(x, y, z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0)
        {
            fscanf_s(file, "%f %f\n", &x, &y);
            vmath::vec2 uv(x, y);
            temp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0)
        {
            fscanf_s(file, "%f %f %f\n", &x, &y, &z);
            vmath::vec3 normal(x, y, z);
            temp_normals.push_back(normal);
        }
        else if (strcmp(lineHeader, "f") == 0)
        {
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9)
            {
                printf("File can't be read by our simple parser : (Try exporting with other options\n");
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
    }

    // For each vertex of each triangle
    for (unsigned int i = 0; i < vertexIndices.size(); i++)
    {
        unsigned int vertexIndex = vertexIndices[i];
        vmath::vec3 vertex = temp_vertices[vertexIndex - 1];
        out_vertices.push_back(vertex);
    }

    // uv
    for (unsigned int i = 0; i < uvIndices.size(); i++)
    {
        unsigned int uvIndex = uvIndices[i];
        vmath::vec2 uv = temp_uvs[uvIndex - 1];
        out_uvs.push_back(uv);
    }

    // normal
    for (unsigned int i = 0; i < normalIndices.size(); i++)
    {
        unsigned int normalIndex = normalIndices[i];
        vmath::vec3 normal = temp_normals[normalIndex - 1];
        out_normals.push_back(normal);
    }
}
#endif
