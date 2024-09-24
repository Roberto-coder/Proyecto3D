#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "display.h"
#include "vector.h"
#include "matrix.h"

#define N_POINTS (9 * 9 * 9)
vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];

vec3_t cube_rotation = {0, 0, 0};
vec3_t cube_translation = {0, 0, 0};
vec3_t cube_scale = {1, 1, 1};

float fov_factor = 720;

bool is_running = false;
int previous_frame_time = 0;

void setup(void){
    // Allocate the required memory in bytes to hold the color buffer
    color_buffer = (uint32_t*) malloc(sizeof (uint32_t) * window_width * window_height);

    if(!color_buffer){
        fprintf(stderr, "Error allocating memory for frame buffer.\n");
    }

    //Creating SDL texture that isused to display the
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
        );

    int point_count = 0;

    // Start loading my array of vectors
    // From -1 to 1 (in this 9*9*9 cube)
    for (float x=-1; x<=1; x+=0.25){
        for (float y=-1; y<=1; y+=0.25){
            for (float z=-1; z<=1; z+=0.25){
                vec3_t new_point = {.x = x, .y = y, .z = z};
                // vec3_t new_point = {x, y, z};
                cube_points[point_count++] = new_point;
            }
        }
    }

}

void process_input(void){
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                is_running = false;
            break;
    }
}

/*
 * Function that receives a 3D point and returns a projected 2D point
 */
vec2_t project(vec3_t point) {
    vec2_t projected_point;
    float fov = 720.0; // Este puede ser un valor constante o variable

    // Proyección ortogonal simple: x' = (x * fov) / z, y' = (y * fov) / z
    if (point.z != 0) {
        projected_point.x = (point.x * fov) / point.z;
        projected_point.y = (point.y * fov) / point.z;
    } else {
        // Si z es cero, proyecta directamente al centro o ajusta el punto
        projected_point.x = point.x * fov;
        projected_point.y = point.y * fov;
    }

    return projected_point;
}
void render(void){
    draw_grid();

    // Loop all the projected points and render them
    for(int i=0; i<N_POINTS; i++){
        vec2_t projected_point = projected_points[i];
        draw_rect(
                projected_point.x + window_width/2,
                projected_point.y + window_height/2,
                4,
                4,
                0xFFFFFF00
                );
    }
    render_color_buffer();
    clear_color_buffer(0xFF000000);

    SDL_RenderPresent(renderer);
}

void update(void){
    cube_rotation.x += 0.01;
    cube_rotation.y += 0.01;
    cube_rotation.z += 0.01;

    /*cube_scale.x += 0.002;
    cube_scale.y+= 0.001;*/

    /*cube_translation.x += 0.01;*/
    cube_translation.z += 0.01;

    // Create scale, rotation, and translation matrices that will be used to multiply the mesh vertices
    mat4_t scale_matrix = mat4_make_scale(cube_scale.x,
                                          cube_scale.y,
                                          cube_scale.z);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(cube_rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(cube_rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(cube_rotation.z);
    mat4_t translation_matrix = mat4_make_translation(
            cube_translation.x, cube_translation.y,
            cube_translation.z);

    // Create a World Matrix combining scale, rotation, and translation matrices
    mat4_t world_matrix = mat4_identity();

    // Order matters: First scale, then rotate, then translate.
    // [T]*[R]*[S]*v
    world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
    world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

    vec4_t transformed_points[N_POINTS];

    for(int i = 0; i < N_POINTS; i++){

        vec4_t transformed_point = vec4_from_vec3(cube_points[i]);

        // Multiply the world matrix by the original vector
        transformed_point = mat4_mul_vec4(world_matrix,
                                          transformed_point);

        // Save transformed vertex in the array of transformed vertices
        transformed_points[i] = transformed_point;

        // Project the current point
        vec2_t projected_point = project(
                vec3_from_vec4(transformed_points[i]));

        // Save the projected 2D vectors
        projected_points[i] = projected_point;
    }
}

int main(int argc, char* argv[]) {
    is_running = initialize_window();

    setup();

    while (is_running){
        // Guarantees that every frame is executed every FRAME_TARGET_TIME
        // Wait some time until the reach the target frame time in milliseconds
        int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
        // Only delay execution if we are running too fast
        if(time_to_wait >0 && time_to_wait <= FRAME_TARGET_TIME){
            SDL_Delay(time_to_wait);
        }
        previous_frame_time = SDL_GetTicks();

        process_input();
        update();
        render();

    }

    destroy_window();
    return 0;
}
