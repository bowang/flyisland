attribute vec3 positionIn;
attribute vec2 texcoordIn;
attribute vec3 normalIn;
attribute vec3 tangentIn;
attribute vec3 bitangentIn;

varying vec2 texcoord;
varying vec3 normal;
varying vec3 tangent;
varying vec3 bitangent;
varying vec3 eyePosition;

void main() {

    vec4 eyeTemp = gl_ModelViewMatrix * vec4(positionIn, 1);
    eyePosition = eyeTemp.xyz;

    gl_Position = gl_ProjectionMatrix * eyeTemp;

    normal    = gl_NormalMatrix * normalIn;
    tangent   = tangentIn;
    bitangent = bitangentIn;
    texcoord  = texcoordIn;

}
