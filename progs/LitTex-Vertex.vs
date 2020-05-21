uniform highp mat4 proj;
uniform highp mat4 view;
uniform highp mat4 model;
in highp vec3 pos;
in highp vec3 col;
in highp vec3 norm;
in highp vec2 texCoord;
out highp vec3 fcol;
out highp vec3 fnorm;
out highp vec3 fpos;
out highp vec2 ftex;
void main()
{
    gl_Position = (proj * (view * (model * vec4(pos, 1.0))));
    fcol = col;
    fnorm = norm;
    fpos = (model * vec4(pos, 1.0)).xyz;
    ftex = texCoord;
}