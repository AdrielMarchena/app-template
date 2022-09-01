#type vertex
#version 400 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_ViewProj;

out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;

	gl_Position = u_ViewProj * vec4(a_Position, 1.0);
}

#type fragment
#version 400 core

layout(location = 0) out vec4 o_Color;

in vec2 v_TexCoord;

uniform sampler2D u_Framebuffer;
uniform vec3 u_Shadow;
uniform float u_AmbientStrength;

struct Light
{
	float Intensity = 1.0f;
	vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
};

uniform Light u_Lights[128];

void main()
{
	for(int i = 0; i < 128; i++)
	{
		
	}

	vec3 ambient = u_AmbientStrength * u_Shadow;
	vec4 color = vec4(ambient * texture(u_Framebuffer, v_TexCoord).xyz, 1.0);
	o_Color = color;
}
