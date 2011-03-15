// This is a texture sampler.  It lets you sample textures!  The keyword
// "uniform" means constant - sort of.  The uniform variables are the same
// for all fragments in an object, but they can change in between objects.
uniform sampler2D diffuseMap;
uniform sampler2D specularMap;

// Diffuse, ambient, and specular materials.  These are also uniform.
uniform vec3 Kd;
uniform vec3 Ks;
uniform vec3 Ka;
uniform float alpha;

// These are values that OpenGL interpoates for us.  Note that some of these
// are repeated from the fragment shader.  That's because they're passed
// across.
varying vec2 texcoord;
varying vec3 normal;
varying vec3 eyePosition;

void main() {

    // Normalize the normal, and calculate light vector and view vector
    // Note: this is doing a directional light, which is a little different
    // from what you did in Assignment 2.
    vec3 N  = normalize(normal);
    vec3 L0 = normalize(-gl_LightSource[0].position.xyz);   // directional light1
    vec3 L1 = normalize(-gl_LightSource[1].position.xyz);   // directional light2
    vec3 V  = normalize(-eyePosition);
        
    // Calculate the diffuse color coefficient, and sample the diffuse texture
    // float Rd0 = max(0.0, dot(L0, N));
    // float Rd1 = max(0.0, dot(L1, N));
    float Rd0 = abs(dot(L0, N));
    float Rd1 = abs(dot(L1, N));
    vec3 Td = texture2D(diffuseMap, texcoord).rgb;
    vec3 diffuse = Rd0 * Kd * Td * gl_LightSource[0].diffuse.rgb
                +  Rd1 * Kd * Td * gl_LightSource[1].diffuse.rgb;
    
    // Calculate the specular coefficient
    vec3 R0 = reflect(L0, N);
    vec3 R1 = reflect(L1, N);
    float Rs0 = pow(max(0.0, dot(V, R0)), alpha);
    float Rs1 = pow(max(0.0, dot(V, R1)), alpha);
    vec3 Ts = texture2D(specularMap, texcoord).rgb;
//	vec3 specular = Rs * Ks * Ts * gl_LightSource[0].specular.rgb;
    vec3 specular = Rs0 * Ks * gl_LightSource[0].specular.rgb
                +	Rs1 * Ks * gl_LightSource[1].specular.rgb;
        
    // Ambient is easy
    vec3 ambient =  Ka * gl_LightSource[0].ambient.rgb * 0.2 * Td 
                    + Ka * gl_LightSource[1].ambient.rgb * 0.2 * Td;

    // This actually writes to the frame buffer
    gl_FragColor = vec4(diffuse + specular + ambient, 1);
    // gl_FragColor = vec4(Td,1);
}
