#version 460 core

in vec2 FragTexCoords;

uniform sampler2D TextTexture;
uniform vec3 TextColor;

out vec4 FinalColor;

void main()
{
	float alpha = texture(TextTexture, FragTexCoords).r;

	FinalColor = vec4(TextColor, alpha);
}