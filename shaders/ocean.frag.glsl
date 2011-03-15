vec3 upwelling = vec3(0.0431, 0.12160, 0.3490);
//vec3 upwelling = vec3(0, 0,0);
vec3 sky = vec3(0.69, 0.84, 1);
vec3 air = vec3(0.15, 0.15, 0.15);
//vec3 air = vec3(0.69, 0.84, 1);
float nSnell = 1.34;
float Kdiffuse = 0.91;
float specular_alpha = 10.0;	//specular alpha, could be adjusted

varying vec3 normal;
varying vec3 eyePosition;

uniform mat4 inv_view_for_env;
uniform sampler2D envMap;

void main() {
	
	float reflectivity;
	vec3 N = normalize(normal);

	vec3 L = normalize(-gl_LightSource[0].position.xyz);
	vec3 V = eyePosition;	//the distance from water postion to eye, used for atmosphere scatter
	vec3 E = normalize(-eyePosition);	//direction from each point to the eye	

	float costhetai = abs(dot(N, E));
	float thetai = acos(costhetai);
	float sinthetat = sin(thetai)/nSnell;
	float thetat = asin(sinthetat);
	
	if (thetai == 0.0) {
		reflectivity = (nSnell - 1.0)/(nSnell + 1.0);
		reflectivity = reflectivity * reflectivity;
	}else{
		float fs = sin(thetat - thetai) / (thetat + thetai);
		float ts = tan(thetat - thetai) / (thetat + thetai);
		reflectivity = 0.5 * (fs * fs + ts * ts);
	}
		
	float dist = (length(V) * Kdiffuse)/100.0;
	dist = exp(-dist);
	vec3 R = reflect(L, N);

	vec3 R_E = reflect(E, N);
	R_E = vec4(inv_view_for_env * vec4(R_E, 0.0)).xyz;
	R_E = normalize(R_E);

//--------------------	need cube map?
//	vec2 text = vec2((R_E.x+1.0)/2.0, (R_E.z+1.0)/2.0);
//	sky = texture2D(envMap, text).rgb * 2.0;	//this is quite artificial, to provide sky texture color
	
//	reflectivity *= 10.0;
	float Rs = pow(max(0.0, dot(E, R)), 1000.0);
	vec3 specular = Rs * reflectivity * gl_LightSource[0].specular.rgb * dist;

	vec3 diffuse = (sky * reflectivity + upwelling * (1.0-reflectivity))* dist + air * (1.0-dist)   ;

	// This actually writes to the frame buffer
	gl_FragColor = vec4(diffuse + specular, 1);
//	gl_FragColor = vec4(diffuse, 1);
}
