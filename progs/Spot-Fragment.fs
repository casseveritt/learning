uniform highp mat4 lightFromWorld;
uniform highp mat4 worldFromLight;
//uniform highp vec3 matDifCol;
uniform highp vec3 lightCol;
uniform highp vec3 matSpcCol;
uniform highp float shiny;
uniform highp vec3 camPos;
uniform sampler2D samp;
in highp vec3 fcol;
in highp vec3 fnorm;
in highp vec3 fpos;
in highp vec2 ftex;
out mediump vec4 fragColor;
void main()
{
	highp vec3 lightPos = (worldFromLight * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
	highp vec3 matDifCol = texture(samp, ftex).xyz;

	highp vec3 fposInLight = (lightFromWorld * vec4(fpos, 1.0)).xyz;

	fposInLight /= fposInLight.z;
	highp float radius = sqrt( fposInLight.x * fposInLight.x + fposInLight.y * fposInLight.y);

	highp float cone = 1.0;
	if (radius > 1.0)
	{
		cone = 0.0;
	}

	highp vec3 n = normalize(fnorm);
	highp vec3 v = normalize(camPos - fpos);
	highp vec3 i = normalize(fpos - lightPos);
	highp vec3 r = i - 2.0 * n * dot(n, i);
	highp float specular = pow(max( 0.0, dot(r,v)), shiny);
	highp vec3 spcLit = lightCol * matSpcCol * vec3(specular);

	highp vec3 l = normalize(lightPos - fpos);
	highp float diffuse = cone * max(0.0, dot(l,n));
	highp vec3 difLit = lightCol * matDifCol * vec3(diffuse);
	fragColor = vec4(difLit, 1.0) + vec4(spcLit, 1.0);
}