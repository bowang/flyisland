// uniform mat4 inverseView;

attribute vec3 positionIn;
attribute vec2 texcoordIn;
attribute vec3 normalIn;
// attribute vec3 tangentIn;
// attribute vec3 bitangentIn;

varying vec3 eyePosition;
varying vec2 texcoord;
varying vec3 normal;
// varying vec3 tangent;
// varying vec3 bitangent;
// varying vec4 shadowcoord;

void main() {

    vec4 eyeTemp = gl_ModelViewMatrix * vec4(positionIn, 1);
    eyePosition = eyeTemp.xyz;
    gl_Position = gl_ProjectionMatrix * eyeTemp;

/*
    mat4 modelMat = inverseView * gl_ModelViewMatrix;
    shadowcoord = gl_TextureMatrix[7] * (modelMat * vec4(positionIn, 1));
    mat4 bias = mat4(0.5, 0.0, 0.0, 0.0, 
                     0.0, 0.5, 0.0, 0.0, 
                     0.0, 0.0, 0.5, 0.0, 
                     0.5, 0.5, 0.5, 1.0);
    shadowcoord = bias * shadowcoord;
*/

    normal    = gl_NormalMatrix * normalIn;
//    tangent   = tangentIn;
//    bitangent = bitangentIn;
    texcoord  = texcoordIn;
}
