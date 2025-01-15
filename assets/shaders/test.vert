
#version 460
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0

out vec2 pos;

float x() {
    return sin(10.0);
}

void main()
{
    gl_Position = vec4(aPos -1, 1.0); // see how we directly give a vec3 to vec4's constructor
    pos = vec2(aPos.x - 1, aPos.y - 1);
}