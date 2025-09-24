#version 330

const int PALETTE_SIZE = 32;

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec3 palette[PALETTE_SIZE];

out vec4 finalColor;

void main() {
    // Sample source pixel
    vec4 texColor = texture(texture0, fragTexCoord);

    // Convert red channel back to integer index (0–255)
    int index = int(texColor.r * 255.0 + 0.5);

    // Clamp index to palette size
    index = clamp(index, 0, PALETTE_SIZE - 1);

    // Lookup palette
    vec3 mappedColor = palette[index];

    // Apply original alpha (important for sprites)
    finalColor = vec4(mappedColor, texColor.a);
}

