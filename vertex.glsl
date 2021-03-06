#version 140
attribute vec3 vertexCoord;
attribute vec3 rotation;
attribute vec4 pos;
attribute float inTexCoord;
varying float outTexCoord;
void main()
{
	outTexCoord = inTexCoord;
	gl_Position = 
	mat4(1.0, 0.0, 0.0, 0.0, 0.0, cos(rotation.x), -sin(rotation.x), 0.0, 0.0, sin(rotation.x), cos(rotation.x), 0.0, 0.0, 0.0, 0.0, 1.0) *
	mat4(cos(rotation.y), 0.0, -sin(rotation.y), 0.0, 0.0, 1.0, 0.0, 0.0, sin(rotation.y),  0.0, cos(rotation.y), 0.0, 0.0, 0.0, 0.0, 1.0) *
	vec4(vertexCoord + pos.xyz / pos.w, 1.0 / pos.w);
}
 
