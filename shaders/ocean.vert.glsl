// Ocean Water Shader
attribute vec3 positionIn;
attribute vec2 e_x_t;
//attribute vec3 normalIn;

varying vec3 normal;
varying vec3 eyePosition;

void main() {
// Transform the vertex to get the eye-space position of the vertex
	vec4 eyeTemp = gl_ModelViewMatrix * vec4(positionIn, 1);
//	vec4 eyeTemp = gl_ModelViewMatrix * gl_Vertex;
	eyePosition = eyeTemp.xyz;

	// Transform again to get the clip-space position.  The gl_Position
	// variable tells OpenGL where the vertex should go.
	gl_Position = gl_ProjectionMatrix * eyeTemp;

	vec3 normal_in = vec3(-e_x_t.x, 1.0, -e_x_t.y);
	normal_in = normalize(normal_in);

	// Transform the normal, just like in Assignment 2.
	//normal = gl_NormalMatrix * gl_Normal;
	normal = gl_NormalMatrix * normal_in;
}