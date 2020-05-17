uniform sampler2D samp;
in highp vec3 outcol;
in highp vec2 outtex;
out mediump vec4 fragColor;
void main()
{
    fragColor = texture(samp, outtex);
}