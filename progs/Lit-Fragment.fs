#version 300 es
in highp vec3 outcol;
in highp vec3 outnorm;
out mediump vec4 fragColor;
void main()
{
    fragColor = vec4(outnorm, 1.0);
}