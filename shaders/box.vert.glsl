attribute vec3 positionIn;

void main() {

    gl_Position = gl_ModelViewProjectionMatrix * vec4(positionIn, 1.0);

}
