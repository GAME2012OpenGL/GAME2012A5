#version 430 core

struct Light
{
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
	//calculate ambient
	vec3 ambient = pLight.base.ambientColor * pLight.base.ambientStrength;



	//calculate the vector from this pixels surface to the light source
    vec3 surfaceToLight = pLight.position - WorldPos;
	float distanceSurfaceToLight = length(surfaceToLight);



	//Calculate diffuse and diffuseFactor
	float diffuseFactor = clamp(dot(normalize(Normal), normalize(surfaceToLight)), 0.f, 1.f);
	vec3 diffuse = pLight.base.diffuseColor * pLight.base.diffuseStrength * diffuseFactor;



	float Attenuation = clamp(	1.f / pLight.exponent * distanceSurfaceToLight * distanceSurfaceToLight +
								pLight.linear * distanceSurfaceToLight + 
								pLight.constant, 0.0f, 1.0f);

	vec4 surfaceColor = texture(texture0, texCoord);									
	//frag_colour = vec4(surfaceColor.rgb * (ambient + (Attenuation * diffuse), surfaceColor.a);
	frag_colour = vec4(surfaceColor.rgb * (ambient + diffuse), surfaceColor.a);
}