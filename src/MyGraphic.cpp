// MyGraphic.cpp : Defines the entry point for the console application.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>

#include "Texture.h"
#include "Scene.h"
#include "RenderTarget.h"

#include "PipelineDrawer.h"

#include "DescLoader.h"

#include "ImageUtils.h"

using namespace sunty;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
static void window_size_callback(GLFWwindow* window, int width, int height);

class MyApp
{
public:
    void run()
    {
        if (!initWindow())
        {
            return;
        }
        initGL();
        mainLoop();
        cleanUp();
    }
    void shouldClose()
    {
        if (mWindow != NULL)
        {
            glfwSetWindowShouldClose(mWindow, 1);
        }
    }
protected:
    bool initWindow()
    {
        DescLoader loader;
        if (!loader.loadStarter("../starter.json", mStarter))
        {
            printf("failed to load starter\n");
            return false;
        }
        glfwInit();
        glfwSetErrorCallback(error_callback);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

        mWindow = glfwCreateWindow(
            mStarter.width, 
            mStarter.height, 
            "MyGraphic", 
            nullptr, 
            nullptr
        );
        if (mWindow == nullptr)
        {
            printf("failed to create glfw window\n");
            return false;
        }
        glfwMakeContextCurrent(mWindow);
        glfwSetWindowSizeCallback(mWindow, window_size_callback);
        glfwSwapInterval(0);

        GLenum err = glewInit();
        if (err != GLEW_OK)
        {
            printf("glewInit failed: %u\n", err);
            return false;
        }
        return true;
    }
    bool initGL()
    {
        GLint maxDrawBuffers = 0;
        GLint maxColorAttachments = 0;
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffers);
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
        printf("GL_MAX_DRAW_BUFFERS: %d\n", maxDrawBuffers);
        printf("GL_MAX_COLOR_ATTACHMENTS: %d\n", maxColorAttachments);

        std::filesystem::path searchRoot = "../materials/include";
        for (auto & entry : std::filesystem::recursive_directory_iterator(searchRoot))
        {
            if (!entry.is_regular_file())
            {
                continue;
            }
            if (entry.path().extension() != ".glsl")
            {
                continue;
            }
            std::string text;
            if (loadText(entry.path(), text))
            {
                auto relativePath = entry.path().lexically_relative(searchRoot);
                std::string name = std::string("/") + relativePath.string();
                glNamedStringARB(
                    GL_SHADER_INCLUDE_ARB,
                    name.size(),
                    name.c_str(), 
                    text.size(),
                    text.c_str()
                );
            }
        }

        switch (mStarter.drawerType)
        {
        case DRAWER_TYPE_PIPELINE:
            mDrawer.reset(new PipelineDrawer);
            break;
        default:
            printf("invalid drawer type\n");
            return false;
        }
        
        mDrawer->init(mStarter);

        DescLoader loader;
        loader.searchPaths.emplace_back("../materials");
        loader.loadScene(
            "canvas/scene.json",
            mStarter,
            PASS_DEFAULT,
            mCanvasScene
        );
        mCanvasScene.setCurrentPass(PASS_DEFAULT);
        mCanvasScene.setup();
        //auto ttt = std::make_shared<Texture>();
        //loadPngTexture("../materials/resource/castle.png", "RGB", *ttt);
        mCanvasScene.renders.back()->setParam(
            "Texture1",
            mDrawer->texture()
            //ttt
            //mDrawer->renderTarget()->depthTexture()
        );

        RenderTarget::Options windowOptions;
        windowOptions.externalFBO = true;
        windowOptions.externalFBOID = 0;
        windowOptions.textureCount = 0;
        windowOptions.viewX = 0;
        windowOptions.viewY = 0;
        windowOptions.viewW = mStarter.width;
        windowOptions.viewH = mStarter.height;
        windowOptions.clearColor = glm::vec4(0.5f, 0.5f, 0.5f, 1);
        windowOptions.clearDepth = 1;
        windowOptions.hasDepth = false;

        mWindowRT.reset(new RenderTarget);
        mWindowRT->setup(windowOptions);

        //default fbo must clear first
        //otherwise nothing would be drawn
        //and i don't know why
        mWindowRT->push();
        mWindowRT->clear();

        return true;
    }
    void mainLoop()
    {
        std::chrono::milliseconds interval(1000/mStarter.fps);
        auto startTime = std::chrono::steady_clock::now();
        auto lastDrawnTime = startTime;
        while (!glfwWindowShouldClose(mWindow)) {
            auto currTime = std::chrono::steady_clock::now();
            auto delta = currTime - lastDrawnTime;
            if (delta < interval)
            {
                std::this_thread::sleep_for(interval - delta);
            }
            currTime = std::chrono::steady_clock::now();
            delta = currTime - startTime;
            lastDrawnTime = currTime;
            drawFrame(delta);
            glfwSwapBuffers(mWindow);
            glfwPollEvents();
        }
    }
    void cleanUp()
    {
        mDrawer.reset();
        mWindowRT.reset();
        glfwDestroyWindow(mWindow);
        glfwTerminate();
    }
    void drawFrame(const std::chrono::steady_clock::duration & dur)
    {
        float delta = (float) std::chrono::duration_cast<std::chrono::milliseconds>(dur).count() * 0.001;
        //printf("delta is %f sec\n", delta);
        mDrawer->update(delta);
        mDrawer->draw();

        mWindowRT->push();
        mWindowRT->clear();
        mCanvasScene.draw();
    }
    void checkError()
    {
        auto err = glGetError();
        auto errStr = glewGetErrorString(err);
        printf("%s\n", errStr);
    }
public:
    Starter mStarter;
    GLFWwindow * mWindow = nullptr;
    Scene mCanvasScene;
    std::shared_ptr<RenderTarget> mWindowRT;
    std::shared_ptr<IDrawer> mDrawer;
};

MyApp app;

static void window_size_callback(GLFWwindow* window, int width, int height)
{
    float ASPECT = (float) app.mStarter.width / (float) app.mStarter.height;
    float aspect = (float)width / (float)height;
    bool alignWidth = aspect <= ASPECT;
    int viewW = alignWidth ? width : (int)roundf(height * ASPECT);
    int viewH = alignWidth ? (int)roundf(width / ASPECT) : height;
    int viewX = (width - viewW) / 2;
    int viewY = (height - viewH) / 2;

    //app.mWindowRT->options().W = width;
    //app.mWindowRT->options().H = height;
    app.mWindowRT->options().viewX = viewX;
    app.mWindowRT->options().viewY = viewY;
    app.mWindowRT->options().viewW = viewW;
    app.mWindowRT->options().viewH = viewH;
}

int main()
{
    app.run();
    return 0;
}
