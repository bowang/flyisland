uniform samplerCube cubeTex;
uniform mat4 inverseView;

varying vec3 normal;
varying vec3 eyePosition;

void main() {

    vec3 gold = vec3(255.0, 242.0, 168.0) / 255.0;
    vec3 N = gl_NormalMatrix * normalize(normal);
    vec3 V = normalize(eyePosition);
    vec3 R = reflect(V, N);
    vec4 cubecoord = inverseView * vec4(R,0);

    vec4 color = textureCube(cubeTex, cubecoord.xyz);
    gl_FragColor = color * vec4(gold, 1.0);
    // gl_FragColor = vec4(cubecoord.xyz, 1);
}
