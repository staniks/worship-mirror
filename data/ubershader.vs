#version 330 core
layout (location = 0) in vec3 attributePos;
layout (location = 1) in vec4 attributeColor;
layout (location = 2) in vec2 attributeTexCoord;

uniform bool uniformEnableFog;
uniform bool uniformEnableSpriteBillboarding;

uniform bool uniformEnableParticleMode;
uniform float uniformParticleSize;
uniform vec2 uniformResolution;

uniform mat4 uniformModel;
uniform mat4 uniformView;
uniform mat4 uniformProjection;

uniform bool uniformEnableLighting;
uniform int uniformLightCount;
uniform vec3 uniformLightPositions[32];
uniform vec3 uniformLightColors[32];
uniform float uniformLightRadii[32];

out vec4 vertexColor;
out vec2 texCoord;
out vec4 viewSpacePosition;

void main()
{
    mat4 modelView = uniformView * uniformModel;

    // Set rotation matrix columns for X and Z rotation to identity vectors.
    // This will achieve Doom-style billboarding effect.
    if(uniformEnableSpriteBillboarding)
    {
        // X rotation
        modelView[0][0] = 1;
        modelView[0][1] = 0;
        modelView[0][2] = 0;

        // Z rotation.
        modelView[2][0] = 0;
        modelView[2][1] = 0;
        modelView[2][2] = 1;
    }

    vec4 eyePosition = modelView * vec4(attributePos, 1.0);

    if(uniformEnableParticleMode)
    {
        vec4 projectedPosition = uniformProjection * vec4(uniformParticleSize, uniformParticleSize, eyePosition.z, eyePosition.w);
        vec2 projectedSize = uniformResolution * projectedPosition.xy / projectedPosition.w;
        gl_PointSize = projectedSize.x + projectedSize.y;
    }

    gl_Position = uniformProjection * eyePosition;

    vertexColor = attributeColor;
    if(uniformEnableLighting && !uniformEnableParticleMode)
    {
        for(int i=0; i<uniformLightCount; ++i)
        {
            vec3 modelSpaceVertex = (uniformModel * vec4(attributePos, 1.0)).xyz;
            float distance = length(uniformLightPositions[i] - modelSpaceVertex);
            float attenuation = clamp(1.0 - distance*distance/(uniformLightRadii[i]*uniformLightRadii[i]), 0.0, 1.0);
            vertexColor.rgb += attenuation * uniformLightColors[i];
        }
    }

    texCoord = attributeTexCoord;

    if(uniformEnableFog)
        viewSpacePosition = eyePosition;
    else
        viewSpacePosition = vec4(0,0,0,1);
}
