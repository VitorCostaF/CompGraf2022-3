#version 300 es

precision mediump float;

in vec3 fragN;
in vec3 fragLSun;
in vec3 fragLMoon;
in vec3 fragV;
in float kcSun;
in float klSun;
in float kqSun;
in float kcMoon;
in float klMoon;
in float kqMoon;

// Light properties
uniform vec4 Ia, sunId, sunIs;
uniform vec4 moonId, moonIs;

// Material properties
uniform vec4 Ka, sunKd, sunKs;
uniform vec4 moonKd, moonKs;

uniform float sunShininess;
uniform float moonShininess;

out vec4 outColor;

vec4 Phong(vec3 N, vec3 L, vec3 V, vec4 Kd, vec4 Ks, vec4 Id, vec4 Is, float shininess) {
  N = normalize(N);
  L = normalize(L);

  // Compute lambertian term
  float lambertian = max(dot(N, L), 0.0);

  // Compute specular term
  float specular = 0.0;
  if (lambertian > 0.0) {
    // vec3 R = normalize(2.0 * dot(N, L) * N - L);
    vec3 R = reflect(-L, N);
    V = normalize(V);
    float angle = max(dot(R, V), 0.0);
    specular = pow(angle, shininess);
  }

  vec4 diffuseColor = Kd * Id * lambertian;
  vec4 specularColor = Ks * Is * specular;
  

  return diffuseColor + specularColor;
}

void main() {
  vec4 sunColor = Phong(fragN, fragLSun, fragV, sunKd, sunKs, sunId, sunIs, sunShininess);
  vec4 moonColor = Phong(fragN, fragLMoon, fragV, moonKd, moonKs, moonId, moonIs, moonShininess);

  float sunDist = distance(fragLSun, fragV);
  float moonDist = distance(fragLMoon, fragV);

  vec4 ambientColor = Ka * Ia;

  float FattSun = (1.0/(kcSun + klSun*sunDist + kqSun*pow(sunDist,2.0)));
  float FattMoon = (1.0/(kcMoon + klMoon*moonDist + kqMoon*pow(moonDist,2.0)));

  vec4 color = ambientColor + sunColor * FattSun + moonColor * FattMoon;

  if (gl_FrontFacing) {
    outColor = color;
  } else {
    float i = (color.r + color.g + color.b) / 3.0;
    outColor = vec4(i, 0, 0, 1.0);
  }
}