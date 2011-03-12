uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;

uniform vec3 Kd;
uniform vec3 Ks;
uniform vec3 Ka;
uniform float alpha;

varying vec2 texcoord;
varying vec3 normal;
varying vec3 tangent;
varying vec3 bitangent;
varying vec3 eyePosition;

void main() {

    vec4 norm = vec4(texture2D(normalMap, texcoord).rgb, 1);
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
    
    float Rd0 = max(0.0, dot(L0, N));
    float Rd1 = max(0.0, dot(L1, N));
    vec3 Td = texture2D(diffuseMap, texcoord).rgb;
    vec3 diffuse =  Rd0 * Kd * Td * gl_LightSource[0].diffuse.rgb
                  + Rd1 * Kd * Td * gl_LightSource[1].diffuse.rgb;

    vec3 R0 = reflect(-L0, N);
    vec3 R1 = reflect(-L1, N);
    float Rs0 = pow(max(0.0, dot(V, R0)), alpha);
    float Rs1 = pow(max(0.0, dot(V, R1)), alpha);
    vec3 Ts = texture2D(specularMap, texcoord).rgb;
    vec3 specular =  Rs0 * Ks * Ts * gl_LightSource[0].specular.rgb
                   + Rs1 * Ks * Ts * gl_LightSource[1].specular.rgb;

    vec3 ambient = Ka * (gl_LightSource[0].ambient.rgb + gl_LightSource[1].ambient.rgb);

    gl_FragColor = vec4(diffuse + specular + ambient, 1);
}
