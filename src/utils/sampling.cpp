void setInitialPosition(float* spherePos, int num_particles,float kf)
{
    for (int ie = 0; ie < num_particles; ie++)
    {
        spherePos[ie * 3 + 0] = kf * ((ie%25)%5);
        spherePos[ie * 3 + 1] = kf * ((ie/25  ));
        spherePos[ie * 3 + 2] = kf * ((ie%25)/5);
    }   
}