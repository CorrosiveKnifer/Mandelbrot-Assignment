#version 460 core

uniform vec3 Color;
uniform sampler2D imageTexture;

in vec2 fragUV;

out vec4 FinalColor;

void main()
{
	FinalColor = vec4(Color, 1.0f) * texture(imageTexture, fragUV);
}