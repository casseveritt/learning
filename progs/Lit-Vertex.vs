#version 300 es
uniform highp mat4 proj;
uniform highp mat4 view;
uniform highp mat4 model;
in highp vec3 pos;
in highp vec3 col;
in highp vec3 norm;
out highp vec3 outcol;
out highp vec3 outnorm;
out highp vec3 outpos;
void main()
{
    gl_Position = (proj * (view * (model * vec4(pos, 1.0))));
    outcol = col;
    outnorm = norm;
    outpos = (model * vec4(pos, 1.0)).xyz;
}