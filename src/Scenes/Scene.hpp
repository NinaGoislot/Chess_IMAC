#pragma once

// Base interface for every renderable UI/game scene.
class Scene {
public:
    // Constructors
    Scene()                        = default;
    Scene(const Scene&)            = default;
    Scene(Scene&&)                 = default;
    Scene& operator=(const Scene&) = default;
    Scene& operator=(Scene&&)      = default;
    virtual ~Scene()               = default;

    // Render function implemented by each concrete scene.
    virtual void render() = 0;
};
