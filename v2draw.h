/*
 * v2draw.h
 *
 * Debug drawing functions for v2 types. Depends on v2.h and SDL2, requires C11 or higher.
 *
 * In one source file, define V2DRAW_IMPL before including this header.
 *
 */

/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org/>
 */
#ifndef V2DRAW_H
#define V2DRAW_H

#include "SDL2/SDL.h"
#include "v2.h"

#ifndef V2DRAW_SCALE
#define V2DRAW_SCALE 100
#endif
#ifndef V2DRAW_POINT_SIZE
#define V2DRAW_POINT_SIZE 8
#endif

void v2draw_circ(SDL_Renderer *ren, struct v2circ circ);
void v2draw_poly(SDL_Renderer *ren, struct v2poly *poly);
void v2draw_ray(SDL_Renderer *ren, struct v2ray ray);
void v2draw_vec(SDL_Renderer *ren, v2v start, v2v v);
void v2draw_point(SDL_Renderer *ren, v2v v);

#endif

#ifdef V2DRAW_IMPL
#undef V2DRAW_IMPL

#include <complex.h>

v2v v2draw_translation(SDL_Renderer *ren) {
	int w, h;
	SDL_GetRendererOutputSize(ren, &w, &h);
	return v2v(w, h) * 0.5;
}

void v2draw_circ(SDL_Renderer *ren, struct v2circ circ) {
	int rad = circ.radius * V2DRAW_SCALE;
	v2v center = v2conj(circ.center) * V2DRAW_SCALE + v2draw_translation(ren);

	// Midpoint circle algorithm stolen from https://en.wikipedia.org/wiki/Midpoint_circle_algorithm#C_example
	int x0 = v2x(center);
	int y0 = v2y(center);
	int x = rad - 1;
	int y = 0;
	int dx = 1;
	int dy = 1;
	int diam = rad*2;
	int err = dx - diam;

	while (x >= y) {
		SDL_RenderDrawPoint(ren, x0 + x, y0 + y);
		SDL_RenderDrawPoint(ren, x0 + y, y0 + x);
		SDL_RenderDrawPoint(ren, x0 - y, y0 + x);
		SDL_RenderDrawPoint(ren, x0 - x, y0 + y);
		SDL_RenderDrawPoint(ren, x0 - x, y0 - y);
		SDL_RenderDrawPoint(ren, x0 - y, y0 - x);
		SDL_RenderDrawPoint(ren, x0 + y, y0 - x);
		SDL_RenderDrawPoint(ren, x0 + x, y0 - y);

		if (err <= 0) {
			y++;
			err += dy;
			dy += 2;
		} else {
			x--;
			dx += 2;
			err += dx - diam;
		}
	}
}

void v2draw_poly(SDL_Renderer *ren, struct v2poly *poly) {
	unsigned count = poly->sides+1;
	SDL_Point points[count];

	v2v translate = v2draw_translation(ren);
	for (unsigned i = 0; i < poly->sides; i++) {
		v2v point = v2conj(poly->points[i]) * V2DRAW_SCALE + translate;
		points[i] = (SDL_Point){v2x(point), v2y(point)};
	}
	points[count-1] = points[0];

	SDL_RenderDrawLines(ren, points, count);
}

void v2draw_ray(SDL_Renderer *ren, struct v2ray ray) {
	int w, h;
	SDL_GetRendererOutputSize(ren, &w, &h);
	v2v translate = v2v(w, h) * 0.5;

	v2v a = v2conj(ray.start) * V2DRAW_SCALE + translate;
	v2v b = v2conj(ray.start + ray.direction * (w+h)) * V2DRAW_SCALE + translate;

	SDL_RenderDrawLine(ren, v2x(a), v2y(a), v2x(b), v2y(b));
}

void v2draw_vec(SDL_Renderer *ren, v2v start, v2v v) {
	v2v translate = v2draw_translation(ren);

	start = v2conj(start) * V2DRAW_SCALE + translate;
	v = v2conj(v) * V2DRAW_SCALE;
	v2v end = start + v;

	// Line
	SDL_RenderDrawLine(ren, v2x(start), v2y(start), v2x(end), v2y(end));
	// Arrow
	v2v prong = v2v(0.15, 0.7);
	v2v prong1 = start + prong*v*-I;
	v2v prong2 = start + -v2conj(prong)*v*-I;
	SDL_Point arrow[3] = {
		{v2x(prong1), v2y(prong1)},
		{v2x(end), v2y(end)},
		{v2x(prong2), v2y(prong2)},
	};
	SDL_RenderDrawLines(ren, arrow, 3);
}

void v2draw_point(SDL_Renderer *ren, v2v v) {
	v = v2conj(v) * V2DRAW_SCALE + v2draw_translation(ren);
	SDL_Rect r = {v2x(v)-V2DRAW_POINT_SIZE/2, v2y(v)-V2DRAW_POINT_SIZE/2, V2DRAW_POINT_SIZE, V2DRAW_POINT_SIZE};
	SDL_RenderFillRect(ren, &r);
}

#endif
