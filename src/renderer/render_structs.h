#pragma once

#include <glm/glm.hpp>

struct ObjectDataConstants
{
    glm::mat4 model;
};

struct UniformBufferObject
{
    // The model matrix will be saved a a push constant. Since it changes often a push constant is faster than having to look it up in a descriptor set
    //glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};