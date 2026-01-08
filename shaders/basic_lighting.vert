#version 330 core

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexTangent;
in vec4 vertexColor;

uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matNormal;

out vec3 fragPosition;
out vec2 fragTexCoord;
out vec3 fragNormal;
out vec4 fragColor;

// Used to rotate the tangent toward mesh normal.
out mat3 TBN;


void main() {

  // Compute binormal from vertex normal and tangent. W component is the tangent handedness
  vec3 vertexBinormal = cross(vertexNormal, vertexTangent.xyz)*vertexTangent.w;

  // Compute fragment normal based on normal transformations
  mat3 normalMatrix = transpose(inverse(mat3(matModel)));

  // used for lighting calculations.
  fragPosition = vec3(matModel*vec4(vertexPosition, 1.0));
  fragNormal = normalize(normalMatrix*vertexNormal);

  vec3 fragTangent = normalize(normalMatrix*vertexTangent.xyz);
  fragTangent = normalize(fragTangent - dot(fragTangent, fragNormal)*fragNormal);

  vec3 fragBinormal = normalize(normalMatrix*vertexBinormal);
  fragBinormal = cross(fragNormal, fragTangent);

  TBN = transpose(mat3(fragTangent, fragBinormal, fragNormal));

  fragTexCoord = vertexTexCoord;
  fragColor = vertexColor;

  gl_Position = mvp*vec4(vertexPosition, 1.0);
}   
