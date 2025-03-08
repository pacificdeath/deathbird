// bird palette indices
// outline              : 0x00
// body                 : 0x10
// beak 1               : 0x20
// beak 2               : 0x30
// eyes                 : 0x40
// gore 1               : 0x50
// gore 2               : 0x60
// gore 3               : 0x70
// umbrella handle      : 0x80
// umbrella canopy 1    : 0x90
// umbrella canopy 2    : 0xA0
// explosion 1          : 0xB0
// explosion 2          : 0xC0
// explosion 3          : 0xD0
// explosion 4          : 0xE0
// extra                : 0xF0

#version 330

const int PALETTE_SIZE = 16;

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec3 palette[PALETTE_SIZE];

out vec4 finalColor;

void main() {
    vec4 texColor = texture(texture0, fragTexCoord);
    float step = 1.0 / float(PALETTE_SIZE);
    int index = int(
        (texColor.r + (step / 2.0)) * float(PALETTE_SIZE)
    );
    vec3 mappedColor = palette[index];
    finalColor = vec4(mappedColor, texColor.a);
}
