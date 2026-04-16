#pragma once

// Base interface for every renderable scene
class Scene {
public:
    // Constructors
    Scene()                        = default;
    Scene(const Scene&)            = default;
    Scene(Scene&&)                 = default;
    Scene& operator=(const Scene&) = default;
    Scene& operator=(Scene&&)      = default;
    virtual ~Scene()               = default;

    // Render function
    virtual void render() = 0;
};
