__kernel void pie( __global float *Z,float X)
{
int i=get_global_id(0);
float j = i*X;
Z[i]=X * 4 / (1 + (j*j));
}