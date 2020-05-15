#version 330 core
layout (location = 0) in vec3 attributePos;
layout (location = 1) in vec4 attributeColor;
layout (location = 2) in vec2 attributeTexCoord;

out vec4 vertexColor;
out vec2 texCoord;

void main()
{
    gl_Position = vec4(attributePos, 1.0);
    vertexColor = attributeColor;
    texCoord = attributeTexCoord;
}
