#version 460 core

layout (location = 0) in vec2 aPos;

void main()
{
    // Pass the vertex position to the fragment shader without any transformation
    gl_Position = vec4(aPos, 0.0, 1.0);
}
