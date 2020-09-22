#include "RenderObject.h"

using namespace sunty;

void RenderObject::setup(
    const std::string & name,
    std::shared_ptr<Mesh> mesh,
    std::shared_ptr<Shader> shader,
    RenderOptions options)
{
    mName = name;
    mMesh = mesh;
    mShader = shader;
    mOptions = options;

    mShader->use(true);
    mMesh->use(true);
    auto& vbo = mMesh;
    mShader->setAttribute("Position", VertexBufferObject::AttrType::ATTR_POSITION, *vbo);
    mShader->setAttribute("Color", VertexBufferObject::AttrType::ATTR_COLOR, *vbo);
    mShader->setAttribute("Coord", VertexBufferObject::AttrType::ATTR_TEXTURE_COORD, *vbo);
    mShader->setAttribute("Normal", VertexBufferObject::AttrType::ATTR_NORMAL, *vbo);
    mShader->setAttribute("Tangent", VertexBufferObject::AttrType::ATTR_TANGENT, *vbo);
    mMesh->use(false);
    mShader->use(false);
}

void sunty::RenderObject::setParam(const char * name, const UniformValue & value)
{
    std::string paramName = name;
    if (mParamAlias.find(paramName) != mParamAlias.end())
    {
        paramName = mParamAlias[paramName];
    }
    if (mShader->hasUniform(paramName.c_str()))
    {
        mParams[paramName] = value;
    }
    else
    {
        //printf("RenderObject %s cannot find param %s=>%s\n", mName.c_str(), name, paramName.c_str());
    }
}

void sunty::RenderObject::draw()
{
    if (mOptions.alphaBlend)
    {
        glEnable(GL_BLEND);
    }
    else
    {
        glDisable(GL_BLEND);
    }
    if (mOptions.depthTest)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    if (mOptions.depthMask)
    {
        glDepthMask(GL_TRUE);
    }
    else
    {
        glDepthMask(GL_FALSE);
    }
    switch (mOptions.cullMode)
    {
    case RenderOptions::CULL_NONE:
        glDisable(GL_CULL_FACE);
        break;
    case RenderOptions::CULL_BACK:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        break;
    case RenderOptions::CULL_FRONT:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        break;
    }

    mShader->use(true);

    int textureCounter = 0;
    for (auto & entry : mParams)
    {
        auto name = entry.first.c_str();
        auto & param = entry.second;
        switch (param.type)
        {
            case UniformValue::TYPE_SCALAR:
                mShader->setUniformScalar(name, param.scalar);
                break;
            case UniformValue::TYPE_VECTOR2:
                mShader->setUniformVector2(name, param.vector2);
                break;
            case UniformValue::TYPE_VECTOR3:
                mShader->setUniformVector3(name, param.vector3);
                break;
            case UniformValue::TYPE_VECTOR4:
                mShader->setUniformVector4(name, param.vector4);
                break;
            case UniformValue::TYPE_MATRIX3:
                mShader->setUniformMatrix3(name, param.matrix3);
                break;
            case UniformValue::TYPE_MATRIX4:
                mShader->setUniformMatrix4(name, param.matrix4);
                break;
            case UniformValue::TYPE_INTEGER:
                mShader->setUniformInt(name, param.integer);
                break;
            case UniformValue::TYPE_TEXTURE:
            {
                glActiveTexture(GL_TEXTURE0 + textureCounter);
                glBindTexture(param.texture->target(), param.texture->id());
                mShader->setUniformInt(name, textureCounter);
                ++textureCounter;
                break;
            }
            default:
                break;
        }
    }

    mMesh->use(true);
    switch (mOptions.drawMode)
    {
    case RenderOptions::DRAW_LIST:
        mMesh->drawTriangles();
        break;
    case RenderOptions::DRAW_STRIP:
        mMesh->drawTriangleStrip();
        break;
    default:
        break;
    }
    mMesh->use(false);
    mShader->use(false);
}