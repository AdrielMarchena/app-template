#type vertex
#version 400 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in int a_EntityID;

uniform mat4 u_ViewProj;

out vec3 v_Pos;
out vec4 v_Color;
flat out int v_EntityID;

void main()
{
	v_Color = a_Color;
	v_Pos = a_Position;
	v_EntityID = a_EntityID;
	gl_Position = u_ViewProj * vec4(a_Position, 1.0);
}

#type fragment
#version 400 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

in vec3 v_Pos;
in vec4 v_Color;
flat in int v_EntityID;

void main()
{
	o_Color = v_Color;
	o_EntityID = v_EntityID;
}