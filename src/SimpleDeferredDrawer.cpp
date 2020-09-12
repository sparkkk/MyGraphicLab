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

void sunty::SimpleDeferredDrawer::init(const Starter & config)
{
	int width = config.width;
	int height = config.height;

	DescLoader loader;
	loader.searchPaths.emplace_back("../materials");
	if (!loader.loadScene(config.path, config, mScene))
	{
		printf("failed to load scene\n");
	}
	mScene.setCurrentPass(PASS_GEOMETRY);
	mScene.setup();
	mScene.setCurrentPass(PASS_LIGHTING);
	mScene.setup();

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
	LPRTOption.clearColor = glm::vec4(0, 0, 0, 1);
	LPRTOption.clearDepth = 1;
	LPRTOption.hasDepth = false;

	mLightingPassRT.reset(new RenderTarget);
	mLightingPassRT->setup(LPRTOption);

	Camera * geometryCamera = nullptr;
	for (auto & c : mScene.cameras)
	{
		if (c.pass & PASS_GEOMETRY)
		{
			geometryCamera = &c;
			break;
		}
	}

	for (auto render : mScene.renders)
	{
		if ((render->getPassMask() & PASS_LIGHTING) == 0)
		{
			continue;
		}
		render->setParam("ViewerPosition", geometryCamera->position);
		render->setParam("GBuffer.TextureWorldPosition", mGeometryPassRT->texture(0));
		render->setParam("GBuffer.TextureNormal", mGeometryPassRT->texture(1));
		render->setParam("GBuffer.TextureTangent", mGeometryPassRT->texture(2));
		render->setParam("GBuffer.TextureNormalMap", mGeometryPassRT->texture(3));
		render->setParam("GBuffer.TextureDiffuseMap", mGeometryPassRT->texture(4));
		render->setParam("GBuffer.TextureSpecularMap", mGeometryPassRT->texture(5));
	}
}

void SimpleDeferredDrawer::update(float delta)
{
	mScene.setCurrentPass(PASS_GEOMETRY);
	mScene.update(delta);
	mScene.setCurrentPass(PASS_LIGHTING);
	mScene.update(delta);
}

void sunty::SimpleDeferredDrawer::draw()
{
	{
		mGeometryPassRT->push();
		mGeometryPassRT->clear();
		mScene.setCurrentPass(PASS_GEOMETRY);
		mScene.draw();
	}
	{
		mLightingPassRT->push();
		mLightingPassRT->clear();
		mScene.setCurrentPass(PASS_LIGHTING);
		mScene.draw();
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
