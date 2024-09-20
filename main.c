#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include <math.h>
#include "display.h"
#include "vector.h"
#include "matrix.h"

#define N_POINTS (9*9*9)
vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];

vec3_t cube_rotation ={0, 0, 0};
vec3_t cube_traslation ={0, 0, 0};
vec3_t cube_scale ={1, 1, 1};

float fov_factor = 720;

bool is_running = false;
int previous_frame_time =0;

void setup(void) {
    // Allocate the required memory in bytes to
    //hold the color buffer
    color_buffer = (uint8_t*)malloc(sizeof(uint32_t)*window_width*window_height);

    if(!color_buffer) {
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


    for (float x =-1; x<=1; x+=0.25) {
        for (float y =-1; y<=1; y+=0.25) {
            for (float z =-1; z<=1; z+=0.25) {
                vec3_t new point = {.x = x, .y = y, .z = z};

                cube_points[point_count++] = new_point;
            }
        }
    }
}

void proccess_input(void) {
    SDL_Event event;
    //SDL_PollEvent(&event);

    while (SDL_PollEvent(&event)) {  // Bucle para procesar todos los eventos
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

}

void update(void) {
    //TODO: Coming soon... (rotacion)
    cube_rotation.x += 0.01;
    cube_rotation.y += 0.01;
    cube_rotation.z += 0.01;

    /*cube_scaling += 0.001
    cube_scale.y += 0.01;*/

    /*cube_traslation.x += 0.001*/
    cube_traslation.z += 0.01;

    mat4_t scale_matrix = mat4_make_scale(cube_scale.x, cube_scale.y, cube_scale.z);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(cube_rotation.x, cube_rotation.y, cube_rotation.z);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(cube_rotation.x, cube_rotation.y, cube_rotation.z);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(cube_rotation.x, cube_rotation.y, cube_rotation.z);
    mat4_t traslation_matrix = mat4_make_traslation(cube_traslation.x, cube_traslation.y, cube_traslation.z);

    mat4_t world_matrix = mat4_identity();

    world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
    world_matrix = mat4_mul_mat4(traslation_matrix, world_matrix);

    vec4_t transformed_points[N_POINTS];

    for (int i =0:i< N_POINTS; i++){
        vec4_t transformed_point= vec4_from_vec3(cube_points[i]);

        transformed_point = mat4_mul_mat4(world_matrix,transformed_point );

        transformed_points[i] = transformed_point;

        vec2_t projected_point = project(vec3_from_vec4(transformed_points[i]));

        projected_points[i] = projected_point;
    }

}

void render(void) {
    draw_grid();
    //draw_rect(300,200,300,150,0xFFFF00FF);

    for (int i=0; i<N_POINTS; i++) {
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

    //SDL_RenderPresent(render);
    SDL_RenderPresent(renderer);
}

/*int main(int argc, char* argv[]) {
    is_running = initialize_window();

    setup();

    while(is_running) {
        proccess_input();
        update();
        render();

        //Guarantees that every frame is executed every FRAME_TARGET_TIME
        //Wait some time until the reach the target frame time in milliseconds
        int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks()-previous_frame_time);

        //Only delay execution if we are running too fast
        if(time_to_wait>0 && time_to_wait <= FRAME_TARGET_TIME ) {
            SDL_Delay(time_to_wait);
        }

        destroy_window();
        return 0;
    }
}*/

int main(int argc, char* argv[]) {
    is_running = initialize_window();

    setup();

    while (is_running) {
        proccess_input();
        update();
        render();

        //Guarantees that every frame is executed every FRAME_TARGET_TIME
        //Wait some time until the reach the target frame time in milliseconds
        int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

        //Only delay execution if we are running too fast
        if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
            SDL_Delay(time_to_wait);
        }

        previous_frame_time = SDL_GetTicks();  // Actualiza el tiempo del frame anterior
    }

    destroy_window();  // Esto debe estar fuera del bucle principal
    return 0;
}

