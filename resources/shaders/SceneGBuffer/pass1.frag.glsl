#version 330 core
layout (location = 0) out vec4 FragColor;



in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform int renderType;

void main()
{    
    switch (renderType)
    {
    case 0: //"gPosition"
        FragColor = vec4(FragPos, 1.0f);
        break;
    case 1: //"gNormal"
        FragColor = vec4(normalize(Normal), 1.0f);
        break;
    case 2: //"gAlbedo"
        FragColor.rgb = texture(texture_diffuse1, TexCoords).rgb;
        break;
    case 3: //"gSpecular"
        FragColor.r = texture(texture_specular1, TexCoords).r;
        break;
    }
}