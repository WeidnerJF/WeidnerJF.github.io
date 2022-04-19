#shader vertex
#version 440 core

	layout(location = 0) in vec3 position;
	layout(location = 1) in vec2 texCoord;

	out vec2 v_TexCoord;
	uniform mat4 Model;
	uniform mat4 Projection;
	uniform mat4 View;

void main()
{
	gl_Position = Projection * View * Model *vec4(position,1.0f);
	v_TexCoord = texCoord;
};

#shader fragment
#version 440 core

in vec2 v_TexCoord;
out vec4 color;
uniform sampler2D u_Texture;
void main()
{
	vec4 texColor = texture(u_Texture, v_TexCoord);
	color = texColor;
};
