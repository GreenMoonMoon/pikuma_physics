#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

vec3 hue_shift(vec3 color, float hue) {
    const vec3 k = vec3(0.57735, 0.57735, 0.57735);
    float cosAngle = cos(hue);
    return vec3(col * cosAngle + cross(k, col) * sin(hue) + k * dot(k, col) * (1.0 - cosAngle));
}

void main()
{
    vec3 shifted_color = hue_shift(colDiffuse.rgb, 0.5);

    // Calculate final fragment color
    finalColor = vec4(mix(colDiffuse.rgb, shifted_color, fragTexCoord.y), 1.0);
}