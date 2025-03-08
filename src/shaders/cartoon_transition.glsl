#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform vec2 resolution;
uniform float size;

void main() {
    vec2 uv = (fragTexCoord * 2) - 1;
    uv.x *= resolution.x / resolution.y;
    if (length(uv) < size) {
        finalColor = vec4(1.0,0.0,0.0,0.0);
    } else {
        finalColor = vec4(0.0,0.0,0.0,1.0);
    }
}

