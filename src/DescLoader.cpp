#include "DescLoader.h"
#include "nlohmann/json.hpp"
#include "RenderGroup.h"
#include "Context.h"
#include "Cubemap.h"

using namespace sunty;
using namespace nlohmann;

DescLoader::DescLoader()
{
}

DescLoader::~DescLoader()
{
}

static std::map<std::string, Pass> sPassMap =
{
    { "default", PASS_DEFAULT },
    { "geometry", PASS_GEOMETRY },
    { "lighting", PASS_LIGHTING },
};

static std::map<std::string, DrawerType> sDrawerTypeMap =
{
    { "forward", DRAWER_TYPE_FORWARD },
    { "deferred", DRAWER_TYPE_DEFERRED },
};

static Pass parsePass(const std::vector<std::string> & pass)
{
    Pass res = PASS_NONE;
    for (auto & p : pass)
    {
        res = (Pass) (res | sPassMap[p]);
    }
    return res;
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


#define JD_FROM_JSON(name) if (j.contains(#name)) { j.at(#name).get_to(jd.name); }

struct JDRenderOptions
{
    bool alphaBlend = false;
    bool depthTest = false;
    bool depthMask = false;
    std::string cullMode;
    std::string drawMode;
};

void from_json(const json& j, JDRenderOptions& jd) {
    JD_FROM_JSON(alphaBlend);
    JD_FROM_JSON(depthTest);
    JD_FROM_JSON(depthMask);
    JD_FROM_JSON(cullMode);
    JD_FROM_JSON(drawMode);
}

struct JDShader
{
    std::string type;
    std::string pathVert;
    std::string pathFrag;
};

void from_json(const json& j, JDShader& jd)
{
    JD_FROM_JSON(type);
    JD_FROM_JSON(pathVert);
    JD_FROM_JSON(pathFrag);
}

struct JDMesh
{
    std::string type;
    std::string path;
};

void from_json(const json& j, JDMesh& jd)
{
    JD_FROM_JSON(type);
    JD_FROM_JSON(path);
}

struct JDUniformItem
{
    std::string id;
    std::vector<std::string> value;
};

void from_json(const json& j, JDUniformItem& jd)
{
    JD_FROM_JSON(id);
    JD_FROM_JSON(value);
}

struct JDMaterial
{
    std::vector<JDUniformItem> uniforms;
};

void from_json(const json& j, JDMaterial& jd)
{
    JD_FROM_JSON(uniforms);
}

struct JDRender
{
    std::string version;
    JDRenderOptions options;
    JDShader shader;
    JDMesh mesh;
    JDMaterial material;
};

void from_json(const json& j, JDRender& jd)
{
    JD_FROM_JSON(version);
    JD_FROM_JSON(options);
    JD_FROM_JSON(shader);
    JD_FROM_JSON(mesh);
    JD_FROM_JSON(material);
}

struct JDLight
{
    std::string version;
    std::string type;
    std::vector<JDUniformItem> uniforms;
};

void from_json(const json& j, JDLight& jd)
{
    JD_FROM_JSON(version);
    JD_FROM_JSON(type);
    JD_FROM_JSON(uniforms);
}

struct JDCameraItem
{
    std::string type;
    std::vector<std::string> pass;
    float fan = 0;
    float width = 0;
    std::vector<float> range;
    std::vector<float> at;
    std::vector<float> look;
    std::vector<float> up;
    bool flipY = false;
};

void from_json(const json& j, JDCameraItem& jd)
{
    JD_FROM_JSON(type);
    JD_FROM_JSON(pass);
    JD_FROM_JSON(fan);
    JD_FROM_JSON(width);
    JD_FROM_JSON(range);
    JD_FROM_JSON(at);
    JD_FROM_JSON(look);
    JD_FROM_JSON(up);
    JD_FROM_JSON(flipY);
}

struct JDRenderGroupRenderItem
{
    std::string path;
    std::vector<std::string> pass;
};

void from_json(const json& j, JDRenderGroupRenderItem& jd)
{
    JD_FROM_JSON(path);
    JD_FROM_JSON(pass);
}

struct JDRenderGroupItem
{
    std::string name;
    std::vector<JDRenderGroupRenderItem> renders;
};

void from_json(const json& j, JDRenderGroupItem& jd)
{
    JD_FROM_JSON(name);
    JD_FROM_JSON(renders);
}

struct JDTransformItem
{
    std::string type;
    std::vector<std::string> value;
};

void from_json(const json& j, JDTransformItem& jd)
{
    JD_FROM_JSON(type);
    JD_FROM_JSON(value);
}

struct JDRenderItem
{
    std::string path;
    std::string group;
    std::vector<std::string> pass;
    bool follow = false;
    std::vector<JDTransformItem> transforms;
    std::vector<JDUniformItem> uniforms;
};

void from_json(const json& j, JDRenderItem& jd)
{
    JD_FROM_JSON(path);
    JD_FROM_JSON(group);
    JD_FROM_JSON(pass);
    JD_FROM_JSON(follow);
    JD_FROM_JSON(transforms);
    JD_FROM_JSON(uniforms);
}

struct JDLightItem
{
    std::string path;
    std::vector<JDUniformItem> uniforms;
};

void from_json(const json& j, JDLightItem& jd)
{
    JD_FROM_JSON(path);
    JD_FROM_JSON(uniforms);
}

struct JDScene
{
    std::string version;
    std::vector<JDCameraItem> cameras;
    std::vector<JDRenderGroupItem> renderGroups;
    std::vector<JDRenderItem> renders;
    std::vector<JDLightItem> lights;
};

void from_json(const json& j, JDScene& jd)
{
    JD_FROM_JSON(version);
    JD_FROM_JSON(cameras);
    JD_FROM_JSON(renderGroups);
    JD_FROM_JSON(renders);
    JD_FROM_JSON(lights);
}

struct ParseContext
{
    DescLoader * loader = nullptr;
    RenderOptions options;
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Mesh> mesh;
    std::unordered_map<std::string, UniformValue> uniforms;
    std::vector<Camera> cameras;
    std::unordered_map<std::string, JDRenderGroupItem> renderGroupMap;
    std::vector<std::shared_ptr<IRenderObject>> renders;
    std::vector<std::vector<Scene::Transform>> transforms;
    std::vector<Scene::Light> lights;
    std::vector<bool> follows;
    std::string errorMessage;
};

static bool parseRenderOptions(ParseContext & context, const JDRenderOptions & jd)
{
    static const std::map<std::string, decltype(RenderOptions::cullMode)> cullModeMap = {
        { "none", RenderOptions::CULL_NONE },
        { "back", RenderOptions::CULL_BACK },
        { "front", RenderOptions::CULL_FRONT },
    };
    static const std::map<std::string, decltype(RenderOptions::drawMode)> drawModeMap = {
        { "list", RenderOptions::DRAW_LIST },
        { "strip", RenderOptions::DRAW_STRIP },
        { "fan", RenderOptions::DRAW_FAN },
    };
    RenderOptions & renderOptions(context.options);
    renderOptions.alphaBlend = jd.alphaBlend;
    renderOptions.depthTest = jd.depthTest;
    renderOptions.depthMask = jd.depthMask;
    renderOptions.cullMode = cullModeMap.at(jd.cullMode);
    renderOptions.drawMode = drawModeMap.at(jd.drawMode);
    return true;
}

static bool parseShader(ParseContext & context, const JDShader & jd)
{
    auto shader = Context::get().resource().queryShader(
        context.loader->assembllyPath(jd.pathVert),
        context.loader->assembllyPath(jd.pathFrag)
    );
    if (shader == nullptr)
    {
        return false;
    }
    context.shader = std::static_pointer_cast<Shader>(shader);
    return true;
}

static bool parseMesh(ParseContext & context, const JDMesh & jd)
{
    context.mesh = std::make_shared<Mesh>();
    if (jd.type == "obj")
    {
        auto mesh = Context::get().resource().queryMesh(
            context.loader->assembllyPath(jd.path),
            true
        );
        if (mesh == nullptr)
        {
            return false;
        }
        context.mesh = std::static_pointer_cast<Mesh>(mesh);
        return true;
    }
    else
    {
        return false;
    }
}

static bool parseUniformValue(ParseContext & context, const std::vector<std::string> & list, UniformValue & value)
{
    typedef std::function<bool(void)> proc;
    std::map<std::string, proc> procs = {
        {
            "texture",
            [&]() -> bool {
                std::string type = list[1];
                if (type == "file")
                {
                    auto texture = Context::get().resource().queryTexture(
                        context.loader->assembllyPath(list[2]),
                        list[3].c_str()
                    );
                    if (texture == nullptr)
                    {
                        return false;
                    }
                    value.type = UniformValue::TYPE_TEXTURE;
                    value.texture = std::static_pointer_cast<Texture>(texture);
                    return true;
                }
                else if (type == "built-in")
                {
                    auto texture = std::make_shared<Texture>();
                    int color[3] = {0};
                    if (list[2] == "black")
                    {
                        color[0] = color[1] = color[2] = 0;
                    }
                    else if (list[2] == "white")
                    {
                        color[0] = color[1] = color[2] = 255;
                    }
                    else if (list[2] == "gray")
                    {
                        color[0] = color[1] = color[2] = 128;
                    }
                    else if (list[2] == "red")
                    {
                        color[0] = 255;
                        color[1] = color[2] = 0;
                    }
                    else if (list[2] == "green")
                    {
                        color[1] = 255;
                        color[0] = color[2] = 0;
                    }
                    else if (list[2] == "blue")
                    {
                        color[2] = 255;
                        color[0] = color[1] = 0;
                    }
                    else if (list[2] == "cyan")
                    {
                        color[0] = 0;
                        color[1] = color[2] = 255;
                    }
                    else if (list[2] == "magenta")
                    {
                        color[1] = 0;
                        color[0] = color[2] = 255;
                    }
                    else if (list[2] == "yellow")
                    {
                        color[2] = 0;
                        color[0] = color[1] = 255;
                    }
                    else
                    {
                        return false;
                    }
                    uint8_t buf[4*4*3];
                    uint8_t * p = buf;
                    for (int i = 0; i < 16; ++i)
                    {
                        p[0] = color[0];
                        p[1] = color[1];
                        p[2] = color[2];
                        p += 3;
                    }
                    Texture::Option option;
                    option.width = 4;
                    option.height = 4;
                    option.pixelLayout = Texture::PL_RGB;
                    option.internalDataType = Texture::DT_BYTE;
                    option.inputDataType = Texture::DT_BYTE;
                    option.data = buf;
                    texture->setup(option);
                    value.type = UniformValue::TYPE_TEXTURE;
                    value.texture = texture;
                    return true;
                }
                else if (type == "rgb")
                {
                    auto texture = std::make_shared<Texture>();
                    float color[3] = { 0 };
                    color[0] = (float) std::atof(list[2].c_str());
                    color[1] = (float) std::atof(list[3].c_str());
                    color[2] = (float) std::atof(list[4].c_str());
                    float buf[4 * 4 * 3];
                    float * p = buf;
                    for (int i = 0; i < 16; ++i)
                    {
                        p[0] = color[0];
                        p[1] = color[1];
                        p[2] = color[2];
                        p += 3;
                    }
                    Texture::Option option;
                    option.width = 4;
                    option.height = 4;
                    option.pixelLayout = Texture::PL_RGB;
                    option.internalDataType = Texture::DT_F16;
                    option.inputDataType = Texture::DT_F32;
                    option.data = buf;
                    texture->setup(option);
                    value.type = UniformValue::TYPE_TEXTURE;
                    value.texture = texture;
                    return true;
                }
                else
                {
                    return false;
                }
            }
        },
        {
            "cubemap",
            [&]() -> bool {
                std::string type = list[1];
                if (type == "file")
                {
                    auto cubemap = Context::get().resource().queryCubemap(
                        context.loader->assembllyPath(list[2]),
                        list[3].c_str()
                    );
                    if (cubemap == nullptr)
                    {
                        return false;
                    }
                    value.type = UniformValue::TYPE_TEXTURE;
                    value.texture = std::static_pointer_cast<Cubemap>(cubemap);
                    return true;
                }
                else
                {
                    return false;
                }
            }
        },
        {
            "scalar",
            [&]() -> bool {
                value.type = UniformValue::TYPE_SCALAR;
                value.scalar = (float) std::atof(list[1].c_str());
                return true;
            }
        },
        {
            "vec2",
            [&]() -> bool {
                value.type = UniformValue::TYPE_VECTOR2;
                value.vector2[0] = (float)std::atof(list[1].c_str());
                value.vector2[1] = (float)std::atof(list[2].c_str());
                return true;
            }
        },
        {
            "vec3",
            [&]() -> bool {
                value.type = UniformValue::TYPE_VECTOR3;
                value.vector3[0] = (float)std::atof(list[1].c_str());
                value.vector3[1] = (float)std::atof(list[2].c_str());
                value.vector3[2] = (float)std::atof(list[3].c_str());
                return true;
            }
        },
        {
            "vec4",
            [&]() -> bool {
                value.type = UniformValue::TYPE_VECTOR4;
                value.vector4[0] = (float)std::atof(list[1].c_str());
                value.vector4[1] = (float)std::atof(list[2].c_str());
                value.vector4[2] = (float)std::atof(list[3].c_str());
                value.vector4[3] = (float)std::atof(list[4].c_str());
                return true;
            }
        },
        {
            "int",
            [&]() -> bool {
                value.type = UniformValue::TYPE_INTEGER;
                value.integer = (int)std::atoi(list[1].c_str());
                return true;
            }
        },
        {
            "mat4",
            [&]() -> bool {
                value.type = UniformValue::TYPE_MATRIX4;
                glm::mat4 mat(1.0f);
                memcpy(value.matrix4, glm::value_ptr(mat), sizeof(mat));
                return true;
            }
        }
    };
    if (procs.find(list[0]) == procs.end())
    {
        return false;
    }

    return procs[list[0]]();
}

static bool parseUniformItem(ParseContext & context, const JDUniformItem & jd)
{
    UniformValue uniformValue;
    if (!parseUniformValue(context, jd.value, uniformValue))
    {
        return false;
    }
    context.uniforms[jd.id] = uniformValue;
    return true;
}

static bool parseMaterial(ParseContext & context, const JDMaterial & jd)
{
    for (auto & item : jd.uniforms)
    {
        if (!parseUniformItem(context, item))
        {
            return false;
        }
    }
    return true;
}

static bool parseCameraItem(ParseContext & context, const JDCameraItem & jd)
{
    auto & config = context.loader->config;
    float aspect = (float) config.width / (float) config.height;

    if (jd.type == "perspect")
    {
        context.cameras.emplace_back();
        context.cameras.back().makePerspectProj(
            glm::pi<float>() * jd.fan / 180.0f,
            aspect,
            jd.range[0],
            jd.range[1]
        );
        context.cameras.back().makeView(
            glm::vec3(jd.at[0], jd.at[1], jd.at[2]),
            glm::vec3(jd.look[0], jd.look[1], jd.look[2]),
            glm::vec3(jd.up[0], jd.up[1], jd.up[2])
        );
        context.cameras.back().flipY = jd.flipY;
        context.cameras.back().pass = parsePass(jd.pass);
        return true;
    }
    else if (jd.type == "ortho")
    {
        context.cameras.emplace_back();
        context.cameras.back().makeOrthoProj(
            jd.width,
            jd.width / aspect,
            jd.range[0],
            jd.range[1]
        );
        context.cameras.back().makeView(
            glm::vec3(jd.at[0], jd.at[1], jd.at[2]),
            glm::vec3(jd.look[0], jd.look[1], jd.look[2]),
            glm::vec3(jd.up[0], jd.up[1], jd.up[2])
        );
        context.cameras.back().flipY = jd.flipY;
        context.cameras.back().pass = parsePass(jd.pass);
        return true;
    }
    else if (jd.type == "identity")
    {
        context.cameras.emplace_back();
        context.cameras.back().flipY = jd.flipY;
        context.cameras.back().pass = parsePass(jd.pass);
        return true;
    }
    else
    {
        return false;
    }
}

static bool parseTransformItem(ParseContext & context, const JDTransformItem & jd)
{
    Scene::Transform transform;
    if (jd.type == "scale")
    {
        transform.type = Scene::Transform::Type::TYPE_SCALE;
    }
    else if (jd.type == "translate")
    {
        transform.type = Scene::Transform::TYPE_TRANSLATE;
    }
    else if (jd.type == "rotate")
    {
        transform.type = Scene::Transform::TYPE_ROTATE;
    }
    else
    {
        return false;
    }
    for (auto & exprStr : jd.value)
    {
        transform.exprs.emplace_back(split(exprStr, " "));
    }
    context.transforms.back().emplace_back(transform);
    return true;
}

static bool parseRenderItem(ParseContext & context, const JDRenderItem & jd)
{
    context.renders.emplace_back();
    context.transforms.emplace_back();
    context.follows.emplace_back(jd.follow);
    DescLoader& loader = *context.loader;
    if (jd.path != "")
    {
        context.renders.back().reset(new RenderObject);
        auto ptr = std::static_pointer_cast<RenderObject>(context.renders.back());
        if (!loader.loadRenderObject(jd.path, *ptr))
        {
            return false;
        }
    }
    else if (jd.group != "")
    {
        context.renders.back().reset(new RenderGroup);
        auto & jdGroup = context.renderGroupMap[jd.group];
        std::vector<std::shared_ptr<IRenderObject>> subRenders;
        for (auto & item : jdGroup.renders)
        {
            subRenders.emplace_back(new RenderObject);
            auto subPtr = std::static_pointer_cast<RenderObject>(subRenders.back());
            if (!loader.loadRenderObject(item.path, *subPtr))
            {
                return false;
            }
            subPtr->setPassMask(parsePass(item.pass));
        }
        auto ptr = std::static_pointer_cast<RenderGroup>(context.renders.back());
        ptr->setup(subRenders);
    }
    else
    {
        return false;
    }
    context.renders.back()->setPassMask(parsePass(jd.pass));
    for (auto & item : jd.transforms)
    {
        if (!parseTransformItem(context, item))
        {
            return false;
        }
    }
    for (auto & item : jd.uniforms)
    {
        if (!parseUniformItem(context, item))
        {
            return false;
        }
    }
    return true;
}

static bool parseLightItem(ParseContext & context, const JDLightItem & jd)
{
    context.lights.emplace_back();
    DescLoader& loader = *context.loader;
    if (!loader.loadLight(jd.path, context.lights.back()))
    {
        return false;
    }

    for (auto & item : jd.uniforms)
    {
        if (!parseUniformItem(context, item))
        {
            return false;
        }
    }
    return true;
}


bool sunty::DescLoader::loadRenderObject(
	const std::filesystem::path & path,
    RenderObject & render)
{
    auto asbPath = assembllyPath(path);
    std::ifstream streamIn(asbPath);
    if (!streamIn.is_open())
    {
        return false;
    }

    searchPaths.emplace_back(asbPath.parent_path());
    
    ParseContext context;

    context.loader = this;

    json jsonRender;

    streamIn >> jsonRender;

    JDRender jdRender = jsonRender.get<JDRender>();

    if (!parseRenderOptions(context, jdRender.options))
    {
        return false;
    }

    if (!parseShader(context, jdRender.shader))
    {
        return false;
    }

    if (!parseMesh(context, jdRender.mesh))
    {
        return false;
    }

    if (!parseMaterial(context, jdRender.material))
    {
        return false;
    }

    render.setup(
        context.mesh,
        context.shader,
        context.options
    );

    for (auto & entry : context.uniforms)
    {
        auto & id = entry.first;
        auto & val = entry.second;
        render.setParam(id.c_str(), val);
    }

    searchPaths.pop_back();
    
    return true;
}

bool sunty::DescLoader::loadLight(
	const std::filesystem::path & path,
    Scene::Light & light)
{
    auto asbPath = assembllyPath(path);
    std::ifstream streamIn(asbPath);
    if (!streamIn.is_open())
    {
        return false;
    }

    searchPaths.emplace_back(asbPath.parent_path());

    ParseContext context;

    context.loader = this;

    json json;

    streamIn >> json;

    JDLight jdLight = json.get<JDLight>();

    if (jdLight.type == "point")
    {
        light.type = Scene::Light::TYPE_POINT;
    }
    else
    {
        return false;
    }

    for (auto & item : jdLight.uniforms)
    {
        if (!parseUniformItem(context, item))
        {
            return false;
        }
    }

    light.uniforms = std::move(context.uniforms);

    searchPaths.pop_back();
    return true;
}

bool sunty::DescLoader::loadScene(
	const std::filesystem::path & path,
    const Starter & config,
    Scene & scene)
{
    auto asbPath = assembllyPath(path);
    std::ifstream streamIn(asbPath);
    if (!streamIn.is_open())
    {
        return false;
    }

    this->config = config;

    searchPaths.emplace_back(asbPath.parent_path());

    ParseContext context;

    context.loader = this;

    json json;

    streamIn >> json;

    JDScene jdScene = json.get<JDScene>();

    for (auto & item : jdScene.cameras)
    {
        if (!parseCameraItem(context, item))
        {
            return false;
        }
    }
    for (auto & item : jdScene.renderGroups)
    {
        context.renderGroupMap[item.name] = item;
    }
    for (auto & item : jdScene.renders)
    {
        if (!parseRenderItem(context, item))
        {
            return false;
        }
        auto uniforms = std::move(context.uniforms);
        for (auto & entry : uniforms)
        {
            auto & id = entry.first;
            auto & value = entry.second;
            context.renders.back()->setParam(id.c_str(), value);
        }
    }
    for (auto & item : jdScene.lights)
    {
        if (!parseLightItem(context, item))
        {
            return false;
        }
        auto uniforms = std::move(context.uniforms);
        for (auto & entry : uniforms)
        {
            auto & id = entry.first;
            auto & value = entry.second;
            context.lights.back().uniforms[id] = value;
        }
    }

    scene.cameras = std::move(context.cameras);
    scene.renders = std::move(context.renders);
    scene.transforms = std::move(context.transforms);
    scene.lights = std::move(context.lights);
    scene.follows = std::move(context.follows);

    searchPaths.pop_back();

    return true;
}

struct JDStarter
{
    std::string version;
    int width = 800;
    int height = 600;
    int fps = 40;
    std::string drawerType = "forward";
    std::string path;
};

bool DescLoader::loadStarter(
	const std::filesystem::path & path,
    Starter & starter)
{
    std::ifstream streamIn(path);
    if (!streamIn.is_open())
    {
        return false;
    }
    json j;
    JDStarter jd;
    streamIn >> j;
    JD_FROM_JSON(version);
    JD_FROM_JSON(width);
    JD_FROM_JSON(height);
    JD_FROM_JSON(fps);
    JD_FROM_JSON(drawerType);
    JD_FROM_JSON(path);

    starter.width = jd.width;
    starter.height = jd.height;
    starter.fps = jd.fps;
    starter.drawerType = sDrawerTypeMap[jd.drawerType];
    starter.path = jd.path;
    return true;
}


std::filesystem::path DescLoader::assembllyPath(const std::filesystem::path & path)
{
    for (int k = searchPaths.size() - 1; k >= 0; --k)
    {
        auto & sp = searchPaths[k];
        auto ap = sp / path;
        ap = ap.lexically_normal();
        auto repos = ap.string().find("[f]");
        if (repos != std::string::npos)
        {
            std::string repStr = ap.string().replace(repos, 3, "-pos-x");
            if (std::filesystem::exists(repStr))
            {
                return ap;
            }
        }
        else
        {
            if (std::filesystem::exists(ap))
            {
                return ap;
            }
        }
    }
    return path;
}
