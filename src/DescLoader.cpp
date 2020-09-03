#include "DescLoader.h"
#include "nlohmann/json.hpp"
#include "SimpleObjLoader.h"
#include "ImageUtils.h"

using namespace sunty;
using namespace nlohmann;

DescLoader::DescLoader()
{
}


DescLoader::~DescLoader()
{
}

#define JD_FROM_JSON(name) j.at(#name).get_to(jd.name)

struct JDRenderOptions
{
    bool alphaBlend;
    bool depthTest;
    bool depthMask;
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
    float fan;
    std::vector<float> aspect;
    std::vector<float> range;
    std::vector<float> at;
    std::vector<float> look;
    std::vector<float> up;
    bool flipY;
};

void from_json(const json& j, JDCameraItem& jd)
{
    JD_FROM_JSON(type);
    JD_FROM_JSON(fan);
    JD_FROM_JSON(aspect);
    JD_FROM_JSON(range);
    JD_FROM_JSON(at);
    JD_FROM_JSON(look);
    JD_FROM_JSON(up);
    JD_FROM_JSON(flipY);
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
    bool follow;
    std::vector<JDTransformItem> transforms;
    std::vector<JDUniformItem> uniforms;
};

void from_json(const json& j, JDRenderItem& jd)
{
    JD_FROM_JSON(path);
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
    std::vector<JDRenderItem> renders;
    std::vector<JDLightItem> lights;
};

void from_json(const json& j, JDScene& jd)
{
    JD_FROM_JSON(version);
    JD_FROM_JSON(cameras);
    JD_FROM_JSON(renders);
    JD_FROM_JSON(lights);
}

static bool loadFile(const std::string & path, std::string & text)
{
    std::ifstream stream;
    stream.open(path);
    if (!stream.is_open())
    {
        return false;
    }
    auto size = std::streamoff(stream.seekg(0, std::ios::end).tellg());
    text.resize(size + 1);
    stream.seekg(0, std::ios::beg);
    stream.read(const_cast<char*>(text.data()), size);
    text[size] = 0;
    stream.close();
    return true;
}

static std::string processPath(const std::string & path)
{
#if WIN32 || WIN64
    auto res = path;
    for (size_t i = 0; i < res.size(); ++i)
    {
        if (res[i] == '/')
        {
            res[i] = '\\';
        }
    }
    return res;
#else
    return path;
#endif
}

static std::string getDir(const std::string & path)
{
#if WIN32 || WIN64
    auto sepPos = path.rfind('\\');
#else
    auto sepPos = path.rfind('/');
#endif
    if (sepPos == std::string::npos)
    {
        return "";
    }
    return path.substr(0, sepPos + 1);
}

struct ParseContext
{
    std::string dirPath;
    RenderOptions options;
    std::shared_ptr<Shader> shader;
    std::shared_ptr<VertexBufferObject> vbo;
    std::shared_ptr<VertexArrayObject> vao;
    std::unordered_map<std::string, UniformValue> uniforms;
    std::vector<Camera> cameras;
    std::vector<RenderObject> renders;
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
    context.shader = std::make_shared<Shader>();
    std::string vertCode, fragCode;
    if (!loadFile(context.dirPath + processPath(jd.pathVert), vertCode))
    {
        return false;
    }
    if (!loadFile(context.dirPath + processPath(jd.pathFrag), fragCode))
    {
        return false;
    }
    context.shader->setup(vertCode.c_str(), fragCode.c_str());
    return true;
}

static bool parseMesh(ParseContext & context, const JDMesh & jd)
{
    context.vbo = std::make_shared<VertexBufferObject>();
    context.vao = std::make_shared<VertexArrayObject>();
    if (jd.type == "obj")
    {
        SimpleObjLoader loader;
        loader.load(
            (context.dirPath + processPath(jd.path)).c_str(),
            true,
            *context.vbo,
            *context.vao
        );
        return true;
    }
    else if (jd.type == "simple")
    {
        return false;
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
                auto texture = std::make_shared<Texture>();
                if (type == "file")
                {
                    if (!loadPngTexture(
                        (context.dirPath + processPath(list[2])).c_str(),
                        list[3].c_str(),
                        *texture
                    ))
                    {
                        return false;
                    }
                    value.type = UniformValue::TYPE_TEXTURE;
                    value.texture = texture;
                    return true;
                }
                else if (type == "built-in")
                {
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
                auto cubemap = std::make_shared<Cubemap>();
                if (type == "file")
                {
                    if (!loadPngCubemap(
                        (context.dirPath + processPath(list[2])).c_str(),
                        list[3].c_str(),
                        *cubemap
                    ))
                    {
                        return false;
                    }
                    value.type = UniformValue::TYPE_TEXTURE;
                    value.texture = cubemap;
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
    if (jd.type == "perspect")
    {
        context.cameras.emplace_back();
        context.cameras.back().makePerspectProj(
            glm::pi<float>() * jd.fan / 180.0f,
            jd.aspect[0] / jd.aspect[1],
            jd.range[0],
            jd.range[1]
        );
        context.cameras.back().makeView(
            glm::vec3(jd.at[0], jd.at[1], jd.at[2]),
            glm::vec3(jd.look[0], jd.look[1], jd.look[2]),
            glm::vec3(jd.up[0], jd.up[1], jd.up[2])
        );
        context.cameras.back().flipY = jd.flipY;
        return true;
    }
    else if (jd.type == "ortho")
    {
        context.cameras.emplace_back();
        context.cameras.back().makeOrthoProj(
            jd.aspect[0],
            jd.aspect[1],
            jd.range[0],
            jd.range[1]
        );
        context.cameras.back().makeView(
            glm::vec3(jd.at[0], jd.at[1], jd.at[2]),
            glm::vec3(jd.look[0], jd.look[1], jd.look[2]),
            glm::vec3(jd.up[0], jd.up[1], jd.up[2])
        );
        context.cameras.back().flipY = jd.flipY;
        return true;
    }
    else if (jd.type == "identity")
    {
        context.cameras.emplace_back();
        context.cameras.back().flipY = jd.flipY;
        return true;
    }
    else
    {
        return false;
    }
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
    DescLoader loader;
    if (!loader.loadRenderObject(
        (context.dirPath + jd.path).c_str(),
        context.renders.back()
    ))
    {
        return false;
    }
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
    DescLoader loader;
    if (!loader.loadLight(
        (context.dirPath + jd.path).c_str(),
        context.lights.back()
    ))
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


bool sunty::DescLoader::loadRenderObject(const char * path, RenderObject & render)
{
    std::ifstream streamIn(path);
    if (!streamIn.is_open())
    {
        return false;
    }
    
    ParseContext context;

    context.dirPath = getDir(path);

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
        context.vbo,
        context.vao,
        context.shader,
        context.options
    );

    for (auto & entry : context.uniforms)
    {
        auto & id = entry.first;
        auto & val = entry.second;
        render.setParam(id.c_str(), val);
    }
    
    return true;
}

bool sunty::DescLoader::loadLight(const char * path, Scene::Light & light)
{
    std::ifstream streamIn(path);
    if (!streamIn.is_open())
    {
        return false;
    }

    ParseContext context;

    context.dirPath = getDir(path);

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

    return true;
}

bool sunty::DescLoader::loadScene(const char * path, Scene & scene)
{
    std::ifstream streamIn(path);
    if (!streamIn.is_open())
    {
        return false;
    }

    ParseContext context;

    context.dirPath = getDir(path);

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
            context.renders.back().setParam(id.c_str(), value);
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

    scene.setup();

    return true;
}
