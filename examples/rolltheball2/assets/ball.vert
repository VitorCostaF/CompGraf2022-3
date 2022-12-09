#version 300 es

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;
uniform float KcSun;
uniform float KlSun;
uniform float KqSun;
uniform float KcMoon;
uniform float KlMoon;
uniform float KqMoon;

uniform vec4 sunLight;
uniform vec4 moonLight;

out vec3 fragN;
out vec3 fragLSun;
out vec3 fragLMoon;
out vec3 fragV;
out float kq;
out float kl;
out float kc;

out float kcSun;
out float klSun;
out float kqSun;
out float kcMoon;
out float klMoon;
out float kqMoon;

void main() {
  vec3 P = (viewMatrix * modelMatrix * vec4(inPosition, 1.0)).xyz;
  vec3 N = normalMatrix * inNormal;
  vec3 LSun = -(viewMatrix * sunLight).xyz;
  vec3 LMoon = -(viewMatrix * moonLight).xyz;

  fragLSun = LSun;
  fragLMoon = LMoon;
  fragV = -P;
  fragN = N;

  kcSun = KcSun;
  klSun = KlSun;
  kqSun = KqSun;
  kcMoon = KcMoon;
  klMoon = KlMoon;
  kqMoon = KqMoon;

  gl_Position = projMatrix * vec4(P, 1.0);
}