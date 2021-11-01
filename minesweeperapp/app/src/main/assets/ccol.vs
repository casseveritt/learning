uniform highp mat4 proj;
uniform highp mat4 view;
uniform highp mat4 model;
in highp vec3 pos;
in highp vec3 col;
out highp vec3 outcol;
void main()
{
    gl_Position = (proj * (view * (model * vec4(pos, 1.0))));
    outcol = col;
}