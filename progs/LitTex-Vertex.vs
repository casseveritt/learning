uniform highp mat4 proj;
uniform highp mat4 view;
uniform highp mat4 model;
in highp vec3 pos;
in highp vec3 col;
in highp vec3 norm;
in highp vec2 texCoord;
out highp vec3 colo;
out highp vec3 norma;
out highp vec3 posi;
out highp vec2 texcoords;
void main()
{
    gl_Position = (proj * (view * (model * vec4(pos, 1.0))));
    colo = col;
    norma = norm;
    posi = (model * vec4(pos, 1.0)).xyz;
    texcoords = texCoord;
}