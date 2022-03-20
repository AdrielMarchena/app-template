#type vertex
#version 400 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in int a_EntityID;

uniform mat4 u_ViewProj;

out vec4 v_Color;
out vec2 v_TexCoord;
out float v_TexIndex;
out vec3 v_Pos;
flat out int v_EntityID;

void main()
{
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;
	v_Pos = a_Position;
	v_EntityID = a_EntityID;
	gl_Position = u_ViewProj * vec4(a_Position, 1.0);
}

#type fragment
#version 400 core
#define MAX_TEXTURES_SLOTS 16

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;
in vec3 v_Pos;
flat in int v_EntityID;

uniform sampler2D u_Textures[MAX_TEXTURES_SLOTS];

void main()
{
	o_EntityID = v_EntityID;

	int index = int(v_TexIndex);
	o_Color = texture(u_Textures[index], v_TexCoord) * v_Color;
	//if (o_Color.a <= 0.0)
	//	discard;
}
