#shader vertex
#version 330 core

layout(location = 0) in vec4 position;

void main()
{
   gl_Position = position;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec4 gl_FragCoord;

void main()
{
   color = vec4(gl_FragCoord.xy, 500, 500) / 500;
};
