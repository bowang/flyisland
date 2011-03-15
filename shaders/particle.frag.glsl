uniform sampler2D particleMap;
varying float alpha;
varying vec3 texcoord_in;

void main() {

    vec3 texcoord = gl_TexCoord[4].xyz;

    gl_FragColor = texture2D(particleMap, texcoord.xy) * alpha;

}
