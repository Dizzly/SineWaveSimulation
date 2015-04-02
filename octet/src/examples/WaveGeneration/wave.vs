//////////////////////////////////////////////////////////////////////////////////////////
//
// Default vertex shader for materials. Extend this to deal with bump mapping, defered rendering, shadows etc.
//

// matrices
uniform mat4 modelToProjection;
uniform mat4 modelToCamera;

uniform vec4 waves[20];
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

  for(int i=0;i<numWaves;++i)
  {
	float amp=length(waves[i].xy);
	vec2 dir=normalize(waves[i].xy);

	wnormal += vec3(
	-(waves[i].z* dir.x * amp *
            cos(dot(pos.xz,dir) * waves[i].z + time * waves[i].w)),
            1,
            -(waves[i].z* dir.y * amp *
            cos(dot(pos.xz,dir) * waves[i].z + time * waves[i].w)));

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

