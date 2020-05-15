#version 330 core
layout (location = 0) out vec4 outFragColor;

in vec4 vertexColor;
in vec2 texCoord;

uniform sampler2D uniformTexture0;

void main()
{
    vec4 textureColor = texture(uniformTexture0, texCoord);
    outFragColor = vertexColor * textureColor;
}
