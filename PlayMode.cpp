#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

// Assume the wall length is 4

GLuint world_meshes_for_lit_color_texture_program = 0;
Scene::Drawable::Pipeline wall_pipeline;
glm::vec3 wall_scale;
Load<MeshBuffer> world_meshes(LoadTagDefault, []() -> MeshBuffer const * {
    MeshBuffer const *ret = new MeshBuffer(data_path("world.pnct"));
    world_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
    return ret;
});

Load<Scene> world_scene(LoadTagDefault, []() -> Scene const * {
    return new Scene(
            data_path("world.scene"),
            [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name) {
                Mesh const &mesh = world_meshes->lookup(mesh_name);
                
                Scene::Drawable::Pipeline pipeline = lit_color_texture_program_pipeline;
                pipeline.vao = world_meshes_for_lit_color_texture_program;
                pipeline.type = mesh.type;
                pipeline.start = mesh.start;
                pipeline.count = mesh.count;
                
                if (transform->name == "Wall") {
                    wall_pipeline = pipeline;
                    wall_scale = transform->scale;
                } else {
                    scene.drawables.emplace_back(transform);
                    scene.drawables.back().pipeline = pipeline;
                }
            });
});

PlayMode::PlayMode() :
        scene(*world_scene),
        // TODO: do something about this
        level(
                {
                        {Cell::Corner, Cell::Horiz,  Cell::Horiz,  Cell::Horiz},
                        {Cell::Vert,   Cell::Corner, Cell::Horiz,  Cell::None},
                        {Cell::Vert,   Cell::Vert,   Cell::Corner, Cell::Horiz},
                        {Cell::Vert,   Cell::Vert,   Cell::None,   Cell::Vert},
                },
                0, 0, 0, 3
        
        ) {
    for (auto &transform: scene.transforms) {
        if (transform.name == "Player") player = &transform;
    }
    if (player == nullptr) throw std::runtime_error("Player not found.");
    player->position = glm::vec3(level.start_row * 4 + 2, level.start_col * 4 + 2, 0);
    
    for (size_t row = 0; row < level.height; row++) {
        for (size_t col = 0; col < level.width; col++) {
            if (level.has_border(row, col, Direction::Down)) {
                scene.transforms.emplace_back();
                scene.transforms.back().position = glm::vec3(4 * row, 4 * col + 2, 0);
                scene.transforms.back().rotation = glm::angleAxis(glm::pi<float>() / 2.0f, glm::vec3(0, 0, 1));
                scene.transforms.back().scale = wall_scale;
                scene.drawables.emplace_back(&scene.transforms.back());
                scene.drawables.back().pipeline = wall_pipeline;
            }
            if (level.has_border(row, col, Direction::Left)) {
                scene.transforms.emplace_back();
                scene.transforms.back().position = glm::vec3(4 * row + 2, 4 * col, 0);
                scene.transforms.back().rotation = glm::angleAxis(0.0f, glm::vec3(0, 0, 1));
                scene.transforms.back().scale = wall_scale;
                scene.drawables.emplace_back(&scene.transforms.back());
                scene.drawables.back().pipeline = wall_pipeline;
            }
        }
    }
    for (size_t col = 0; col < level.width; col++) {
        if (level.has_border(level.height - 1, col, Direction::Up)) {
            scene.transforms.emplace_back();
            scene.transforms.back().position = glm::vec3(4 * level.height, 4 * col + 2, 0);
            scene.transforms.back().rotation = glm::angleAxis(glm::pi<float>() / 2.0f, glm::vec3(0, 0, 1));
            scene.transforms.back().scale = wall_scale;
            scene.drawables.emplace_back(&scene.transforms.back());
            scene.drawables.back().pipeline = wall_pipeline;
        }
    }
    for (size_t row = 0; row < level.height; row++) {
        if (level.has_border(row, level.width - 1, Direction::Right)) {
            scene.transforms.emplace_back();
            scene.transforms.back().position = glm::vec3(4 * row + 2, 4 * level.width, 0);
            scene.transforms.back().rotation = glm::angleAxis(0.0f, glm::vec3(0, 0, 1));
            scene.transforms.back().scale = wall_scale;
            scene.drawables.emplace_back(&scene.transforms.back());
            scene.drawables.back().pipeline = wall_pipeline;
        }
    }
    
    // get pointer to camera for convenience:
    // TODO: make this nicer and not look at scene cameras
    if (scene.cameras.size() != 1)
        throw std::runtime_error(
                "Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
    camera = &scene.cameras.front();
    
    // start location sound
    triangle.set_frequency(440);
    
    // start background
    // TODO
}

PlayMode::~PlayMode() = default;

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
    
    if (evt.type == SDL_KEYDOWN) {
        if (evt.key.keysym.sym == SDLK_ESCAPE) {
            SDL_SetRelativeMouseMode(SDL_FALSE);
            return true;
        } else if (evt.key.keysym.sym == SDLK_a) {
            left.downs += 1;
            left.pressed = true;
            return true;
        } else if (evt.key.keysym.sym == SDLK_d) {
            right.downs += 1;
            right.pressed = true;
            return true;
        } else if (evt.key.keysym.sym == SDLK_w) {
            up.downs += 1;
            up.pressed = true;
            return true;
        } else if (evt.key.keysym.sym == SDLK_s) {
            down.downs += 1;
            down.pressed = true;
            return true;
        }
    } else if (evt.type == SDL_KEYUP) {
        if (evt.key.keysym.sym == SDLK_a) {
            left.pressed = false;
            return true;
        } else if (evt.key.keysym.sym == SDLK_d) {
            right.pressed = false;
            return true;
        } else if (evt.key.keysym.sym == SDLK_w) {
            up.pressed = false;
            return true;
        } else if (evt.key.keysym.sym == SDLK_s) {
            down.pressed = false;
            return true;
        }
    } else if (evt.type == SDL_MOUSEBUTTONDOWN) {
        if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
            SDL_SetRelativeMouseMode(SDL_TRUE);
            return true;
        }
    }
    
    return false;
}

void PlayMode::update(float elapsed) {
    // move player:
    {
        // combine inputs into a move:
        constexpr float PlayerSpeed = 10.0f;
        auto move = glm::vec3(0.0f, 0.0f, 0.0f);
        if (left.pressed && !right.pressed) move.x = -1.0f;
        if (!left.pressed && right.pressed) move.x = 1.0f;
        if (down.pressed && !up.pressed) move.y = -1.0f;
        if (!down.pressed && up.pressed) move.y = 1.0f;
        
        //make it so that moving diagonally doesn't go faster:
        if (move != glm::vec3(0.0f, 0.0f, 0.0f)) move = glm::normalize(move) * PlayerSpeed * elapsed;
        
        player->position += move;
        // camera follows the player
        camera->transform->position += move;
    }
    
    // reset button press counters:
    left.downs = 0;
    right.downs = 0;
    up.downs = 0;
    down.downs = 0;
    
    // tick
    since_tick += elapsed;
    while (since_tick > TICK) {
        since_tick -= TICK;
        tick();
    }
}

void PlayMode::tick() {
    if (tick_count % 2 == 1) {
        triangle.set_frequency(440 * powf(2, player->position.x / 1000));
        triangle.play();
    }
    
    tick_count++;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
    // update camera aspect ratio for drawable:
    camera->aspect = float(drawable_size.x) / float(drawable_size.y);
    
    //set up light type and position for lit_color_texture_program:
    // TODO: consider using the Light(s) in the scene to do this
    glUseProgram(lit_color_texture_program->program);
    glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
    glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, -1.0f)));
    glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
    glUseProgram(0);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.
    
    scene.draw(*camera);
    
    { //use DrawLines to overlay some text:
        glDisable(GL_DEPTH_TEST);
        float aspect = float(drawable_size.x) / float(drawable_size.y);
        DrawLines lines(glm::mat4(
                1.0f / aspect, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        ));
        
        constexpr float H = 0.09f;
        lines.draw_text("Mouse motion rotates camera; WASD moves; escape ungrabs mouse",
                        glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
                        glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
                        glm::u8vec4(0x00, 0x00, 0x00, 0x00));
        float ofs = 2.0f / drawable_size.y;
        lines.draw_text("Mouse motion rotates camera; WASD moves; escape ungrabs mouse",
                        glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + +0.1f * H + ofs, 0.0),
                        glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
                        glm::u8vec4(0xff, 0xff, 0xff, 0x00));
    }
    GL_ERRORS();
}
