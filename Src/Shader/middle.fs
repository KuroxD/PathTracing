#version 330 core

layout(location = 0) out vec4 FragColor;

uniform sampler2D curFrameTex;
void main()
{
	vec3 color=texture2D(curFrameTex,gl_FragCoord.xy/textureSize(curFrameTex,0)).rgb;
    FragColor=vec4(color,1);
}
