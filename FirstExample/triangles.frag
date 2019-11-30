#version 430 core

struct Light
{
	float ambientStrength;

	vec3 diffuseColor;
	float diffuseStrength;

	float specularStrength;
	float shininess;
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
uniform vec3 eyePos;


void main()
{
	////////////calculate ambient//////////////////
	vec3 ambient = pLight.base.diffuseColor * pLight.base.ambientStrength;
	//////////////////////////////////////////////////////


	////////////calculate diffuse//////////////////
	//calculate the vector from this pixels surface to the light source
    vec3 surfaceToLight = pLight.position - WorldPos;
	float distanceSurfaceToLight = length(surfaceToLight);
	surfaceToLight = normalize(surfaceToLight);

	//Calculate diffuse and diffuseFactor
	float diffuseFactor = max(dot(normalize(Normal), surfaceToLight), 0.f);
	vec3 diffuse = pLight.base.diffuseColor * pLight.base.diffuseStrength * diffuseFactor;
	//////////////////////////////////////////////////////

	////////////calculate specular//////////////////
	vec3 specular = vec3(0.f, 0.f, 0.f);
	if(diffuseFactor > 0.f)
	{
		vec3 surfaceToEye = normalize(eyePos - WorldPos);
		vec3 reflectDir = reflect(-surfaceToLight, normalize(Normal));

		float specularFactor = dot(surfaceToEye, reflectDir);

		if(specularFactor > 0.f)
		{
			specularFactor = pow(specularFactor, pLight.base.shininess);
			specular = pLight.base.diffuseColor * pLight.base.specularStrength * specularFactor;
		}
	}
	//////////////////////////////////////////////////////

	//float Attenuation = clamp(	1.f / pLight.exponent * distanceSurfaceToLight * distanceSurfaceToLight +
	//							pLight.linear * distanceSurfaceToLight + 
	//							pLight.constant, 0.0f, 1.0f);

	vec4 surfaceColor = texture(texture0, texCoord);									
	//frag_colour = vec4(surfaceColor.rgb * (ambient + (Attenuation * diffuse), surfaceColor.a);
	frag_colour = vec4(surfaceColor.rgb * (ambient + diffuse + specular), surfaceColor.a);
}