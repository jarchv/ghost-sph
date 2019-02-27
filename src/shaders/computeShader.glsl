#version 430
#extension GL_ARB_compute_shader: enable
#extension GL_ARB_shader_storage_buffer_object: enable

struct Particle 
{
    vec3 position;
    vec3 velocity;
    vec3 force;
    float     density0;
    float     density;
    float     pressure;
};

layout(std140, binding = 4) buffer Particles 
{ 
    Particle System[];
};

layout (local_size_x=128, local_size_y = 1) in;

uniform int numParticles;
//layout (rgba32f, binding = 4) uniform image2D img_output;

layout (rgba32f) uniform image2D data;
void main()
{
    uint gid = gl_GlobalInvocationID.x;
    System[gid].position.xyz = System[gid].position.xyz + vec3(0.1,0.0,0.0);
    
    ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
    
    uint gWidth     = gl_WorkGroupSize.x * gl_NumWorkGroups.x;
    uint gHeight    = gl_WorkGroupSize.y * gl_NumWorkGroups.y;

    uint gSize      = gWidth * gHeight;
    uint offset     = storePos.y * gWidth + storePos.x;

    if (offset < numParticles)
    {
        System[offset].position.xyz += vec3(0.01,0.0,0.0);  
    }
    
    imageStore(data, storePos, vec4(1.0,System[offset].position.y*0.1,System[offset].position.z*0.1,1.0));
}