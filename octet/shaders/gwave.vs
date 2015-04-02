//////////////////////////////////////////////////////////////////////////////////////////
//
// Default vertex shader for materials. Extend this to deal with bump mapping, defered rendering, shadows etc.
//

// matrices
uniform mat4 modelToProjection;
uniform mat4 modelToCamera;

uniform float waves[100];
uniform int numWaves;

uniform float time;

// attributes from vertex buffer
attribute vec4 pos;
attribute vec2 uv;
attribute vec3 normal;
attribute vec4 color;

// outputs
varying vec3 normal_;
varying vec2 uv_;
varying vec4 color_;
varying vec3 model_pos_;
varying vec3 camera_pos_;


void main() {
  gl_Position = modelToProjection * pos;

  vec3 wnormal=vec3(0,0,0);

  for(int i=0;i<numWaves*5;i+=5)
  {
	
	float amp=length(vec2(waves[i],waves[i+1]));
	float frequency=waves[i+2];
	float convSpeed=waves[i+3];
	float q=waves[i+4];
	vec2 dir=normalize(vec2(waves[i],waves[i+1]));

	wnormal += vec3(
	-(dir.x*frequency*amp *cos(frequency*dot(dir,pos.xz)+time*convSpeed)),
	1-(q*frequency*amp*sin(frequency*dot(dir,pos.xz)+time*convSpeed)),
	-(dir.y*frequency*amp *cos(frequency*dot(dir,pos.xz)+time*convSpeed)));
	}
	
	wnormal=normalize(wnormal);
  vec3 tnormal = (modelToCamera * vec4(wnormal, 0.0)).xyz;
  vec3 tpos = (modelToCamera * pos).xyz;
  normal_ = tnormal;
  uv_ = uv;
  color_ = color;
  camera_pos_ = tpos;
  model_pos_ = pos.xyz;
}

