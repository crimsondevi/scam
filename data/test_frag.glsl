#version 330 core

in vec2 Frag_UV;
in vec4 Frag_Color;

uniform sampler2D Texture;
uniform float Time;
uniform float PerInstanceRandom;

out vec4 Out_Color;

void main()
{
    vec4 c = Frag_Color * texture(Texture, Frag_UV);
    c.rgb += pow(0.5 + 0.5 * sin(-(Frag_UV.x + Frag_UV.y * .5) + (-PerInstanceRandom + Time) * 2.0), 200.0);
    Out_Color = c;
}
