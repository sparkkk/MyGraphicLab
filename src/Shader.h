#pragma once

#include "common.h"
#include "VertexBufferObject.h"


namespace sunty
{
	class Shader
	{
	public:
		Shader();
		~Shader();
		void setup(const char * vertexCode, const char * fragmentCode);
		void release();
		void use(bool value);
		bool hasUniform(const char * name);
		bool setUniformInt(const char * name, int value);
		bool setUniformScalar(const char * name, float value);
		bool setUniformVector2(const char * name, const float * value);
		bool setUniformVector3(const char * name, const float * value);
		bool setUniformVector4(const char * name, const float * value);
		bool setUniformMatrix3(const char * name, const float * value);
		bool setUniformMatrix4(const char * name, const float * value);
		bool setAttribute(const char * name, VertexBufferObject::AttrType attrType, VertexBufferObject & vertexBuffer);
		void enableAttribute(const char * name);
		void disableAttribute(const char * name);
	private:
		GLint getUniformLocation(const char * name);
		GLint getAttributeLocation(const char * name);
	private:
		GLuint mProgram = 0;
		std::unordered_map<std::string, GLint> mUniformLocationMap;
		std::unordered_map<std::string, GLint> mAttributeLocationMap;
	};
}


