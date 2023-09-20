#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"
#include "TriangleWave.hpp"
#include "Level.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
    PlayMode();
    
    ~PlayMode() override;
    
    //functions called by main loop:
    bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
    
    void update(float elapsed) override;
    
    void draw(glm::uvec2 const &drawable_size) override;
    
    static constexpr float TICK = TriangleWave::LENGTH;
    float since_tick = 0;
    size_t tick_count = 0;
    
    void tick();
    
    //----- game state -----
    Scene::Transform *player = nullptr;
    // There is also a hidden wall mesh/pipeline in the .cpp file
    
    //input tracking:
    struct Button {
        uint8_t downs = 0;
        uint8_t pressed = 0;
    } left, right, down, up;
    
    //local copy of the game scene (so code can change it during gameplay):
    Scene scene;
    
    Level level;
    
    //camera:
    Scene::Camera *camera = nullptr;
    
    // TODO: add another triangle for the y axis
    TriangleWave triangle;
};
