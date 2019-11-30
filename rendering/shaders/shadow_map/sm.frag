#version 430

void main() {
    gl_FragDepth = 0.5*gl_FragCoord.z;
}