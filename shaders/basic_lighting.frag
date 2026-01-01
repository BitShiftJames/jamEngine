#version 330 core

// based on raylib's basic lighting example.

in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragPosition;
in vec3 fragNormal;

out vec4 finalColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec4 ambient;
uniform vec3 cameraPosition;
uniform vec3 cameraTarget;
uniform vec4 lightColor;

void main() {
  vec4 texColor = texture(texture0, fragTexCoord);
  vec3 normal = normalize(fragNormal);
  vec3 viewD = normalize(cameraPosition - fragPosition);

  vec4 tint = fragColor * colDiffuse;

  vec3 light = -normalize(cameraTarget - cameraPosition);

  float NdotL = max(dot(normal, light), 0.0);
  vec3 lightDot = lightColor.rgb * NdotL;

  vec3 specular = step(0.0, NdotL) * pow(max(0.0, dot(viewD, reflect(-(light), normal))), 16.0);
  
  finalColor = (texColor*((tint + vec4(specular, 1.0))*vec4(lightDot, 1.0)));
  finalColor += texColor*(ambient/10.0)*tint;

}
