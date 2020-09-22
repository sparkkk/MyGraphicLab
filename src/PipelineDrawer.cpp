#include "PipelineDrawer.h"
#include "DescLoader.h"

using namespace sunty;

void PipelineDrawer::init(const Starter & config)
{
	DescLoader loader;
	loader.searchPaths.emplace_back("../materials");
	if (!loader.loadScene(config.scene, config, mScene))
	{
		printf("failed to load scene: %s\n", config.scene.c_str());
		return;
	}
    if (!loader.loadPipeline(config.pipeline, config, mPipeline))
    {
		printf("failed to load pipeline: %s\n", config.pipeline.c_str());
        return;
    }

    for (Pass pass : mPipeline.passes)
    {
        mScene.setCurrentPass(pass);
        mScene.setup();
    }

    for (auto & link : mPipeline.links)
    {
        UniformValue value;
        get(link.src, value) && set(link.dst, value);
    }
}

void PipelineDrawer::update(float delta)
{
    for (Pass pass : mPipeline.passes)
    {
        mScene.setCurrentPass(pass);
	    mScene.update(delta);
    }
}

void PipelineDrawer::draw()
{
    for (int i = 0; i < mPipeline.passes.size(); ++i)
    {
        Pass pass = mPipeline.passes[i];
        auto rt = mPipeline.renderTargets[i];
        rt->push();
        rt->clear();
        mScene.setCurrentPass(pass);
        mScene.draw();
    }
}

void PipelineDrawer::release()
{
}


std::shared_ptr<Texture> PipelineDrawer::texture()
{
    return mOutput;
}

static std::vector<std::string> split(const std::string & text, const std::string & sep)
{
    std::vector<std::string> res;
    size_t p = 0;
    size_t q = 0;
    q = text.find(sep, p);
    while (q != std::string::npos)
    {
        res.emplace_back(text.substr(p, q - p));
        p = q + sep.size();
        q = text.find(sep, p);
    }
    res.emplace_back(text.substr(p));
    return res;
}

static std::string join(const std::vector<std::string> strs, const std::string & sep, size_t offset = 0)
{
    std::string res;
    for (size_t i = offset; i < strs.size(); ++i)
    {
        res += strs[i];
        if (i < strs.size() - 1)
        {
            res += sep;
        }
    }
    return res;
}

static std::unordered_map<std::string, Pass> sPassMap =
{
    { "default", PASS_DEFAULT },
    { "geometry", PASS_GEOMETRY },
    { "lighting", PASS_LIGHTING },
    { "depth1", PASS_DEPTH_1 },
};

bool PipelineDrawer::get(const std::string & id, UniformValue & value)
{
    auto ids = split(id, ".");
    if (ids.size() < 1)
    {
        return false;
    }
    if (sPassMap.find(ids[0]) == sPassMap.end())
    {
        return false;
    }
    Pass pass = sPassMap[ids[0]];
    if (ids[1] == "texture")
    {
        if (ids[2] == "depth")
        {
            value = mPipeline.getRenderTargetByPass(pass)->depthTexture();
            return true;
        }
        else
        {
            int index = std::atoi(ids[2].c_str());
            value = mPipeline.getRenderTargetByPass(pass)->texture(index);
            return true;
        }
    }
    else if (ids[1] == "camera")
    {
        if (ids[2] == "vp")
        {
            value = mScene.getCameraByPass(pass)->getVP();
            return true;
        }
        else if (ids[2] == "pos")
        {
            value = mScene.getCameraByPass(pass)->position;
            return true;
        }
    }
    return false;
}

bool PipelineDrawer::set(const std::string & id, const UniformValue & value)
{
    auto ids = split(id, ".");
    if (ids.size() < 1)
    {
        return false;
    }
    if (ids[0] == "output")
    {
        mOutput = value.texture;
        return true;
    }
    if (sPassMap.find(ids[0]) == sPassMap.end())
    {
        return false;
    }
    Pass pass = sPassMap[ids[0]];
    std::string alias = join(ids, ".", 1);
    mScene.setCurrentPass(pass);
    for (auto & r : mScene.renders)
    {
        if (r->getCurrentPass() == pass)
        {
            r->setParam(alias.c_str(), value);
        }
    }
    return true;
}