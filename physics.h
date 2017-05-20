#ifndef PHYSICS_H
#define PHYSICS_H

typedef struct
{
	double E[3];
	double B[3];
}
aether;

#define MU_0 1.2566370614e-6f
#define EPS_0 8.854187817e-12f

extern void update_aether(aether *dest, aether const *src, size_t width, size_t height, size_t depth, double dx, double dt);

#endif
