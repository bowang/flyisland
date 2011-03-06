uniform sampler2D diffuseTex;
uniform sampler2D specularTex;
uniform sampler2D normalTex;
// uniform sampler2D shadowTex;

uniform vec3 Kd;
uniform vec3 Ks;
uniform vec3 Ka;
uniform float alpha;

varying vec2 texcoord;
varying vec3 normal;
varying vec3 tangent;
varying vec3 bitangent;
varying vec3 eyePosition;
// varying vec4 shadowcoord;

void main() {

    vec4 norm = vec4(texture2D(normalTex, texcoord).rgb, 1);
    mat4 expand = mat4(2,0,0,0, 0,2,0,0, 0,0,2,0, -1,-1,-1,1);
    norm = expand * norm;
    vec3 t = normalize(tangent);
    vec3 b = normalize(bitangent);
    vec3 n = normalize(normal);
    vec3 N = norm.x*t + norm.y*b + norm.z*n;
    N = normalize(N);

    vec3 L0 = normalize(gl_LightSource[0].position.xyz);
    vec3 L1 = normalize(gl_LightSource[1].position.xyz - eyePosition);
    vec3 V = normalize(-eyePosition);
    
    // Calculate the diffuse color coefficient, and sample the diffuse texture
    float Rd0 = max(0.0, dot(L0, N));
    float Rd1 = max(0.0, dot(L1, N));
    vec3 Td = texture2D(diffuseTex, texcoord).rgb;
    vec3 diffuse0 = Rd0 * Kd * Td * gl_LightSource[0].diffuse.rgb;
    vec3 diffuse1 = Rd1 * Kd * Td * gl_LightSource[1].diffuse.rgb;
    
    // Calculate the specular coefficient
    vec3 R0 = reflect(-L0, N);
    vec3 R1 = reflect(-L1, N);
    float Rs0 = pow(max(0.0, dot(V, R0)), alpha);
    float Rs1 = pow(max(0.0, dot(V, R1)), alpha);
    vec3 Ts = texture2D(specularTex, texcoord).rgb;
    vec3 specular0 = Rs0 * Ks * Ts * gl_LightSource[0].specular.rgb;
    vec3 specular1 = Rs1 * Ks * Ts * gl_LightSource[1].specular.rgb;

    // Ambient is easy
    vec3 ambient0 = Ka * gl_LightSource[0].ambient.rgb;
    vec3 ambient1 = Ka * gl_LightSource[1].ambient.rgb;

/*
    vec4 shadowcoord_w = shadowcoord / shadowcoord.w;
    shadowcoord_w.z -= 0.001;
    float distanceFromLight = texture2D(shadowTex,shadowcoord_w.xy).z;
    // float distanceFromLight = texture2D(shadowTex,shadowcoord.xy).z;

    float shadow = distanceFromLight < shadowcoord_w.z ? 0.3 : 1.0 ;
*/
    gl_FragColor = vec4( (diffuse0 + specular0 + ambient0)
                        + diffuse1 + specular1 + ambient1 , 1);
}
