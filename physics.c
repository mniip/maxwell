#include <stddef.h>

#include "physics.h"

#define Q2 = 1.4142135623730950488

void update_aether(aether *dest, aether const *src, size_t width, size_t height, size_t depth, double dx, double dt)
{
#define index(a, x, y, z) ((a)[((x) * height + (y)) * depth + (z)])
#define tindex(a, x, y, z) index(a, (x + width) % width, (y + height) % height, (z + depth) % depth)
	for(size_t x = 0; x < width; x++)
		for(size_t y = 0; y < height; y++)
			for(size_t z = 0; z < depth; z++)
			{
				double curlEX = (
					+ tindex(src, x, y + 1, z).E[2]
					- tindex(src, x, y - 1, z).E[2]
					+ tindex(src, x, y, z - 1).E[1]
					- tindex(src, x, y, z + 1).E[1]
				) / 2.0 / dx;
				double curlEY = (
					+ tindex(src, x, y, z + 1).E[0]
					- tindex(src, x, y, z - 1).E[0]
					+ tindex(src, x - 1, y, z).E[2]
					- tindex(src, x + 1, y, z).E[2]
				) / 2.0 / dx;
				double curlEZ = (
					+ tindex(src, x + 1, y, z).E[1]
					- tindex(src, x - 1, y, z).E[1]
					+ tindex(src, x, y - 1, z).E[0]
					- tindex(src, x, y + 1, z).E[0]
				) / 2.0 / dx;
				double curlBX = (
					+ tindex(src, x, y + 1, z).B[2]
					- tindex(src, x, y - 1, z).B[2]
					+ tindex(src, x, y, z - 1).B[1]
					- tindex(src, x, y, z + 1).B[1]
				) / 2.0 / dx;
				double curlBY = (
					+ tindex(src, x, y, z + 1).B[0]
					- tindex(src, x, y, z - 1).B[0]
					+ tindex(src, x - 1, y, z).B[2]
					- tindex(src, x + 1, y, z).B[2]
				) / 2.0 / dx;
				double curlBZ = (
					+ tindex(src, x + 1, y, z).B[1]
					- tindex(src, x - 1, y, z).B[1]
					+ tindex(src, x, y - 1, z).B[0]
					- tindex(src, x, y + 1, z).B[0]
				) / 2.0 / dx;
				index(dest, x, y, z).B[0] = index(src, x, y, z).B[0] - dt * curlEX;
				index(dest, x, y, z).B[1] = index(src, x, y, z).B[1] - dt * curlEY;
				index(dest, x, y, z).B[2] = index(src, x, y, z).B[2] - dt * curlEZ;
				
				index(dest, x, y, z).E[0] = index(src, x, y, z).E[0] + dt * (curlBX / MU_0) / EPS_0;
				index(dest, x, y, z).E[1] = index(src, x, y, z).E[1] + dt * (curlBY / MU_0) / EPS_0;
				index(dest, x, y, z).E[2] = index(src, x, y, z).E[2] + dt * (curlBZ / MU_0) / EPS_0;
			}
}
