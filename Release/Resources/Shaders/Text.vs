#version 460 core

layout (location = 0) in vec4 Vertex; //xy & uv

out vec2 FragTexCoords;

uniform mat4 ProjectionMat;

void main()
{
	gl_Position = ProjectionMat * vec4(Vertex.xy, 0.0f, 1.0f);
	FragTexCoords = Vertex.zw;
}