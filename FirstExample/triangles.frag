#version 430 core

struct Light
{
	vec3 ambientColor;
	float ambientStrength;
	vec3 diffuseColor;
	float diffuseStrength;
};

struct PointLight
{
	Light base;

	vec3 position;
	float constant, linear, exponent;	//Quadratic equation for attenuation
};




in vec2 texCoord;
in vec3 Normal;
in vec3 WorldPos;

out vec4 frag_colour;

uniform sampler2D texture0;
uniform PointLight pLight;



void main()
{
	vec4 ambient = vec4(pLight.base.ambientColor, 1.f) * pLight.base.ambientStrength;

	//calculate the vector from this pixels surface to the light source
    vec3 surfaceToLight = pLight.position - WorldPos;
	float distanceSurfaceToLight = length(surfaceToLight);

	//Calculate diffuseFactor
	float diffuseFactor = max(dot(normalize(Normal), normalize(surfaceToLight)), 0.f);

	vec4 diffuse = vec4(pLight.base.diffuseColor, 1.f) * pLight.base.diffuseStrength * diffuseFactor;

	float Attenuation = clamp(	1.f / pLight.exponent * distanceSurfaceToLight * distanceSurfaceToLight +
								pLight.linear * distanceSurfaceToLight + 
								pLight.constant, 0.0f, 1.0f);

	frag_colour = texture(texture0, texCoord) * (ambient + (Attenuation * diffuse));
}