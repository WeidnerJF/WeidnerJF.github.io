#shader LampV
#version 440 core

layout(location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
}
);

#shader LampF
#version 440 core

out vec4 fragmentColor;

void main()
{
	fragmentColor = vec4(1.0f);
}
);