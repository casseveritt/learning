uniform highp vec3 lightPos;
uniform highp vec3 matCol;
uniform highp vec3 lightCol;
in highp vec3 outcol;
in highp vec3 outnorm;
in highp vec3 outpos;
out mediump vec4 fragColor;
void main()
{
	highp vec3 l = (lightPos - outpos);
	l = normalize(l);
	highp float diffuse = dot(l,outnorm);
	highp vec3 lit = lightCol * matCol * vec3(diffuse);
    fragColor = vec4(lit, 1.0);
}