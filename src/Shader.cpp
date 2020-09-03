#include "Shader.h"

using namespace sunty;

Shader::Shader()
{
}


Shader::~Shader()
{
    release();
}


void Shader::setup(const char * vertexCode, const char * fragmentCode)
{
    auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertexCode, NULL);
    glCompileShader(vertex_shader);
    GLint vertCompiled = 0;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertCompiled);
    if (vertCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(
            vertex_shader,
            maxLength,
            &maxLength,
            infoLog.data()
        );
        printf("VertShader Compile Err: %s\n", infoLog.data());
    }

    auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragmentCode, NULL);
    glCompileShader(fragment_shader);
    GLint fragCompiled = 0;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragCompiled);
    if (fragCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(
            fragment_shader,
            maxLength,
            &maxLength,
            infoLog.data()
        );
        printf("FragShader Compile Err: %s\n", infoLog.data());
    }

    mProgram = glCreateProgram();
    glAttachShader(mProgram, vertex_shader);
    glAttachShader(mProgram, fragment_shader);
    glLinkProgram(mProgram);

    GLint isLinked = 0;
    glGetProgramiv(mProgram, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(mProgram, maxLength, &maxLength, infoLog.data());

        printf("ShaderProgram Link Err: %s\n", infoLog.data());

        // The program is useless now. So delete it.
        glDeleteProgram(mProgram);
        mProgram = 0;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}


void Shader::release()
{
    if (mProgram != 0)
    {
        glDeleteProgram(mProgram);
        mUniformLocationMap.clear();
        mAttributeLocationMap.clear();
        mProgram = 0;
    }
}


void Shader::use(bool value)
{
    glUseProgram(value ? mProgram : 0);
}

bool sunty::Shader::hasUniform(const char * name)
{
    auto loc = getUniformLocation(name);
    return loc != -1;
}

bool Shader::setUniformInt(const char * name, int value)
{
    auto loc = getUniformLocation(name);
    if (loc == -1)
    {
        return false;
    }
    glUniform1i(loc, value);
    return true;
}

bool Shader::setUniformScalar(const char * name, float value)
{
    auto loc = getUniformLocation(name);
    if (loc == -1)
    {
        return false;
    }
    glUniform1f(loc, value);
    return true;
}

bool Shader::setUniformVector2(const char * name, const float * value)
{
    auto loc = getUniformLocation(name);
    if (loc == -1)
    {
        return false;
    }
    glUniform2fv(loc, 1, value);
    return true;
}

bool Shader::setUniformVector3(const char * name, const float * value)
{
    auto loc = getUniformLocation(name);
    if (loc == -1)
    {
        return false;
    }
    glUniform3fv(loc, 1, value);
    return true;
}

bool Shader::setUniformVector4(const char * name, const float * value)
{
    auto loc = getUniformLocation(name);
    if (loc == -1)
    {
        return false;
    }
    glUniform4fv(loc, 1, value);
    return true;
}

bool Shader::setUniformMatrix3(const char * name, const float * value)
{
    auto loc = getUniformLocation(name);
    if (loc == -1)
    {
        return false;
    }
    glUniformMatrix3fv(loc, 1, GL_FALSE, value);
    return true;
}

bool Shader::setUniformMatrix4(const char * name, const float * value)
{
    auto loc = getUniformLocation(name);
    if (loc == -1)
    {
        return false;
    }
    glUniformMatrix4fv(loc, 1, GL_FALSE, value);
    return true;
}

bool Shader::setAttribute(
    const char * name, 
    VertexBufferObject::AttrType attrType, 
    VertexBufferObject & vertexBuffer)
{
    auto loc = getAttributeLocation(name);
    if (loc == -1)
    {
        return false;
    }
    int offset = 0;
    for (auto & desc : vertexBuffer.attrDescs())
    {
        if (desc.type != attrType)
        {
            offset += desc.columns;
            continue;
        }
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(
            loc,
            desc.columns,
            GL_FLOAT,
            GL_FALSE,
            vertexBuffer.columns() * sizeof(float),
            (void *) (offset * sizeof(float))
        );
        break;
    }
    return true;
}

void Shader::enableAttribute(const char * name)
{
    auto loc = getAttributeLocation(name);
    glEnableVertexAttribArray(loc);
}

void Shader::disableAttribute(const char * name)
{
    auto loc = getAttributeLocation(name);
    glDisableVertexAttribArray(loc);
}

GLint Shader::getUniformLocation(const char * name)
{
    if (mUniformLocationMap.find(name) == mUniformLocationMap.end())
    {
        auto loc = glGetUniformLocation(mProgram, name);
        if (loc == -1)
        {
            return -1;
        }
        else
        {
            mUniformLocationMap[name] = loc;
            return loc;
        }
    }
    else
    {
        return mUniformLocationMap.at(name);
    }
}

GLint Shader::getAttributeLocation(const char * name)
{
    if (mAttributeLocationMap.find(name) == mAttributeLocationMap.end())
    {
        auto loc = glGetAttribLocation(mProgram, name);
        if (loc == -1)
        {
            return -1;
        }
        else
        {
            mAttributeLocationMap[name] = loc;
            return loc;
        }
    }
    else
    {
        return mAttributeLocationMap.at(name);
    }
}
