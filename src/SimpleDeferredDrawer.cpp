#include "SimpleDeferredDrawer.h"
#include "DescLoader.h"

using namespace sunty;

SimpleDeferredDrawer::SimpleDeferredDrawer()
{
}


SimpleDeferredDrawer::~SimpleDeferredDrawer()
{
	release();
}

void sunty::SimpleDeferredDrawer::init(const Config & config)
{
	int width = config.width;
	int height = config.height;
	float aspect = (float)config.width / (float)config.height;

	DescLoader loader;
	if (!loader.loadScene(
		"materials/phong-deferred/scene-geometry-pass.json",
		mGeometryPassScene
	))
	{
		printf("failed to load geometry scene\n");
	}
	if (!loader.loadScene(
		"materials/phong-deferred/scene-lighting-pass.json",
		mLightingPassScene
	))
	{
		printf("failed to load lighting scene\n");
	}

	RenderTarget::Options GPRTOption;
	GPRTOption.externalFBO = false;
	GPRTOption.externalFBOID = 0;
	GPRTOption.textureCount = 6;
	for (int k = 0; k < 6; ++k)
	{
		GPRTOption.textureOptions[k].width = width;
		GPRTOption.textureOptions[k].height = height;
		GPRTOption.textureOptions[k].pixelLayout = Texture::PL_RGBA;
		GPRTOption.textureOptions[k].internalDataType = Texture::DT_F16;
	}
	GPRTOption.viewX = 0;
	GPRTOption.viewY = 0;
	GPRTOption.viewW = width;
	GPRTOption.viewH = height;
	GPRTOption.clearColor = glm::vec4(0, 0, 0, 0);
	GPRTOption.clearDepth = 1;
	GPRTOption.hasDepth = true;
	GPRTOption.depthOption.width = width;
	GPRTOption.depthOption.height = height;
	GPRTOption.depthOption.pixelLayout = Texture::PL_DEPTH;
	GPRTOption.depthOption.internalDataType = Texture::DT_BYTE;

	mGeometryPassRT.reset(new RenderTarget);
	mGeometryPassRT->setup(GPRTOption);


	RenderTarget::Options LPRTOption;
	LPRTOption.externalFBO = false;
	LPRTOption.externalFBOID = 0;
	LPRTOption.textureCount = 1;
	LPRTOption.textureOptions[0].width = width;
	LPRTOption.textureOptions[0].height = height;
	LPRTOption.textureOptions[0].pixelLayout = Texture::PL_RGB;
	LPRTOption.textureOptions[0].internalDataType = Texture::DT_F16;
	LPRTOption.viewX = 0;
	LPRTOption.viewY = 0;
	LPRTOption.viewW = width;
	LPRTOption.viewH = height;
	LPRTOption.clearColor = glm::vec4(1, 0, 0, 1);
	LPRTOption.clearDepth = 1;
	LPRTOption.hasDepth = false;

	mLightingPassRT.reset(new RenderTarget);
	mLightingPassRT->setup(LPRTOption);


	mLightingPassScene.renders.back().setParam("ViewerPosition", mGeometryPassScene.cameras.back().position);
	mLightingPassScene.renders.back().setParam("GBuffer.TextureWorldPosition", mGeometryPassRT->textures()[0]);
	mLightingPassScene.renders.back().setParam("GBuffer.TextureNormal", mGeometryPassRT->textures()[1]);
	mLightingPassScene.renders.back().setParam("GBuffer.TextureTangent", mGeometryPassRT->textures()[2]);
	mLightingPassScene.renders.back().setParam("GBuffer.TextureNormalMap", mGeometryPassRT->textures()[3]);
	mLightingPassScene.renders.back().setParam("GBuffer.TextureDiffuseMap", mGeometryPassRT->textures()[4]);
	mLightingPassScene.renders.back().setParam("GBuffer.TextureSpecularMap", mGeometryPassRT->textures()[5]);
}

void SimpleDeferredDrawer::update(float delta)
{
	mGeometryPassScene.update(delta);
	mLightingPassScene.update(delta);
}

void sunty::SimpleDeferredDrawer::draw()
{
	{
		mGeometryPassRT->push();
		mGeometryPassRT->clear();
		mGeometryPassScene.draw();
	}
	{
		mLightingPassRT->push();
		mLightingPassRT->clear();
		mLightingPassScene.draw();
	}
}

void sunty::SimpleDeferredDrawer::release()
{
	mGeometryPassRT.reset();
	mLightingPassRT.reset();
}

std::shared_ptr<RenderTarget> sunty::SimpleDeferredDrawer::renderTarget()
{
	return mLightingPassRT;
}
