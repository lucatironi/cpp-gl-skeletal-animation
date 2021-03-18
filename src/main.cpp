#include <string>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "model.hpp"
#include "shader.hpp"
#include "texture.hpp"

static void ProcessInput(GLFWwindow* window);
static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

static Model LoadModelFromFilename(const std::string& path);

// settings
const unsigned int WindowWidth  = 800;
const unsigned int WindowHeight = 600;

Model model;
uint currentAnimation = 0;
bool animationChanged = false;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "Skeletal Animation", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // setup OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    model = LoadModelFromFilename("../assets/zombie.fbx");
    Texture2D texture("../assets/zombie.png", GL_FALSE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
    Shader defaultShader("../src/shaders/default.vs", "../src/shaders/default.fs");

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        ProcessInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate texture
        glActiveTexture(GL_TEXTURE0);
        texture.Bind();

        // Prepare transformations matrices and uniforms
        defaultShader.Use();
        defaultShader.SetMatrix4("projection", glm::perspective(glm::radians(90.0f), static_cast<GLfloat>(WindowWidth) / static_cast<GLfloat>(WindowHeight), 0.1f, 100.0f));
        defaultShader.SetMatrix4("view", glm::lookAt(glm::vec3(0.0f, 6.0f, 8.0f), glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
        defaultShader.SetMatrix4("model", glm::mat4(1.0f));
        defaultShader.SetInteger("animated", model.HasAnimations());

        // Set model transformation and render the model
        model.SetBoneTransformations(defaultShader, currentFrame);
        model.Draw(defaultShader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

/// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
static void ProcessInput(GLFWwindow* window)
{
    // ESC closes the application
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

     if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !animationChanged)
    {
        if (currentAnimation < model.GetNumAnimations() - 1)
            currentAnimation++;
        else
            currentAnimation = 0;
        model.SetAnimation(currentAnimation);
        animationChanged = true;
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE)
        animationChanged = false;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void FramebufferSizeCallback(GLFWwindow* /* window */, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


Model LoadModelFromFilename(const std::string& path)
{
    Model model;
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_Fast | aiProcess_GlobalScale | aiProcess_LimitBoneWeights);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP: " << importer.GetErrorString() << std::endl;
    }
    else {
        // retrieve the directory path of the filepath
        model.SetDirectory(path.substr(0, path.find_last_of('/')));
        model.InitFromScene(importer.GetOrphanedScene());
    }
    return model;
}