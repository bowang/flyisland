uniform sampler2D particleMap;
varying float alpha;

void main() {

    vec3 texcoord = gl_TexCoord[4];

    gl_FragColor = texture2D(particleMap, texcoord) * alpha;

}
