#version 330 core
layout (location = 0) out vec4 outFragColor;
layout (location = 1) out vec4 outBrightFragColor;

in vec4 vertexColor;
in vec2 texCoord;
in vec4 viewSpacePosition;

uniform sampler2D uniformTexture0;
uniform sampler2D uniformTexture1;

uniform bool uniformEnableParticleMode;
uniform bool uniformEnableEmission;
uniform bool uniformEnableFog;
uniform float uniformFogDensity;

void main()
{
    vec2 final_texCoord;
    if(uniformEnableParticleMode)
        final_texCoord = gl_PointCoord;
    else
        final_texCoord = texCoord;

	vec4 textureColor = texture(uniformTexture0, final_texCoord);
    if (textureColor.a <= 0.1)
        discard;

    if(uniformEnableParticleMode && vertexColor.a <= 0.1)
        discard;

    outFragColor = vertexColor * textureColor;

    if(uniformEnableEmission)
    {
        vec4 emissionColor = texture(uniformTexture1, final_texCoord);
        if(length(emissionColor.rgb) > 0)
            outFragColor += textureColor * emissionColor;
    }

    if(uniformEnableFog)
    {
        float distance = length(viewSpacePosition);
        float fogFactor = 1.0 /exp(distance * uniformFogDensity * distance * uniformFogDensity);
        fogFactor = clamp( fogFactor, 0.0, 1.0 );

        outFragColor.rgb *= fogFactor;
    }

    float brightness = dot(outFragColor.rgb, vec3(0.2126, 0.7152, 0.0722)) * outFragColor.a;
    outBrightFragColor = brightness > 0.75 ? vec4(outFragColor.rgb, outFragColor.a) : vec4(0.0, 0.0, 0.0, outFragColor.a);
}
