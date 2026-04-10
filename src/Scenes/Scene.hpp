#pragma once

class Scene {
public:
    Scene()                        = default;
    Scene(const Scene&)            = default;
    Scene(Scene&&)                 = default;
    Scene& operator=(const Scene&) = default;
    Scene& operator=(Scene&&)      = default;
    virtual ~Scene()               = default;

    virtual void render() = 0;
};