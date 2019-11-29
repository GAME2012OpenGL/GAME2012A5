#version 430 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_texture;
layout(location = 2) in vec3 vertex_normal;

out vec2 texCoord;
out vec3 Normal;
out vec3 WorldPos;

// Values that stay constant for the whole mesh.
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
	gl_Position = projection * view * model * vec4(vertex_position, 1.0);
	texCoord = vertex_texture;

	//So far, we've only used matrices to transform coordinates. The problem is that normals are not coordinates, 
	//they are unit vectors representing directions. Rotation transformations are fine, because the rotating a unit vector results in another 
	//unit vector, but scaling or translating a normal will result in an incorrect normal. The solution is to multiply the normals 
	//by a different matrix – one that has the translation and scaling parts fixed.
	//Removing the translation part of a 4x4 matrix is simple: we just remove the 4th column and row, converting it to a 3x3 matrix. 
	//Fixing the scaling is a bit trickier, but I'll jump straight to the answer, which is to invert and transpose the matrix. 
	//We will also need to renormalise each normal after it has been transformed, to ensure that it is still a unit vector. 
	//The GLSL to do this looks like:

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	Normal = normalize(normalMatrix * vertex_normal);

	//The model variable is the original 4x4 model transformation matrix. 
	//The mat3 function removes the translation part of the matrix. The inverse and transpose functions will fix up the scaling part.
	//Finally, after we transform the original normal with normalMatrix * normal, the normalize function will ensure that the transformed normal is a unit vector.

	WorldPos = vec3(model * vec4(vertex_position, 1.0));
}