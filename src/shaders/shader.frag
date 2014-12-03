#version 330

uniform sampler2D waterSampler;
uniform sampler2D sandSampler;
uniform sampler2D grassSampler;
uniform sampler2D rockSampler;
uniform sampler2D snowSampler;

uniform mat4 view_matrix;
uniform highp int nbVertex;

float sandLimit = 0.05;
float grassLimit = 0.4;
float rockLimit = 0.7;
float snowLimit = 1.0;

vec3 light_spec = vec3(1.0,1.0,1.0);
vec3 light_diff = vec3(1.0,1.0,1.0);
vec3 light_amb = vec3(0.2,0.2,0.2);

in vec2 ex_textCoord;
in vec3 ex_normal;
in float ex_height;
in vec4 ex_pos;
out vec4 out_color;

in vec3 eye_position;
in vec3 eye_normal;



void main() {

	vec4 texel;
	if (ex_height < sandLimit) {
		texel = texture2D(waterSampler, ex_textCoord.xy);
	} else {
		vec4 sand = texture2D(sandSampler, ex_textCoord.xy);
		vec4 grass = texture2D(grassSampler, ex_textCoord.xy);
		vec4 rock = texture2D(rockSampler, ex_textCoord.xy);
		vec4 snow = texture2D(snowSampler, ex_textCoord.xy);
		vec4 weights;



		weights.x = clamp(1.0f - abs(ex_height - sandLimit) / 0.2f, 0.0, 1.0);
		weights.y = clamp(1.0f - abs(ex_height - grassLimit) / 0.2f, 0.0, 1.0);
		weights.z = clamp(1.0f - abs(ex_height - rockLimit) / 0.2f, 0.0, 1.0);
		weights.w = clamp(1.0f - abs(ex_height - snowLimit) / 0.2f, 0.0, 1.0);
		weights = weights / (weights.x + weights.y + weights.z + weights.w);
		texel = sand * weights.x + grass * weights.y + rock * weights.z + snow * weights.w;
	}

	vec3 Ia = light_amb * vec3(texel);

	vec3 light_pos = vec3(nbVertex*10, 2000.0, nbVertex*10);

	vec3 light_pos_eye = vec3(view_matrix * vec4(light_pos,1.0));

	vec3 direction = normalize(light_pos_eye - eye_position);

	vec3 Id = light_diff * vec3(texel) * max(dot(direction,eye_normal),0.0);

	// Specular
	vec3 reflection_eye = reflect(-direction, eye_normal);
	float dot_spec = max(dot(reflection_eye, normalize(-eye_position)),0.0);
	float spec_factor = pow(dot_spec, 1000.0);
	vec3 Is = light_spec * vec3(0.0,0.0,0.0) * spec_factor;

	out_color = vec4(Ia+Id+Is, 1.0);
	//out_color = texel;
	//out_color = vec4(ex_normal, 1.0);

}
