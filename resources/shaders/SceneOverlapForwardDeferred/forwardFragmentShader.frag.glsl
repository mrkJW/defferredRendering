#version 430

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 PositionData;
layout (location = 2) out vec3 NormalData;
layout (location = 3) out vec3 ColorData;

uniform int lightingComplexity;

uniform struct LightInfo {
  vec4 Position;                // Light position in eye coords.
  vec3 L;                       // D,S intensity
  vec3 La;                      // Ambient
} Light ;

uniform struct MaterialInfo {
  vec3 Kd;                      // Diffuse reflectivity
} Material;

void main() {
    vec3 s = normalize( vec3(Light.Position) - Position);
    float sDotN = max( dot(s,normalize(Normal)), 0.0 );

    unsigned int j = 0;
    for (unsigned int i = 0; i < lightingComplexity; i++)
    {
        j = i+3;
    }

    FragColor = vec4( Light.L * Material.Kd * sDotN, 1.0 );
}
