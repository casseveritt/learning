#version 320
//#version 300 es
uniform highp vec3 lightPos;
in highp vec3 outcol;
in highp vec3 outnorm;
in highp vec3 outpos;
out mediump vec4 fragColor;
void main()
{
	highp vec3 l = lightPos - outpos;
	l = normalize(l);
	highp float d = dot(l,outnorm);
    fragColor = vec4(d,d,d, 1.0);
}