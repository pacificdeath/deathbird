#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform sampler2D paletteTex;
uniform int paletteIndex;

out vec4 finalColor;

void main() {
    vec4 texColor = texture(texture0, fragTexCoord);

    int index = int(texColor.r * 255.0 + 0.5);

    vec2 paletteDimensions = textureSize(paletteTex, 0);

    float u = (float(index) + 0.5) / float(paletteDimensions.x);
    float v = (float(paletteIndex) + 0.5) / float(paletteDimensions.y);
    vec3 mappedColor = texture(paletteTex, vec2(u, v)).rgb;

    finalColor = vec4(mappedColor, texColor.a);
}

