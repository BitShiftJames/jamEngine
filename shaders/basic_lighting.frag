#version 330 core

// based on raylib's basic lighting example.

in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragPosition;
in vec3 fragNormal;
in mat3 TBN;

out vec4 finalColor;

uniform sampler2D texture0;
uniform sampler2D normalMap;
uniform sampler2D roughMap;
uniform vec4 colDiffuse;

uniform vec4 ambient;
uniform vec3 cameraPosition;
uniform vec3 cameraTarget;
uniform vec4 lightColor;

void main() {
  // tex color
  vec4 texColor = texture(texture0, fract(fragTexCoord));
  vec4 tint = fragColor * colDiffuse;

  // 
  vec3 spotDir = -normalize(cameraTarget - cameraPosition);
  vec3 lightDir = normalize(cameraPosition - fragPosition);
  vec3 halfwayDir = normalize(spotDir + lightDir);

  //
  float theta = dot(spotDir, lightDir);
  float hardcutOff = 0.906;
  float softcutOff = 0.819;

  //vec3 normal = normalize(fragNormal);
  vec3 normal = texture(normalMap, vec2(fragTexCoord.x, fragTexCoord.y)).rgb;
  float roughness = texture(roughMap, vec2(fragTexCoord.x, fragTexCoord.y)).r;
  normal = normalize(normal*2.0 - 1.0);
  normal = normalize(normal*TBN);

  //
  float NdotL = smoothstep(softcutOff, hardcutOff, theta) * max(dot(normal, lightDir), 0.0);
  vec3 lightDot = lightColor.rgb * NdotL;

  // Calculate specular
  float shininess = mix(64.0, 0.0, roughness);
  float specStrength = 1.0 - roughness;
  float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

  vec3 specular = lightColor.rgb * spec * specStrength;
  ////////

  finalColor = (texColor*((tint + vec4(specular, 1.0))*vec4(lightDot, 1.0)));
  finalColor += texColor*(ambient/10.0)*tint;
  //finalColor = vec4(normal, 1.0);
}
