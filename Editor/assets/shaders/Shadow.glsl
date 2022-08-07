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

void main()
{
	vec4 texColor = texture(u_Framebuffer, v_TexCoord);
	vec4 white = vec4(1, 1, 1, 1);
    vec4 black = vec4(0, 0, 0, 1);

	float average = texColor.r + texColor.g + texColor.b/3.0;

	if (average <= 0.5)
    {
        texColor = black;
    }
    else 
        texColor = white;

	o_Color = texColor;
}
