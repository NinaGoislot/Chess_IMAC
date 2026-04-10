#pragma once

#include <type_traits>
#include "Game/settings.hpp"

class InputManager
{
public:
    explicit InputManager(settings& settingsRef);

    void update();
    void onMouseButton(int button, int action, int mods);
    void onCursorPosition(double xpos, double ypos);
    void onScroll(double xoffset, double yoffset);

private:
    static constexpr float kMouseSensitivity = 0.2f;
    static constexpr float kZoomSpeed        = 0.5f;
    static constexpr float kMinPitch         = -89.0f;
    static constexpr float kMaxPitch         = 89.0f;
    static constexpr float kMinDistance      = 6.0f;
    static constexpr float kMaxDistance      = 25.0f;

    static_assert(kMinPitch < kMaxPitch, "InputManager: min pitch must be < max pitch");
    static_assert(kMinDistance < kMaxDistance, "InputManager: min distance must be < max distance");
    static_assert(kMouseSensitivity > 0.0f, "InputManager: mouse sensitivity must be > 0");
    static_assert(kZoomSpeed > 0.0f, "InputManager: zoom speed must be > 0");
    static_assert(std::is_floating_point_v<decltype(settings::cameraYawDegrees)>, "InputManager: camera yaw must be a float");
    static_assert(std::is_floating_point_v<decltype(settings::cameraPitchDegrees)>, "InputManager: camera pitch must be a float");
    static_assert(std::is_floating_point_v<decltype(settings::cameraDistance)>, "InputManager: camera distance must be a float");

    settings& _settings;
    bool      _rightMouseDown = false;
    bool      _firstMouseMove = true;
    double    _lastMouseX     = 0.0;
    double    _lastMouseY     = 0.0;

    float _mouseSensitivity = kMouseSensitivity;
    float _zoomSpeed        = kZoomSpeed;
    float _minPitch         = kMinPitch;
    float _maxPitch         = kMaxPitch;
    float _minDistance      = kMinDistance;
    float _maxDistance      = kMaxDistance;

    void applyZoom(float yoffset);
};
