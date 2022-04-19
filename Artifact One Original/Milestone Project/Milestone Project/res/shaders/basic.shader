#shader vertex
#version 440 core

	layout(location = 0) in vec3 position;
	layout(location = 1) in vec3 normal;
	layout(location = 2) in vec2 texCoord;


	out vec3 vertexNormal;
	out vec3 vertexFragmentPos;
	out vec2 v_TexCoord;

	uniform mat4 Model;
	uniform mat4 Projection;
	uniform mat4 View;

void main()
{
	gl_Position = Projection * View * Model *vec4(position,1.0f);
	vertexFragmentPos = vec3(model * vec4(position, 1.0f));
	vertexNormal = mat3(transpose(inverse(model))) * normal;
	v_TexCoord = texCoord;
};

#shader fragment
#version 440 core

in vec3 vertexNormal;
in vec3 vertexFragmentPos;
in vec2 v_TexCoord;

out vec4 fragmentColor;
uniform sampler2D uTexture;
uniform vec2 uvScale;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;

void main()
{
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(vertexNormal);
	vec3 lightDirection = normalize(lightPos - vertexFragmentPos);
	float impact = max(dot(norm, lightDirection), 0.0)
	vec3 diffuse = impact * lightColor;

	float specularIntensity = 0.8f;
	float highlightSize = 16.0f;
	vec3 viewDir = normalize(viewPosition - vertexFragmentPos);
	vec3 reflectDir = reflect(-lightDirection, norm);
	float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
	vec3 specular = specularIntensity * specularComponent * lightColor;


	vec4 textureColor = texture(uTexture, v_TexCoord * uvScale);

	vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

	fragmentColor = vec4(phong, 1.0);
	
};

