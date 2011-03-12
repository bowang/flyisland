attribute vec3 positionIn;
attribute float alphaIn;
uniform float baseSize;

varying float alpha;

const float screenWidth = 1024.0;

void main() {

    gl_Position = gl_ModelViewProjectionMatrix * vec4(positionIn, 1.0);

    gl_PointSize = baseSize * screenWidth / (1.0 + gl_Position.z);

    alpha = alphaIn;

}
