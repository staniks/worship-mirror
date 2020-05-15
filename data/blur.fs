#version 330 core
layout (location = 0) out vec4 outFragColor;

in vec4 vertexColor;
in vec2 texCoord;

uniform sampler2D uniformTexture0;
uniform int uniformPass;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 tex_offset = 1.0 / textureSize(uniformTexture0, 0);
    vec3 result = texture(uniformTexture0, texCoord).rgb * weight[0];
    if(uniformPass == 0)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(uniformTexture0, texCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(uniformTexture0, texCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(uniformTexture0, texCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(uniformTexture0, texCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    outFragColor = vec4(result, 1.0);
}
