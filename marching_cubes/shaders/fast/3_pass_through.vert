#version 430

in uint inCaseMarker;

out uint edgeMarker;

void main() {
    edgeMarker = inCaseMarker;
}