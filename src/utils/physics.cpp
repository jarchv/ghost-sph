void collisions(float* spherePos, float&tSim, float& v0, int num_particles, float timeStep)
{   
    float gravity = 9.80665;

    for (int ie = 0; ie < num_particles; ie++)
    {
        //spherePos[ie * 3 + 0] += v0 * timeStep - 0.5 * gravity * timeStep * timeStep ;
        spherePos[ie * 3 + 1] += v0 * timeStep - 0.5 * gravity * timeStep * timeStep ;
        //spherePos[ie * 3 + 2] += v0 * timeStep - 0.5 * gravity * timeStep * timeStep ;
    }   

    v0   -= timeStep * gravity;
    tSim += timeStep;    
}