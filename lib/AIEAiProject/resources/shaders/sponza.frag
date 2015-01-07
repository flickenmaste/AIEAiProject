#version 330

in vec4 worldPosition;
in vec4 worldNormal;

layout( location = 0 ) out vec4 fragColor;

void main()
{
	vec3 color = vec3(1);

	if (mod( worldPosition.x, 1.0 ) < 0.05f ||
		mod( worldPosition.y, 1.0 ) < 0.05f ||
		mod( worldPosition.z, 1.0 ) < 0.05f)
		color = vec3(0);

	float d = max( 0, dot( normalize(vec3(1,1,1)), normalize(worldNormal.xyz) ) ) * 0.75f;

	fragColor.rgb = color * 0.25f + color * d;
	fragColor.a = 1;
}