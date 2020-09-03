// MyGraphic.cpp : Defines the entry point for the console application.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>

#include "Texture.h"
#include "Scene.h"
#include "RenderTarget.h"

#include "SimpleForwardDrawer.h"
#include "SimpleDeferredDrawer.h"

#include "DescLoader.h"

#include "ImageUtils.h"

using namespace sunty;


static int WIDTH = 800;
static int HEIGHT = 600;
static int FPS = 50;
static float ASPECT = (float)WIDTH / (float)HEIGHT;
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
		initWindow();
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
	void initWindow()
	{
		glfwInit();
		glfwSetErrorCallback(error_callback);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

		mWindow = glfwCreateWindow(WIDTH, HEIGHT, "MyGraphic", nullptr, nullptr);
		glfwMakeContextCurrent(mWindow);
		glfwSetWindowSizeCallback(mWindow, window_size_callback);
		glfwSwapInterval(0);

		GLenum err = glewInit();
	}
	void initGL()
	{
		mDrawer.reset(new SimpleForwardDrawer);

		IDrawer::Config config;
		config.width = WIDTH;
		config.height = HEIGHT;
		
		mDrawer->init(config);

		DescLoader loader;
		loader.loadScene(
			"../materials/canvas/scene.json",
			mCanvasScene
		);
		//auto ttt = std::make_shared<Texture>();
		//loadPngTexture("../materials/resource/castle.png", "RGB", *ttt);
		mCanvasScene.renders.back().setParam(
			"Texture1",
			mDrawer->renderTarget()->textures()[0]
			//ttt
		);

		RenderTarget::Options windowOptions;
		windowOptions.externalFBO = true;
		windowOptions.externalFBOID = 0;
		windowOptions.textureCount = 0;
		windowOptions.viewX = 0;
		windowOptions.viewY = 0;
		windowOptions.viewW = WIDTH;
		windowOptions.viewH = HEIGHT;
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
	}
	void mainLoop()
	{
		std::chrono::milliseconds interval(1000/FPS);
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
	GLFWwindow * mWindow = nullptr;
	Scene mCanvasScene;
	std::shared_ptr<RenderTarget> mWindowRT;
	std::shared_ptr<IDrawer> mDrawer;
};

MyApp app;

static void window_size_callback(GLFWwindow* window, int width, int height)
{
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
