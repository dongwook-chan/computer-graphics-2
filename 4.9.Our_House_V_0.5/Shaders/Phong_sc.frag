#version 430

struct LIGHT {
	vec4 position;
	vec4 ambient_color, diffuse_color, specular_color;
	vec4 light_attenuation_factors;
	vec3 spot_direction;
	float spot_exponent;
	float spot_cutoff_angle;
	bool light_on;
};

struct MATERIAL {
	vec4 ambient_color;
	vec4 diffuse_color;
	vec4 specular_color;
	vec4 emissive_color;
	float specular_exponent;
};

uniform vec4 u_global_ambient_color;
#define NUMBER_OF_LIGHTS_SUPPORTED 4
uniform LIGHT u_light[NUMBER_OF_LIGHTS_SUPPORTED];
uniform MATERIAL u_material;
uniform int isTiger;
uniform int isBuilding;
uniform int isSide;
uniform int isFront;
uniform int freq;
uniform int density;
uniform int time;
uniform int screen_x;
uniform int screen_y;

// somewhat awkward extension
uniform int screen_effect = 0;
uniform float screen_width = 0.125f;

uniform bool u_blind_effect = false;

uniform bool u_cartoon_effect = false;
uniform float u_cartoon_levels = 3.0f;

const float zero_f = 0.0f;
const float one_f = 1.0f;

in vec3 v_position_EC;
in vec3 v_normal_EC;
in vec2 v_position_sc;
in vec3 v_position;

layout(location = 0) out vec4 final_color;

vec4 lighting_equation(in vec3 P_EC, in vec3 N_EC) {
	int scatter = 0;

	vec4 color_sum;
	float local_scale_factor, tmp_float;
	vec3 L_EC;

	color_sum = u_material.emissive_color + u_global_ambient_color * u_material.ambient_color;

	for (int i = 0; i <NUMBER_OF_LIGHTS_SUPPORTED; i++) {
		if (!u_light[i].light_on) continue;

		local_scale_factor = one_f;
		if (u_light[i].position.w != zero_f) {
			L_EC = u_light[i].position.xyz - P_EC.xyz;

			if (u_light[i].light_attenuation_factors.w != zero_f) {
				vec4 tmp_vec4;

				tmp_vec4.x = one_f;
				tmp_vec4.z = dot(L_EC, L_EC);
				tmp_vec4.y = sqrt(tmp_vec4.z);
				tmp_vec4.w = zero_f;
				local_scale_factor = one_f / dot(tmp_vec4, u_light[i].light_attenuation_factors);
			}

			L_EC = normalize(L_EC);

			if (u_light[i].spot_cutoff_angle < 180.0f) {
				float spot_cutoff_angle = clamp(u_light[i].spot_cutoff_angle, zero_f, 90.0f);
				vec3 spot_dir = normalize(u_light[i].spot_direction);

				tmp_float = dot(-L_EC, spot_dir);
				if (tmp_float >= cos(radians(spot_cutoff_angle))) {
					if (isTiger >0){
						if(isTiger == 1){// && acos(tmp_float) < 1.0f) {
							tmp_float = pow(tmp_float, u_light[i].spot_exponent) * cos(freq * acos(tmp_float));
							//tmp_float = mod(pow(2.0f,tmp_float + freq*cos(acos(tmp_float))), 2.0f);
							tmp_float = pow(0.5f, tmp_float) - 1.0f;
						}
						if(isTiger == 2 ) {
							if(acos(tmp_float) < 0.3f)
								tmp_float = 0;
							//tmp_float = pow(tmp_float, u_light[i].spot_exponent) * cos(time*10.0f * acos(tmp_float));
							//tmp_float = mod(pow(2.0f,tmp_float + time*cos(acos(tmp_float))), 2.0f);
							tmp_float = cos(time * acos(tmp_float));
							//temp_float = mo


							//if(acos(tmp_float))
							//tmp_float = cos(freq*acos(tmp_float))*10*acos(tmp_float);
							//tmp_float = tmp_float*(time/10.0f);
							//tmp_float = time*tmp_float*acos(tmp_float);
							//else if(acos(tmp_float)<2.0f)
							//	tmp_float = 0;
						}
					}
					else tmp_float = pow(tmp_float, u_light[i].spot_exponent);

					if (tmp_float < zero_f) tmp_float = zero_f;
				}
				else
					tmp_float = zero_f;
					local_scale_factor *= tmp_float;
			}
		}
		else {
			L_EC = normalize(u_light[i].position.xyz);
		}

		if (local_scale_factor > zero_f) {
			vec4 local_color_sum = u_light[i].ambient_color * u_material.ambient_color;

			tmp_float = dot(N_EC, L_EC);
			if (tmp_float > zero_f) {
				if (u_cartoon_effect)
					local_color_sum += u_light[i].diffuse_color*u_material.diffuse_color*floor(tmp_float*u_cartoon_levels) / u_cartoon_levels;
					//local_color_sum = pow(2.0f,local_color_sum);
				else {
					local_color_sum += u_light[i].diffuse_color*u_material.diffuse_color*tmp_float;

					vec3 H_EC = normalize(L_EC - normalize(P_EC));
					tmp_float = dot(N_EC, H_EC);
					if (tmp_float > zero_f) {
						local_color_sum += u_light[i].specular_color*u_material.specular_color*pow(tmp_float, u_material.specular_exponent);
					}
				}
			}
			color_sum += local_scale_factor * local_color_sum;
		}
	}
	return color_sum;
}

void punc(float x_min, float y_min, float r){
	int remove = 1;
	int hor = 1;
	int side = 1;
	float x = 1.0f;
	float y = 1.0f;
	float i;

	while(x > x_min && y > y_min){
		if(remove == 1){
			if(hor == 1){
				if (side == 1){
					for(i=0;i<1.0f;i+=2*y*r){
						if(i< v_position_sc.y && v_position_sc.y <=i + y*r && 0< v_position_sc.x&& v_position_sc.x <=r*x){
							discard; //return;
						}
					}
				}
			}
			else{
				if (side == 1){
					for(i=0;i<1.0f;i+=2*x*r){
						if(i< v_position_sc.x&& v_position_sc.x <=i+x*r && 0< v_position_sc.y && v_position_sc.y <=r*y){
							discard; //return;
						}
					}
				}
			}
		}
		if (hor == 1)
			y *= r;
		else
			x *= r;
		hor = 1 - hor; remove = 1 - remove;
	}
}

void main(void) {
	switch(screen_effect){
	case 1:
		float x_mod, y_mod;
		x_mod = mod(v_position_sc.x*screen_x, 1.0f);
		y_mod = mod(v_position_sc.y*screen_y, 1.0f);

		if ((x_mod > screen_width) && (x_mod < 1.0f - screen_width) &&
			(y_mod > screen_width) && (y_mod < 1.0f - screen_width))
			discard;
		break;
	case 2:
		x_mod = v_position_sc.x*220.f + 10.0f;
		y_mod = v_position_sc.y*150.f + 10.0f;
		if((15.0f < x_mod) && (x_mod < 65.0f) && (115.0f < y_mod) && (y_mod < 155.0f))
			discard;
		if((15.0f < x_mod) && (x_mod < 65.0f) && 65.0f< y_mod && y_mod <110.0f)
			discard;
		if((15.0f < x_mod) && (x_mod < 55.0f) && 15.0f< y_mod && y_mod <60.0f)
			discard;
		if((15.0f < x_mod) && (x_mod < 30.0f) && 105.0f< y_mod && y_mod <120.0f)
			discard;
		if((60.0f < x_mod) && (x_mod < 95.0f) && 15.0f< y_mod && y_mod <95.0f)
			discard;
		if((70.0f < x_mod) && (x_mod < 135.0f) && 85.0f< y_mod && y_mod <120.0f)
			discard;
		if((115.0f < x_mod) && (x_mod < 165.0f) && 55.0f< y_mod && y_mod <70.0f)
			discard;
		if((100.0f < x_mod) && (x_mod < 120.0f) && 15.0f< y_mod && y_mod <75.0f)
			discard;
		if((115.0f < x_mod) && (x_mod < 135.0f) && 70.0f< y_mod && y_mod <95.0f)
			discard;
		if((100.0f < x_mod) && (x_mod < 165.0f) && 15.0f< y_mod && y_mod <25.0f)
			discard;
		if((125.0f < x_mod) && (x_mod < 165.0f) && 15.0f< y_mod && y_mod <50.0f)
			discard;
		if((130.0f < x_mod) && (x_mod < 180.0f) && 90.0f< y_mod && y_mod <120.0f)
			discard;
		if((150.0f < x_mod) && (x_mod < 180.0f) && 90.0f< y_mod && y_mod <155.0f)
			discard;
		if((170.0f < x_mod) && (x_mod < 180.0f) && 140.0f< y_mod && y_mod <155.0f)
			discard;
		if((70.0f < x_mod) && (x_mod < 145.0f) && 125.0f< y_mod && y_mod <155.0f)
			discard;
		if((130.0f < x_mod) && (x_mod < 160.0f) && 140.0f< y_mod && y_mod <155.0f)
			discard;
		if((90.0f < x_mod) && (x_mod < 125.0f) && 80.0f< y_mod && y_mod <120.0f)
			discard;
		if((50.0f < x_mod) && (x_mod < 70.0f) && 15.0f< y_mod && y_mod <30.0f)
			discard;
		if((120.0f < x_mod) && (x_mod < 165.0f) && 55.0f< y_mod && y_mod <85.0f)
			discard;
		if((170.0f < x_mod) && (x_mod < 225.0f) && 40.0f< y_mod && y_mod <85.0f)
			discard;
		if((70.0f < x_mod) && (x_mod < 145.0f) && 125.0f< y_mod && y_mod <155.0f)
			discard;
		if((170.0f < x_mod) && (x_mod < 225.0f) && 15.0f< y_mod && y_mod <35.0f)
			discard;
		if((170.0f < x_mod) && (x_mod < 205.0f) && 25.0f< y_mod && y_mod <50.0f)
			discard;
		if((195.0f < x_mod) && (x_mod < 225.0f) && 40.0f< y_mod && y_mod <130.0f)
			discard;
		if((185.0f < x_mod) && (x_mod < 225.0f) && 135.0f< y_mod && y_mod <155.0f)
			discard;
		if((185.0f < x_mod) && (x_mod < 225.0f) && 135.0f< y_mod && y_mod <155.0f)
			discard;
		if((185.0f < x_mod) && (x_mod < 225.0f) && 90.0f< y_mod && y_mod <130.0f)
			discard;
		if((210.0f < x_mod) && (x_mod < 225.0f) && 110.0f< y_mod && y_mod <140.0f)
			discard;
		if((170.0f < x_mod) && (x_mod < 200.0f) && 90.0f< y_mod && y_mod <105.0f)
			discard;
		break;
	case 3:
		float x_min = 1/240.0f;
		float y_min = 1/150.0f;	
		float div = 1.0f/(density);

		punc(x_min, y_min, div);

		break;
	}
	if(isBuilding == 1 && v_position.z > 49.99)
		discard;
	if(isBuilding == 1 && isFront == 1&& v_position.x <= 15.0)
		discard;
	if(isBuilding == 1 && isSide == 1 && v_position.y <= 15.0)
		discard;
	if(isBuilding == 1 && isFront == 2&& (v_position.x <= 15.0 || v_position.y <= 15.0))
		discard;
	final_color = lighting_equation(v_position_EC, normalize(v_normal_EC));
}