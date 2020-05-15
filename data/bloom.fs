#version 330 core
layout (location = 0) out vec4 outFragColor;

in vec4 vertexColor;
in vec2 texCoord;

uniform sampler2D uniformTexture0;
uniform sampler2D uniformTexture1;

void main()
{
    vec3 hdrColor = texture(uniformTexture0, texCoord).rgb;
    vec3 bloomColor = texture(uniformTexture1, texCoord).rgb;
    hdrColor += bloomColor;
    outFragColor = vec4(hdrColor, 1.0);
}
