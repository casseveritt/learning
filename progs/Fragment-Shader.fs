#version 410
//#version 300 es
in highp vec3 outcol;
out mediump vec4 fragColor;
void main()
{
    fragColor = vec4(outcol, 1.0);
}