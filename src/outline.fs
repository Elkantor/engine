#version 330

in vec2 fragTexCoord;

out vec4 finalColor;

uniform sampler2D texture0;

// Input
uniform int size;
uniform int width;
uniform int height;
uniform vec4 color;
uniform vec4 pickingColor;

void main()
{
    float x = 1.0 / width;
    float y = 1.0 / height;

    if (pickingColor.rgb == vec3(1.0f, 1.0f, 1.0f)) discard;

    if (texture(texture0, fragTexCoord).rgb == pickingColor.rgb)
    {
        for (int i = -size; i <= size; ++i)
        {
            for (int j = -size; j <= size; ++j)
            {
                if (i == 0 && j == 0)
                {
                    continue;
                }

                if (texture(texture0, fragTexCoord + vec2(i, j) * vec2(x, y)).rgb != pickingColor.rgb)
                {
                    finalColor = color;
                    return;
                }
            }
        }
    }
}