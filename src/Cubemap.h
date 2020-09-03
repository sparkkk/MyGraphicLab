#pragma once

#include "common.h"
#include "Texture.h"

namespace sunty
{

class Cubemap : public Texture
{
public:
    Cubemap();
    virtual bool setup(const Option &option) override;
};

} // namespace sunty