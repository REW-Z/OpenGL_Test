#version 430

struct DirectionalLight
{
	vec4 color;
	vec3 dir;
};

struct Material
{
	vec4 diffuse;
	vec4 specular;
	float gloss;
};

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoords;
layout (location = 2) in vec3 normal;



uniform mat4 m_matrix;
uniform mat4 v_matrix;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;

uniform vec4 ambient;
uniform DirectionalLight light;
uniform Material material;

//layout(binding = 0) uniform sampler2D samp;

out vec3 varyingPos;
out vec3 varyingNormal;
out vec2 uv;

void main(void)
{ 
	varyingPos = (mv_matrix * vec4(position, 1.0)).xyz;
	varyingNormal = (norm_matrix * vec4(normal, 1.0)).xyz;
	uv = texcoords;
	gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
}