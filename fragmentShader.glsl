#version 320 es

precision mediump float;

// Interpolated values from the vertex shaders
in vec3 fragmentColor;
//in vec2 UV;

// Out data
out vec3 color;

// Values that stay constant for whole mesh
uniform sampler2D TextureSampler;

void main()
{
    //color = vec3(0.7,0.0,0);
    color = fragmentColor;
    //color = texture(TextureSampler, UV).rgb;
}