#shader vertex
#version 460 core

layout(location = 0) in vec4 position;

uniform float theta = 0;

void main()
{
    mat3 rotationMatrix1;
    mat3 rotationMatrix2;

    rotationMatrix1[0] = vec3(cos(theta), 0, -sin(theta));
    rotationMatrix1[1] = vec3(0, 1, 0);
    rotationMatrix1[2] = vec3(sin(theta), 0, cos(theta));

    rotationMatrix2[0] = vec3(1, 0, 0);
    rotationMatrix2[1] = vec3(0, cos(theta), sin(theta));
    rotationMatrix2[2] = vec3(0, -sin(theta), cos(theta));

    gl_Position = vec4((position.xyz * rotationMatrix1) * rotationMatrix2, 1);
};

#shader fragment
#version 460 core

layout(location = 0) out vec4 color;
layout(pixel_center_integer) in vec4 gl_FragCoord;

uniform vec4 colorVector = {0.0, 0.7, 1.0, 1.0};

void main()
{
    color = colorVector;
};
