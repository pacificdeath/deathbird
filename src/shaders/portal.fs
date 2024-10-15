#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform vec2 resolution;
uniform float time;
uniform int color_id;
uniform float direction;
uniform float disappearance;

const float waves = 20;
const float thickness = .15;
const float speed = 10;

void main() {
    vec2 uv = (fragTexCoord * 2) - 1;
    uv.x *= resolution.x / resolution.y;
    float g = length(uv);
    float g2 = g + disappearance;
    float time_param = (time * speed) * direction;
    g = sin((g * waves) + time_param) / waves;
    g = abs(g);
    g = smoothstep(0, thickness, g);
    finalColor = vec4(
        fragColor.r > 0 ? g2 - (g * fragColor.r) : 0,
        fragColor.g > 0 ? g2 - g * fragColor.g : 0,
        fragColor.b > 0 ? g2 - g * fragColor.b : 0,
        smoothstep(0, thickness, 1 - g2)
    );
}