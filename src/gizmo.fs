#version 330

in vec2 fragTexCoord;

out vec4 finalColor;

uniform sampler2D texture0;

// Input
uniform int width;
uniform int height;
uniform vec4 outlineColor;

void main()
{
    float x = 1.0 / width;
    float y = 1.0 / height;

    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, fragTexCoord);

    if (texelColor.rgba == vec4(0.0f, 0.0f, 0.0f, 0.0f)) { discard; }

    finalColor = texelColor;

    const int size = 1;

    for (int i = -size; i <= size; ++i)
    {
        for (int j = -size; j <= size; ++j)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }

            if (texture(texture0, fragTexCoord + vec2(i, j) * vec2(x, y)).rgba == vec4(0.0f, 0.0f, 0.0f, 0.0f))
            {
                finalColor = outlineColor;
                return;
            }
        }
    }

}