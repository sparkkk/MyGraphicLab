#pragma once

#include "common.h"
#include "Texture.h"
#include "UniformValue.h"

namespace sunty
{
	struct RenderOptions
	{
		enum
		{
			DRAW_LIST = 0,
			DRAW_STRIP,
			DRAW_FAN
		} drawMode;
		enum
		{
			CULL_NONE,
			CULL_BACK,
			CULL_FRONT
		} cullMode;
		bool alphaBlend;
		bool depthTest;
		bool depthMask;
	};
    class IRenderObject
    {
    public:
        IRenderObject() = default;
        virtual ~IRenderObject() { release(); }
        virtual void release() {}
		virtual void setParam(const char * name, const UniformValue & value) = 0;
		virtual void draw() = 0;
        void setCurrentPass(Pass pass)
        {
            passCurr = pass;
        }
        void setPassMask(Pass pass)
        {
            passMask = pass;
        }
        Pass getPassMask()
        {
            return passMask;
        }
    protected:
        Pass passMask = PASS_NONE;
        Pass passCurr = PASS_NONE;
    };
}