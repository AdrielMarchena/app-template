#type vertex
#version 330 core
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
#version 330 core

layout(location = 0) out vec4 o_Color;

in vec2 v_TexCoord;

uniform sampler2D u_Framebuffer;
uniform sampler2D u_GradeLut;
uniform float u_Width;
uniform float u_Height;

uniform float offsets[25];
uniform int offset_size;

uniform float kernel_5x5_slot[5*5];
uniform int kernel_size;

vec4 transform(in vec4 textureColor, in sampler2D lookupTable);

#define LUT_NO_CLAMP

void main()
{
    if(kernel_size > 0)
    {
        vec3 cl = vec3(0.0f);
        for(int i = 0; i < kernel_size; i++)
            cl += vec3(texture(u_Framebuffer, v_TexCoord + offsets[i % offset_size])) * kernel_5x5_slot[i];
        o_Color = vec4(cl,1.0f);

    }
    else
        o_Color = texture(u_Framebuffer, v_TexCoord);
}