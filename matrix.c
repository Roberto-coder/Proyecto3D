#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include <math.h>
#include "matrix.h"
#include "display.h"
#include "vector.h"

#define N_POINTS (9*9*9)
vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];

vec3_t cube_rotation ={0, 0, 0};
vec3_t cube_traslation ={0, 0, 0};
vec3_t cube_scale ={1, 1, 1};

float fov_factor = 720;

bool is_running = false;
int previous_frame_time = 0;



// Crea una matriz identidad 4x4
mat4_t mat4_identity(void) {
    mat4_t result = {0};
    result.m[0][0] = 1;
    result.m[1][0] = 0;
    result.m[2][0] = 0;
    result.m[3][0] = 0;

    result.m[0][1] = 0;
    result.m[1][1] = 1;
    result.m[2][1] = 0;
    result.m[3][1] = 0;

    result.m[0][2] = 0;
    result.m[1][2] = 0;
    result.m[2][2] = 1;
    result.m[3][2] = 0;

    result.m[0][3] = 0;
    result.m[1][3] = 0;
    result.m[2][3] = 0;
    result.m[3][3] = 1;

    return result;
}

// Crea una matriz de escala 4x4
mat4_t mat4_make_scale(float sx, float sy, float sz) {
    mat4_t result = mat4_identity();
    result.m[0][0] = sx;
    result.m[1][1] = sy;
    result.m[2][2] = sz;
    return result;
}

// Crea una matriz de traslación 4x4
mat4_t mat4_make_translation(float tx, float ty, float tz) {
    mat4_t result = mat4_identity();
    result.m[0][3] = tx;
    result.m[1][3] = ty;
    result.m[2][3] = tz;
    return result;
}

// Crea una matriz de rotación en el eje X 4x4
mat4_t mat4_make_rotation_x(float angle) {
    mat4_t result = mat4_identity();
    float c = cos(angle);
    float s = sin(angle);

    result.m[1][1] = c;
    result.m[1][2] = -s;
    result.m[2][1] = s;
    result.m[2][2] = c;

    return result;
}

// Crea una matriz de rotación en el eje Y 4x4
mat4_t mat4_make_rotation_y(float angle) {
    mat4_t result = mat4_identity();
    float c = cos(angle);
    float s = sin(angle);

    result.m[0][0] = c;
    result.m[0][2] = s;
    result.m[2][0] = -s;
    result.m[2][2] = c;

    return result;
}

// Crea una matriz de rotación en el eje Z 4x4
mat4_t mat4_make_rotation_z(float angle) {
    mat4_t result = mat4_identity();
    float c = cos(angle);
    float s = sin(angle);

    result.m[0][0] = c;
    result.m[0][1] = -s;
    result.m[1][0] = s;
    result.m[1][1] = c;

    return result;
}

// Multiplica una matriz 4x4 por un vector 4D
vec4_t mat4_mul_vec4(mat4_t m, vec4_t v) {
    vec4_t result;

    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;

    return result;
}

// Multiplica dos matrices 4x4
mat4_t mat4_mul_mat4(mat4_t a, mat4_t b) {
    mat4_t result = {0};

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = a.m[i][0] * b.m[0][j] +
                             a.m[i][1] * b.m[1][j] +
                             a.m[i][2] * b.m[2][j] +
                             a.m[i][3] * b.m[3][j];
        }
    }

    return result;
}

