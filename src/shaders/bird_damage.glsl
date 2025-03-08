#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

out vec4 finalColor;

void main() {
    vec4 source = texture(texture0, fragTexCoord);
    if (source.a == 0) {
        finalColor = vec4(1,1,1,0);
    } else {
        finalColor = vec4(1,1,1,1);
    }
}