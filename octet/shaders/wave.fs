//////////////////////////////////////////////////////////////////////////////////////////
//
// default frament shader for textures
//

// constant parameters
uniform vec4 lighting[17];
uniform int num_lights;
uniform sampler2D diffuse_sampler;

// inputs
varying vec3 normal_;
varying vec3 camera_pos_;
varying vec2 uv_;
varying vec4 color_;
varying vec3 model_pos_;
varying vec3 trueNormal_;

void main() {
  vec4 diffuse = texture2D(diffuse_sampler, uv_);
  vec3 nnormal = normalize(normal_);
  vec3 npos = camera_pos_;
  vec3 toEye=normalize(-camera_pos_);
  vec3 diffuse_light = vec3(0,0,1);
  for (int i = 0; i != num_lights; ++i) {
    vec3 light_pos = lighting[i * 4 + 1].xyz;
    vec3 light_direction = lighting[i * 4 + 2].xyz;
    vec3 light_color = lighting[i * 4 + 3].xyz;
    vec3 light_atten = lighting[i * 4 + 4].xyz;

    vec3 dir=light_direction;
    float dist=length(dir);
    dir/=dist;
    float diffuse_factor = dot(nnormal,dir);

    float specFactor=0;
    if(diffuse_factor>0.0)
    {
    vec3 v=reflect(dir,nnormal);
    
    specFactor=pow(max(dot(v,toEye),0.0f),8.0);
    }
    vec3 specColor=(0.1,0.1,0.1,0.5f);

    float att =1.0;
   
    diffuse_light +=  diffuse_factor * light_color * att+specColor*specFactor *att;
  }
 //gl_FragColor=vec4(nnormal.xyz,1.0);
  gl_FragColor = vec4(diffuse.xyz * diffuse_light, 1);
}

