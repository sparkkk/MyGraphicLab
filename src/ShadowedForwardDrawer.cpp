#include "ShadowedForwardDrawer.h"
#include "DescLoader.h"

using namespace sunty;


void sunty::ShadowedForwardDrawer::init(const Starter & config)
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

	mScene.setCurrentPass(PASS_DEPTH_1);
	mScene.setup();
	mScene.setCurrentPass(PASS_DEFAULT);
	mScene.setup();

	int shadowW = 1000;
	int shadowH = 1000;
    RenderTarget::Options depthOptions;
    depthOptions.externalFBO = false;
    depthOptions.externalFBOID = 0;
    depthOptions.textureCount = 0;
    depthOptions.hasDepth = true;
	depthOptions.depthOption.width = shadowW;
	depthOptions.depthOption.height = shadowH;
	depthOptions.depthOption.pixelLayout = Texture::PL_DEPTH;
	depthOptions.depthOption.inputDataType = Texture::DT_F32;
	depthOptions.viewX = 0;
	depthOptions.viewY = 0;
	depthOptions.viewW = shadowW;
	depthOptions.viewH = shadowH;
	depthOptions.clearColor = glm::vec4(1, 1, 1, 1);
	depthOptions.clearDepth = 1.0f;

    mDepthRT.reset(new RenderTarget);
    mDepthRT->setup(depthOptions);
    mDepthRT->push();
    mDepthRT->clear();

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

    Camera * pCamera = nullptr;
    for (auto & c : mScene.cameras)
    {
        if (c.pass & PASS_DEPTH_1)
        {
            pCamera = &c;
            break;
        }
    }
    for (auto & r : mScene.renders)
    {
        if (r->getPassMask() & PASS_DEFAULT)
        {
            r->setParam("LightVP", pCamera->getVP());
            r->setParam("DepthMap", mDepthRT->depthTexture());
        }
    }
}

void ShadowedForwardDrawer::update(float delta)
{
    mScene.setCurrentPass(PASS_DEPTH_1);
    mScene.update(delta);
    mScene.setCurrentPass(PASS_DEFAULT);
	mScene.update(delta);
}

void ShadowedForwardDrawer::draw()
{
	mDepthRT->push();
	mDepthRT->clear();
    mScene.setCurrentPass(PASS_DEPTH_1);
	mScene.draw();

	mCanvasRT->push();
	mCanvasRT->clear();
    mScene.setCurrentPass(PASS_DEFAULT);
	mScene.draw();
}

void ShadowedForwardDrawer::release()
{
    mDepthRT.reset();
	mCanvasRT.reset();
}


std::shared_ptr<Texture> ShadowedForwardDrawer::texture()
{
	return mCanvasRT->texture(0);
}
