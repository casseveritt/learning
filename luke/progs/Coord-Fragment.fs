uniform sampler2D samp;
in highp vec3 outcol;
in highp vec2 outtex;
out mediump vec4 fragColor;
void main()
{
    fragColor = vec4(fract(outtex.x), fract(outtex.y), 0.0, 1.0);
}