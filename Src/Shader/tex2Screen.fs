#version 330 core

out vec4 FragColor;

uniform sampler2D curFrameTex;

vec3 ACESFilm(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e),0,1);
}

void main()
{
    vec3 color=texture2D(curFrameTex,gl_FragCoord.xy/textureSize(curFrameTex,0)).rgb;
    color=ACESFilm(color);
    color=pow(color,vec3(1.0/2.2));
    FragColor=vec4(color,1);
}

