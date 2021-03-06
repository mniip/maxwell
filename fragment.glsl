#version 140
varying float outTexCoord;
vec3 hue(float h) { h = mod(h, 6.0); if(mod(h, 0.3) < 0.02) return vec3(0.0, 0.0, 0.0); h -= mod(h, 0.3); return h < 1.0 ? vec3(1.0, h, 0.0) : h < 2.0 ? vec3(2.0 - h, 1.0, 0.0) : h < 3.0 ? vec3(0.0, 1.0, h - 2.0) : h < 4.0 ? vec3(0.0, 4.0 - h, 1.0) : h < 5.0 ? vec3(h - 4.0, 0.0, 1.0) : vec3(1.0, 0.0, 6.0 - h); }
out vec4 LFragment;
void main()
{
	if(outTexCoord > 2.5)
		LFragment = vec4(1.0, outTexCoord - 3.0, 0.0, 1.0);
	else
		LFragment = vec4(0.0, 1.0, outTexCoord - 1.0, 1.0);
}
 
