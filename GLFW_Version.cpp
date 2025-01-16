#include<iostream>
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Angel.h"
#include "Sphere.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <chrono>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Sphere sun(0.2f);
Sphere spaceshipSphere(0.3);
Sphere fuelTankSphere(0.3);

Shader ourShader;
Model ourModel, Astreoid, fuelTankModel;


typedef Angel::vec4 color4;
typedef Angel::vec4 point4;


GLuint program, skyboxShader, vao, vbo1, vbo2, ibo1, ibo2, skyboxVAO, skyboxVBO, vPosition, vNormal, vTexture, modelUnifrom, viewUniform, projectionUniform, cameraUniform, earthTexture, sunTexture, mercuryTexture,
venusTexture, marsTexture, jupiterTexture, saturnTexture, uruansTexture, neptunTexture, skyboxTexture, spaceShipTexture, fuelTankTexture, astroidTextur;


glm::mat4 model, view, projection;

float mercuryRotation, venusRotation, earthRotation, marsRotation, jupiterRotation, saturnRotation, uransRotation, neptuneRotation;

float spcaeshipFuelTank = 300.f;


glm::mat4 trans;
float translate = 0.f, scale = 0.f, rotating = 0.f;
float factor = 0.005;



float spaceshipFuel = 100;

float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

std::vector<std::string> faces = {
    //"bkg1_right.png",
    //"bkg1_left.png",
    //"bkg1_top.png",
    //"bkg1_bot.png",
    //"bkg1_front.png",
    //"bkg1_back.png"

    "space_rt.png", //right
    "space_lf.png",//left
    "space_up.png",//top
    "space_dn.png",//bottom
    "space_ft.png",//front
    "space_bk.png"//back
};


unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


void initLight()
{
    // Define light properties
    point4 light_direction1(0.0f, 0.0f, 0.0f, 1.0f); // Direction for directional light
    point4 light_direction2(1.0f, 0.0f, 0.0f, 1.0f); // Position for first point light (right side of the sun)
    point4 light_direction3(-1.0f, 0.0f, 0.0f, 1.0f); // Position for second point light (left side of the sun)

    color4 light_ambient(0.2f, 0.2f, 0.2f, 1.0f);
    color4 light_diffuse(1.0f, 1.0f, 0.0f, 1.0f);
    color4 light_specular(1.0f, 1.0f, 1.0f, 1.0f);

    float material_shininess = 10.0f;
    color4 material_ambient(0.5f, 0.5f, 0.5f, 1.0f);
    color4 material_diffuse(1.0f, 1.0f, 1.0f, 1.0f);
    color4 material_specular(1.0f, 1.0f, 1.0f, 1.0f);

    float light_constant = 1.0f;
    float light_linear = 0.09f;
    float light_quadratic = 0.032f;

    // Set properties for first point light
    glUniform1f(glGetUniformLocation(program, "pointLights[0].constant"), light_constant);
    glUniform1f(glGetUniformLocation(program, "pointLights[0].linear"), light_linear);
    glUniform1f(glGetUniformLocation(program, "pointLights[0].quadratic"), light_quadratic);
    glUniform4fv(glGetUniformLocation(program, "pointLights[0].ambient"), 1, light_ambient);
    glUniform4fv(glGetUniformLocation(program, "pointLights[0].diffuse"), 1, light_diffuse);
    glUniform4fv(glGetUniformLocation(program, "pointLights[0].specular"), 1, light_specular);
    glUniform4fv(glGetUniformLocation(program, "pointLights[0].position"), 1, light_direction2);

    // Set properties for second point light
    glUniform1f(glGetUniformLocation(program, "pointLights[1].constant"), light_constant);
    glUniform1f(glGetUniformLocation(program, "pointLights[1].linear"), light_linear);
    glUniform1f(glGetUniformLocation(program, "pointLights[1].quadratic"), light_quadratic);
    glUniform4fv(glGetUniformLocation(program, "pointLights[1].ambient"), 1, light_ambient);
    glUniform4fv(glGetUniformLocation(program, "pointLights[1].diffuse"), 1, light_diffuse);
    glUniform4fv(glGetUniformLocation(program, "pointLights[1].specular"), 1, light_specular);
    glUniform4fv(glGetUniformLocation(program, "pointLights[1].position"), 1, light_direction3);

    // Set material properties for the planets
    glUniform4fv(glGetUniformLocation(program, "material.ambient"), 1, material_ambient);
    glUniform4fv(glGetUniformLocation(program, "material.diffuse"), 1, material_diffuse);
    glUniform4fv(glGetUniformLocation(program, "material.specular"), 1, material_specular);
    glUniform1f(glGetUniformLocation(program, "material.shininess"), material_shininess);
}

void initTexture()
{
    stbi_set_flip_vertically_on_load(true);

    glGenTextures(1, &earthTexture);
    glBindTexture(GL_TEXTURE_2D, earthTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, channels;
    unsigned char* data1 = stbi_load("earth2048.bmp", &width, &height, &channels, 0);

    if (data1)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data1);

    glGenTextures(1, &sunTexture);
    glBindTexture(GL_TEXTURE_2D, sunTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data2 = stbi_load("8k_sun.jpg", &width, &height, &channels, 0);

    if (data2)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data2);

    glGenTextures(1, &mercuryTexture);
    glBindTexture(GL_TEXTURE_2D, mercuryTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data3 = stbi_load("8k_mercury.jpg", &width, &height, &channels, 0);

    if (data3)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data3);

    glGenTextures(1, &venusTexture);
    glBindTexture(GL_TEXTURE_2D, venusTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data4 = stbi_load("8k_venus_surface.jpg", &width, &height, &channels, 0);

    if (data4)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data4);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data4);

    glGenTextures(1, &marsTexture);
    glBindTexture(GL_TEXTURE_2D, marsTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data5 = stbi_load("8k_mars.jpg", &width, &height, &channels, 0);

    if (data5)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data5);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data5);

    glGenTextures(1, &jupiterTexture);
    glBindTexture(GL_TEXTURE_2D, jupiterTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data6 = stbi_load("8k_jupiter.jpg", &width, &height, &channels, 0);

    if (data6)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data6);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data6);

    glGenTextures(1, &saturnTexture);
    glBindTexture(GL_TEXTURE_2D, saturnTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data7 = stbi_load("8k_saturn.jpg", &width, &height, &channels, 0);

    if (data7)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data7);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data7);

    glGenTextures(1, &uruansTexture);
    glBindTexture(GL_TEXTURE_2D, uruansTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data8 = stbi_load("2k_uranus.jpg", &width, &height, &channels, 0);

    if (data8)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data8);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data8);

    glGenTextures(1, &neptunTexture);
    glBindTexture(GL_TEXTURE_2D, neptunTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data9 = stbi_load("2k_neptune.jpg", &width, &height, &channels, 0);

    if (data9)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data9);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data9);

    //spaceship
    glGenTextures(1, &spaceShipTexture);
    glBindTexture(GL_TEXTURE_2D, spaceShipTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data10 = stbi_load("dark-blue-water.jpg", &width, &height, &channels, 0);

    if (data10)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data10);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data10);

    //fuel tank tex
    glGenTextures(1, &fuelTankTexture);
    glBindTexture(GL_TEXTURE_2D, fuelTankTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data11  = stbi_load("barreltex.png", &width, &height, &channels, 0);
    if (data11) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data11);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data11);

    //astroid tex
    glGenTextures(1, &astroidTextur);
    glBindTexture(GL_TEXTURE_2D, astroidTextur);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data12 = stbi_load("rock.png", &width, &height, &channels, 0);
    if (data12) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data12);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data12);


}

void init()
{
    //// Load shaders and use the resulting shader program
    skyboxShader = InitShader("skybox_vshader.glsl", "skybox_fshader.glsl");
    glUseProgram(skyboxShader);

    // Skybox VAO
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    skyboxTexture = loadCubemap(faces);

    glUseProgram(0);
    
    // Create a vertex array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create and initialize a vertex buffer object
    glGenBuffers(1, &vbo1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo1);
    glBufferData(GL_ARRAY_BUFFER, sun.getInterleavedVertexSize(), sun.getInterleavedVertices(), GL_STATIC_DRAW);

    // Create and initialize an index buffer object
    glGenBuffers(1, &ibo1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sun.getIndexSize(), sun.getIndices(), GL_STATIC_DRAW);

    // Load shaders and use the resulting shader program
    program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    // set up vertex arrays
    vPosition = glGetAttribLocation(program, "vertexPosition");
    glEnableVertexAttribArray(vPosition);

    vNormal = glGetAttribLocation(program, "vertexNormal");
    glEnableVertexAttribArray(vNormal);

    vTexture = glGetAttribLocation(program, "vertexTexture");
    glEnableVertexAttribArray(vTexture);

    initLight();
    initTexture();

    modelUnifrom = glGetUniformLocation(program, "model");
    viewUniform = glGetUniformLocation(program, "view");
    projectionUniform = glGetUniformLocation(program, "projection");

    projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, glm::value_ptr(projection));

    // don't forget to enable shader before setting uniforms
    ourShader.use();

    projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    // view/projection transformations
    view = camera.GetViewMatrix();
    ourShader.setMat4("projection", projection);
    ourShader.setMat4("view", view);

    glDisable(GL_DEPTH_TEST);

    // render the spaceship loaded model
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.f));
    model = glm::scale(model, glm::vec3(0.065f, 0.065f, 0.065f));	
    ourShader.setMat4("model", model);
    glEnable(GL_DEPTH_TEST | GL_TEXTURE_2D);
    glClearColor(1.0, 1.0, 1.0, 1.0);

}


void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get the view matrix from the camera
    view = camera.GetViewMatrix();

    // Remove translation from the view matrix for the skybox
    glm::mat4 skyboxView = glm::mat4(glm::mat3(view)); // Remove translation

    glDisable(GL_DEPTH_TEST); // disabling the depth test for the skybox
    // Render the skybox
    glUseProgram(skyboxShader);
    // Remove translation from the view matrix to keep the skybox centered around the camera
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "view"), 1, GL_FALSE, glm::value_ptr(glm::mat4(glm::mat3(view))));
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
    trans = glm::mat4(1.0f);
    trans = glm::scale(trans, glm::vec3(.5f, .5f, .5f)); // Scale the skybox
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "model"), 1, GL_FALSE, glm::value_ptr(trans));
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glEnable(GL_DEPTH_TEST); // enabnling the depth test for the rest of the planets

    // Set view matrix for rendering objects
    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));


    glUniformMatrix4fv(viewUniform, 1, GL_FALSE, glm::value_ptr(view));
    glUniform3fv(cameraUniform, 1, glm::value_ptr(camera.Position));

    // Draw Sun
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo1);
    glBindTexture(GL_TEXTURE_2D, sunTexture);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sun.getInterleavedStride(), BUFFER_OFFSET(0));
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, false, sun.getInterleavedStride(), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(vTexture, 2, GL_FLOAT, false, sun.getInterleavedStride(), (void*)(6 * sizeof(float)));
    trans = glm::mat4(1.0f);
    trans = glm::scale(trans, glm::vec3(1.0f, 1.0f, 1.0f));
    trans = glm::rotate(trans, rotating, glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(modelUnifrom, 1, GL_FALSE, glm::value_ptr(trans));
    glDrawElements(GL_TRIANGLES, sun.getIndexCount(), GL_UNSIGNED_INT, (void*)0);


    //mercury
    glBindTexture(GL_TEXTURE_2D, mercuryTexture);
    trans = glm::mat4(0.7f);
    trans = glm::rotate(trans, mercuryRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    trans = glm::scale(trans, glm::vec3(0.15f, 0.15f, 0.15f));
    trans = glm::translate(trans, glm::vec3(2.3f, 0.0f, 0.0f));
    trans = glm::rotate(trans, mercuryRotation, glm::vec3(0.0, 1.0, 0.0));
    // Send transformation matrix to the shader
    glUniformMatrix4fv(modelUnifrom, 1, GL_FALSE, glm::value_ptr(trans));
    glDrawElements(GL_TRIANGLES, sun.getIndexCount(), GL_UNSIGNED_INT, (void*)0);

    //draw venus
    glBindTexture(GL_TEXTURE_2D, venusTexture);
    trans = glm::mat4(0.7f);
    trans = glm::rotate(trans, venusRotation, glm::vec3(0.0, 1.0, 0.0));
    trans = glm::scale(trans, glm::vec3(0.18f, 0.18f, 0.18f));
    trans = glm::translate(trans, glm::vec3(2.6f, 0.f, 0.f));
    trans = glm::rotate(trans, venusRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    // Send transformation matrix to the shader
    glUniformMatrix4fv(modelUnifrom, 1, GL_FALSE, glm::value_ptr(trans));
    glDrawElements(GL_TRIANGLES, sun.getIndexCount(), GL_UNSIGNED_INT, (void*)0);

    //draw earth
    glBindTexture(GL_TEXTURE_2D, earthTexture);
    trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, earthRotation, glm::vec3(0.0, 1.0, 0.0));
    trans = glm::scale(trans, glm::vec3(0.19f, 0.19f, 0.19f));
    trans = glm::translate(trans, glm::vec3(2.3f, 0.f, 0.f));
    trans = glm::rotate(trans, earthRotation, glm::vec3(0.0, 1.0, 0.0));
    // Send transformation matrix to the shader
    glUniformMatrix4fv(modelUnifrom, 1, GL_FALSE, glm::value_ptr(trans));
    glDrawElements(GL_TRIANGLES, sun.getIndexCount(), GL_UNSIGNED_INT, (void*)0);

    //draw Mars
    glBindTexture(GL_TEXTURE_2D, marsTexture);
    trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, marsRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    trans = glm::scale(trans, glm::vec3(0.2f, 0.2f, 0.2f));
    trans = glm::translate(trans, glm::vec3(2.8f, 0.f, 0.f));
    trans = glm::rotate(trans, marsRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    // Send transformation matrix to the shader
    glUniformMatrix4fv(modelUnifrom, 1, GL_FALSE, glm::value_ptr(trans));
    glDrawElements(GL_TRIANGLES, sun.getIndexCount(), GL_UNSIGNED_INT, (void*)0);


    //Draw Jupiter
    glBindTexture(GL_TEXTURE_2D, jupiterTexture);
    trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, jupiterRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    trans = glm::scale(trans, glm::vec3(0.40f, 0.40f, 0.40f));
    trans = glm::translate(trans, glm::vec3(1.8f, 0.f, 0.f));
    trans = glm::rotate(trans, jupiterRotation, glm::vec3(0.0, 1.0, 0.0));
    // Send transformation matrix to the shader
    glUniformMatrix4fv(modelUnifrom, 1, GL_FALSE, glm::value_ptr(trans));
    glDrawElements(GL_TRIANGLES, sun.getIndexCount(), GL_UNSIGNED_INT, (void*)0);

    //Draw Saturn
    glBindTexture(GL_TEXTURE_2D, saturnTexture);
    trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, saturnRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    trans = glm::scale(trans, glm::vec3(0.40f, 0.40f, 0.40f));
    trans = glm::translate(trans, glm::vec3(2.3f, 0.f, 0.f));
    trans = glm::rotate(trans, saturnRotation, glm::vec3(0.0, 1.0, 0.0));
    // Send transformation matrix to the shader
    glUniformMatrix4fv(modelUnifrom, 1, GL_FALSE, glm::value_ptr(trans));
    glDrawElements(GL_TRIANGLES, sun.getIndexCount(), GL_UNSIGNED_INT, (void*)0);
    //Ring
    trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, saturnRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    trans = glm::scale(trans, glm::vec3(0.24f, 0.24f, 0.24f));
    trans = glm::translate(trans, glm::vec3(3.4f, 0.f, 0.f));
    trans = glm::rotate(trans, saturnRotation, glm::vec3(0.0, 1.0, 0.0));

    //Draw Uranus
    glBindTexture(GL_TEXTURE_2D, uruansTexture);
    trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, uransRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    trans = glm::scale(trans, glm::vec3(0.25f, 0.25f, 0.25f));
    trans = glm::translate(trans, glm::vec3(4.3f, 0.f, 0.f));
    trans = glm::rotate(trans, uransRotation, glm::vec3(0.0, 1.0, 0.0));
    // Send transformation matrix to the shader
    glUniformMatrix4fv(modelUnifrom, 1, GL_FALSE, glm::value_ptr(trans));
    glDrawElements(GL_TRIANGLES, sun.getIndexCount(), GL_UNSIGNED_INT, (void*)0);

    // //Draw Neptune
    glBindTexture(GL_TEXTURE_2D, neptunTexture);
    trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, neptuneRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    trans = glm::scale(trans, glm::vec3(0.20f, 0.20f, 0.20f));
    trans = glm::translate(trans, glm::vec3(5.4f, 0.f, 0.f));
    trans = glm::rotate(trans, neptuneRotation, glm::vec3(0.0, 1.0, 0.0));
    // Send transformation matrix to the shader
    glUniformMatrix4fv(modelUnifrom, 1, GL_FALSE, glm::value_ptr(trans));
    glDrawElements(GL_TRIANGLES, sun.getIndexCount(), GL_UNSIGNED_INT, (void*)0);

    ////draw a sphere to encounter the mspaceship and the fuel model
    glBindTexture(GL_TEXTURE_2D, uruansTexture);
    trans = glm::mat4(1.0f);
    glm::vec3 spaceshipPos = glm::vec3(0.0f, -3.0f, 0.0f);
    // trans = glm::rotate(trans, uransRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    trans = glm::scale(trans, glm::vec3(5.f, 5.f, 5.f));
    // trans = glm::rotate(trans, uransRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    trans = glm::scale(trans, glm::vec3(15.f, 15.f, 15.f));
    trans = glm::translate(trans, spaceshipPos);
    // trans = glm::rotate(trans, uransRotation, glm::vec3(0.0, 1.0, 0.0));
    // Send transformation matrix to the shader
    glUniformMatrix4fv(modelUnifrom, 1, GL_FALSE, glm::value_ptr(trans));
    glDrawElements(GL_TRIANGLES, spaceshipSphere.getIndexCount(), GL_UNSIGNED_INT, (void*)0);

    glBindTexture(GL_TEXTURE_2D, uruansTexture);
    trans = glm::mat4(1.0f);
    trans = glm::scale(trans, glm::vec3(5.f, 5.f, 5.f));
    trans = glm::translate(trans, glm::vec3(-9.0f, 0.0f, -3.f));
    // Send transformation matrix to the shader
    glUniformMatrix4fv(modelUnifrom, 1, GL_FALSE, glm::value_ptr(trans));
    glDrawElements(GL_TRIANGLES, fuelTankSphere.getIndexCount(), GL_UNSIGNED_INT, (void*)0);

    
    //enable depth test
    glEnable(GL_DEPTH_TEST); 
    ourShader.use(); 
    //draw the fueltank model
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(.5f, .5f, .5f));	
    model = glm::translate(model, glm::vec3(-3.8f, 0.0f, 0.f)); 
    ourShader.setMat4("model", model);
    ourShader.setMat4("view", view);
    glBindTexture(GL_TEXTURE_2D, fuelTankTexture);
    fuelTankModel.Draw(ourShader);
    
    //draw a second fuel tank model
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(3.8f, 0.0f, 0.f)); 
    ourShader.setMat4("model", model);
    fuelTankModel.Draw(ourShader);

   //draw the asteroids
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-2.0f, 0.05f, 0.0f));
    model = glm::scale(model, glm::vec3(.05f, .05f, .05f));	
    glBindTexture(GL_TEXTURE_2D, astroidTextur);
    ourShader.setMat4("model", model);
    Astreoid.Draw(ourShader);

    //draw a second debris 
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.05f, 0.0f));
    model = glm::scale(model, glm::vec3(.05f, .05f, .05f));
    ourShader.setMat4("model", model);
    Astreoid.Draw(ourShader);

    //draw a third debris 
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.05f, -2.0f));
    model = glm::scale(model, glm::vec3(.05f, .05f, .05f));
    ourShader.setMat4("model", model);
    Astreoid.Draw(ourShader);

    //draw a fourth debris 
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-2.0f, 0.05f, -2.0f));
    model = glm::scale(model, glm::vec3(.05f, .05f, .05f));
    ourShader.setMat4("model", model);
    Astreoid.Draw(ourShader);

    //draw a fourth debris 
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.05f, -3.0f));
    model = glm::scale(model, glm::vec3(.05f, .05f, .05f));
    ourShader.setMat4("model", model);
    Astreoid.Draw(ourShader);
    
    //draw a fifth debris 
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-0.5f, 0.05f, -3.0f));
    model = glm::scale(model, glm::vec3(.05f, .05f, .05f));
    ourShader.setMat4("model", model);
    Astreoid.Draw(ourShader);

    //debris
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-0.3f, 0.05f, -3.0f));
    model = glm::scale(model, glm::vec3(.05f, .05f, .05f));
    ourShader.setMat4("model", model);
    Astreoid.Draw(ourShader);

    //draw a sixth debris 
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.5f, 0.05f, -3.0f));
    model = glm::scale(model, glm::vec3(.05f, .05f, .05f));
    ourShader.setMat4("model", model);
    Astreoid.Draw(ourShader);

    //draw a seventh debris 
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.7f, 0.05f, -3.0f));
    model = glm::scale(model, glm::vec3(.05f, .05f, .05f));
    ourShader.setMat4("model", model);
    Astreoid.Draw(ourShader);

    //spaceship
    glBindTexture(GL_TEXTURE_2D, spaceShipTexture);
    //glm::vec3 spaceshipPos = glm::vec3(0.0f, -3.0f, 0.0f);

    // Adjusted camera offset: Closer and higher above the spaceship
    glm::vec3 cameraOffset = glm::vec3(0.0f, 4.0f, 0.5f); // Closer and higher

    //Calculate camera position
    view = glm::lookAt(spaceshipPos - glm::vec3(0.0f, 4.0f, 0.5f), spaceshipPos, glm::vec3(0.0f, 1.0f, 0.0f));

    model = glm::mat4(1.0f);
    model = glm::translate(model, spaceshipPos); // Position it at the center
    model = glm::scale(model, glm::vec3(0.065f, 0.065f, 0.065f)); // Scale the spaceship down

    // Apply a rotation to correct the orientation if the spaceship looks down
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate 90 degrees around the x-axis

    ourShader.setMat4("model", model);
    ourShader.setMat4("view", view);
    ourModel.Draw(ourShader);


    if (spaceshipSphere.isColliding(spaceshipPos, glm::vec3(-9.0f, 0.0f, -3.f), spaceshipSphere.getRadius(), fuelTankSphere.getRadius()))
    {
        std::cout << "collision between sphere " << endl;
    }
}

void idle() {

    if (translate > 1.0f) {
        factor = -0.005;
    }
    else if (translate < -1.0f) {
        factor = 0.005;
    }

        rotating += 0.005;

    mercuryRotation += 0.008;
    venusRotation += 0.012;
    earthRotation += 0.016;
    marsRotation += 0.020;
    jupiterRotation += 0.0025;
    saturnRotation += 0.0010;
    uransRotation += 0.0015;
    neptuneRotation += 0.0005;
}


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;

    }
    std::cerr << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;


    ourShader.InitShader("model_2.txt", "model_1.txt");
    fuelTankModel.InitModel("FuelCanister.obj");
    Astreoid.InitModel("Astreoid.obj");
    ourModel.InitModel("StarShip2.obj");


   Sphere s1(1.0f);
   Sphere s2(1.0f);
  
    init();

    while (!glfwWindowShouldClose(window))
    {

        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        static std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
        auto elapsedSeconds = elapsedMilliseconds / 1000.0;
        string elapsedTime = to_string(elapsedSeconds);
        processInput(window);
        if (spcaeshipFuelTank <= 0)
        {
            glfwSetWindowShouldClose(window, true);
            cout << "Game Over your score is: " << elapsedTime;
        }

        


        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    

               
              
        
   
 
        display();

        idle();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
        cout << "Pressed W" << endl;
        spcaeshipFuelTank -= 0.25;

    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
        cout << "Pressed S" << endl;
        spcaeshipFuelTank -= 0.25;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
        cout << "Pressed A" << endl;
        spcaeshipFuelTank -= 0.25;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
        cout << "Pressed D" << endl;
        spcaeshipFuelTank -= 0.25;
    }
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
    cout << "Mouse Moved" << endl;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

