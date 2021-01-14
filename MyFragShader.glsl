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


in vec3 varyingPos;
in vec3 varyingNormal;
in vec2 uv;

out vec4 color;


uniform mat4 m_matrix;
uniform mat4 v_matrix;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;

uniform vec4 ambient;
uniform DirectionalLight light;
uniform Material material;

layout(binding = 0) uniform sampler2D samp;

void main(void)
{
	vec3 N = normalize(varyingNormal);
	vec3 L = normalize((v_matrix * vec4(-light.dir, 0.0)).xyz);
	vec3 V = normalize(-varyingPos);
	vec3 H = normalize(L + V);

	vec3 diffuse = light.color.xyz * material.diffuse.xyz * max(0.0, dot(N, L));
	vec3 specular = light.color.xyz * material.specular.xyz * pow(max(0.0, dot(H, N)), material.gloss); 


	color = texture(samp, uv) * vec4((ambient.xyz + diffuse + specular), 1.0); 
}