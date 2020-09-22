#include "SimpleForwardDrawer.h"
#include "DescLoader.h"

using namespace sunty;

SimpleForwardDrawer::SimpleForwardDrawer()
{
}


SimpleForwardDrawer::~SimpleForwardDrawer()
{
    mCanvasRT.reset();
}

void sunty::SimpleForwardDrawer::init(const Starter & config)
{
    int width = config.width;
    int height = config.height;

    DescLoader loader;
    loader.searchPaths.emplace_back("../materials");
    if (!loader.loadScene(config.scene, config, mScene))
    {
        printf("failed to load scene: %s\n", config.scene.c_str());
        return;
    }

    mScene.setCurrentPass(PASS_DEFAULT);
    mScene.setup();

    RenderTarget::Options canvasOptions;
    canvasOptions.externalFBO = false;
    canvasOptions.externalFBOID = 0;
    canvasOptions.textureCount = 1;
    canvasOptions.textureOptions[0].width = width;
    canvasOptions.textureOptions[0].height = height;
    canvasOptions.textureOptions[0].pixelLayout = Texture::PL_RGB;
    canvasOptions.textureOptions[0].internalDataType = Texture::DT_F16;
    canvasOptions.viewX = 0;
    canvasOptions.viewY = 0;
    canvasOptions.viewW = width;
    canvasOptions.viewH = height;
    canvasOptions.clearColor = glm::vec4(0, 0, 0, 1);
    canvasOptions.clearDepth = 1.0f;
    canvasOptions.hasDepth = true;
    canvasOptions.depthOption.width = width;
    canvasOptions.depthOption.height = height;
    canvasOptions.depthOption.pixelLayout = Texture::PL_DEPTH;
    canvasOptions.depthOption.inputDataType = Texture::DT_F32;

    mCanvasRT.reset(new RenderTarget);
    mCanvasRT->setup(canvasOptions);

    mCanvasRT->push();
    mCanvasRT->clear();
}

void SimpleForwardDrawer::update(float delta)
{
    mScene.update(delta);
}

void sunty::SimpleForwardDrawer::draw()
{
    mCanvasRT->push();
    mCanvasRT->clear();

    mScene.draw();
}

void sunty::SimpleForwardDrawer::release()
{
    mCanvasRT.reset();
}


std::shared_ptr<Texture> sunty::SimpleForwardDrawer::texture()
{
    return mCanvasRT->texture(0);
    //return mCanvasRT->depthTexture();
}
