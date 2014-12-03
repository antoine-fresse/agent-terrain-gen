#version 330

in highp float posAttr;
in vec3 normalAttr;
out float ex_height;
out vec2 ex_textCoord;
out vec3 ex_normal;

out vec3 eye_position;
out vec3 eye_normal;

uniform highp int nbVertex;
uniform highp float pointPerTexture;
uniform highp float maxHeight;

uniform highp mat4 view_matrix;
uniform highp mat4 model_matrix;
uniform highp mat4 proj_matrix;

void main() {
	ex_height = posAttr / maxHeight;
	ex_textCoord = vec2(mod(gl_VertexID, nbVertex), floor(gl_VertexID / nbVertex)) / pointPerTexture;
	ex_normal = normalize(normalAttr);
	vec4 ex_pos =  vec4(mod(gl_VertexID, nbVertex), posAttr, floor(gl_VertexID / nbVertex), 1.0);

	mat4 normal_matrix = transpose(inverse(view_matrix*model_matrix));
	eye_position = vec3(view_matrix*model_matrix*ex_pos);
	eye_normal = normalize(vec3(normal_matrix*vec4(ex_normal,0.0)));

	gl_Position = proj_matrix*vec4(eye_position, 1.0);
}
