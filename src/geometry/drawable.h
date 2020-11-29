#pragma once

#include "glad/glad.h"

class Drawable
{
public:
    virtual void render() const = 0;
};
