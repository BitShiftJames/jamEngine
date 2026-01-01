#version 330 core

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragPosition;
out vec3 fragNormal;

uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matNormal;

void main() {
  fragTexCoord = vertexTexCoord;
  fragColor = vertexColor;
  // used for lighting calculations.
  fragPosition = vec3(matModel*vec4(vertexPosition, 1.0));
  fragNormal = normalize(vec3(matNormal*vec4(vertexNormal, 1.0)));

  gl_Position = mvp*vec4(vertexPosition, 1.0);
}   
