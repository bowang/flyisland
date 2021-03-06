attribute vec3 positionIn;
attribute vec3 normalIn;

varying vec3 normal;
varying vec3 eyePosition;

void main() {

    vec4 eyeTemp = gl_ModelViewMatrix * vec4(positionIn, 1);
    eyePosition = eyeTemp.xyz;

    gl_Position = gl_ProjectionMatrix * eyeTemp;

    normal = normalIn;
}
