//
// Created by josue on 2024-07-17.
//

#include <time.h>

#include "particles_scene.h"
#include "physics/particles.h"
#define RANDOM_IMPLEMENTATION
#include "utils/random.h"
#include "graphics/raylib_utils.h"
#define STB_DS_IMPLEMENTATION
#include "external/stb_ds.h"

#include "cglm/cglm.h"

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "external/raygui.h"
#define GUI_PROPERTY_LIST_IMPLEMENTATION
#include "external/dm_property_list.h"

#define PARTICLE_COUNT 100
#define PIXEL_PER_UNIT 25

static Particle *particles = NULL;

// Mode selection
enum {
    MODE_ADD_PARTICLE = 0,
    MODE_DRAW_CHAIN,
} active_mode = MODE_ADD_PARTICLE;
bool mode_is_edited = false;

// Force list
GuiDMProperty prop[] = {
        PBOOL("Bool", 0, true),
        PSECTION("#102#SECTION", 0, 2),
        PINT("Int", 0, 123),
        PFLOAT("Float", 0, 0.99f),
        PTEXT("Text", 0, (char*)&(char[30]){"Hello!"}, 30),
        PSELECT("Select", 0, "ONE;TWO;THREE;FOUR", 0),
        PINT_RANGE("Int Range", 0, 32, 1, 0, 100),
        PRECT("Rect", 0, 0, 0, 100, 200),
        PVEC2("Vec2", 0, 20, 20),
        PVEC3("Vec3", 0, 12, 13, 14),
        PVEC4("Vec4", 0, 12, 13, 14, 15),
        PCOLOR("Color", 0, 0, 255, 0, 255),
};
int focus = 0, scroll = 0; // Needed by GuiDMPropertyList()

// GUI states
static float mouse_travel;
static bool is_gravity_enabled = true;
static bool is_drag_enabled = true;

// Forces
typedef struct Force {
    enum ForceType {
        FORCE_TYPE_WIND,
        FORCE_TYPE_FRICTION,
    } type;
    vec2 values;
} Force;
static Force *force_list = NULL;

void add_push(vec2 wind, float inverse_mass, vec2 out_forces) {
    glm_vec2_add(out_forces, (vec2){wind[0] * inverse_mass, wind[1] * inverse_mass}, out_forces);
}

void add_friction(){

}

void apply_drag(const vec2 velocity, float coefficient, vec2 forces) {
    vec2 drag = {0};
    vec2 direction;
    float velocity_magnitude_squared = glm_vec2_norm2(velocity);
    if(velocity_magnitude_squared > 0.0f) {
        glm_vec2_normalize_to(velocity, direction);
        glm_vec2_negate(direction);
        float magnitude = coefficient * velocity_magnitude_squared;
        glm_vec2_scale(direction, magnitude, drag);
    }
    glm_vec2_add(forces, drag, forces);
}

void particles_scene_init(void) {
    random_seed(time(NULL) % 41328749032178);

    for (int j = 0; j < PARTICLE_COUNT; ++j) {
        float mass = random_float_range(1.0f, 4.0f);
        arrput(particles, particle(
            (vec2) {random_u32() % 974 + 50, random_u32() % 670 + 50},
            mass * 4.0f,
            mass,
            false
        ));
    }

    Force force = {
        .type = FORCE_TYPE_WIND,
        .values = {10.0f, 0.0f},
    };
    arrput(force_list, force);
    force.type = FORCE_TYPE_FRICTION;
    arrput(force_list, force);
}

void process_inputs(void) {
    if (IsMouseButtonPressed(0) && !mode_is_edited) {
        switch (active_mode) {
            case MODE_ADD_PARTICLE:
                float mass = random_float_range(1.0f, 4.0f);
                arrput(particles, particle(
                        (vec2){GetMouseX(), GetMouseY()},
                    mass * 4.0f,
                    mass,
                    false
                ));
                break;
            case MODE_DRAW_CHAIN:
                mouse_travel = 0.0f;
                arrput(particles, particle(
                    (vec2){GetMouseX(), GetMouseY()},
                    4.0f,
                    1.0f,
                    true
                ));
                break;
        }
    } else if(IsMouseButtonDown(0) && active_mode == MODE_DRAW_CHAIN){
        mouse_travel += glm_vec2_norm((vec2){GetMouseDelta().x, GetMouseDelta().y});
        if (mouse_travel > 100.0f) {
            mouse_travel -= 100.0f;
            arrput(particles, particle(
                (vec2){GetMouseX(), GetMouseY()},
                4.0f,
                1.0f,
                false
            ));
        }
    }
}

void particles_scene_update(float delta_time) {
    process_inputs();

    for (int j = 0; j < arrlen(particles); ++j) {
        if (particles[j].anchor) { continue; }

        vec2 force_sum = {0};

        if (is_gravity_enabled) { force_sum[1] = 10.0f; }

        for (int i = 0; i < arrlen(force_list); ++i) {
            switch (force_list[i].type) {
                case FORCE_TYPE_WIND:
                    add_push(force_list[i].values, particles[j].inverse_mass, force_sum);
                    break;
                case FORCE_TYPE_FRICTION:
                    add_friction();
                    break;
            }
        }

        if (is_drag_enabled){apply_drag(particles[j].velocity, 0.003f, force_sum);}

        // integrate forces
        vec2 acceleration = {0};
        glm_vec2_add(acceleration, force_sum, acceleration);
        glm_vec2_scale(acceleration, delta_time, acceleration);
        glm_vec2_add(particles[j].velocity, acceleration, particles[j].velocity);

        // integrate velocity
        vec2 delta_velocity;
        glm_vec2_scale(particles[j].velocity, delta_time * PIXEL_PER_UNIT, delta_velocity);
        glm_vec2_add(particles[j].position, delta_velocity, particles[j].position);

        // check screen bounds
        if(particles[j].position[0] < particles[j].radius) {
            particles[j].position[0] = particles[j].radius;
            particles[j].velocity[0] = -particles[j].velocity[0];
        } else if (particles[j].position[0] > 1024 - particles[j].radius){
            particles[j].position[0] = 1024 - particles[j].radius;
            particles[j].velocity[0] = -particles[j].velocity[0];
        }
        if(particles[j].position[1] < particles[j].radius) {
            particles[j].position[1] = particles[j].radius;
            particles[j].velocity[1] = -particles[j].velocity[1];
        } else if (particles[j].position[1] > 720 - particles[j].radius){
            particles[j].position[1] = 720 - particles[j].radius;
            particles[j].velocity[1] = -particles[j].velocity[1];
        }
    }
}

void particles_scene_render() {
    for (int j = 0; j < arrlen(particles); ++j) {
        drawParticle(particles[j].position, particles[j].radius, (ivec4){255,255,255,255});
    }

    // draw ui
    if (GuiButton((Rectangle){25, 100, 125, 30 }, "#63#Clear")) {
        arrsetlen(particles, 0);
    }
    GuiCheckBox((Rectangle){30, 140, 25, 25}, "Gravity", &is_gravity_enabled);
    GuiCheckBox((Rectangle){30, 170, 25, 25}, "Drag", &is_drag_enabled);

    // List of properties
    GuiDMPropertyList((Rectangle){25, (GetScreenHeight() - 280)/2, 180, 280}, prop, sizeof(prop)/sizeof(prop[0]), &focus, &scroll);

    if (prop[0].value.vbool >= 1)
    {
        DrawText(TextFormat("FOCUS:%i | SCROLL:%i | FPS:%i", focus, scroll, GetFPS()), prop[8].value.v2.x, prop[8].value.v2.y, 20, prop[11].value.vcolor);
    }

    if (GuiDropdownBox((Rectangle){ 25, 65, 125, 30 }, "#145#Particle;#22#Draw Chain", &active_mode, mode_is_edited)) {
        mode_is_edited = !mode_is_edited;
    }

    DrawText(TextFormat("%10d PARTICLES", arrlen(particles)), GetScreenWidth() - 250, 25, 20, DARKGREEN);
}

void particles_scene_cleanup(void) {
    arrfree(particles);
    arrfree(force_list);
}

void particles_scene_load(Scene *scene) {
    scene->init = particles_scene_init;
    scene->update = particles_scene_update;
    scene->render = particles_scene_render;
    scene->cleanup = particles_scene_cleanup;
}