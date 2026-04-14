#pragma once

#include <type_traits>
#include "Game/settings.hpp"

// Handles mouse input callbacks and updates camera values in settings.
class InputManager
{
public:
    // Constructors
    explicit InputManager(settings& settingsRef);

    // Callback: updates camera orbit from cursor movement.
    void onCursorPosition(double xpos, double ypos);
    // Callback: updates camera zoom from scroll wheel.
    void onScroll(double xoffset, double yoffset);

private:
    // Configuration constants
    static constexpr float MOUSE_SENSITIVITY = 0.2f;
    static constexpr float ZOOM_SPEED        = 0.5f;
    static constexpr float MIN_PITCH         = -89.0f;
    static constexpr float MAX_PITCH         = 89.0f;
    static constexpr float MIN_DISTANCE      = 6.0f;
    static constexpr float MAX_DISTANCE      = 25.0f;

    // Static assertions to ensure configuration sanity
    static_assert(MIN_PITCH < MAX_PITCH, "InputManager: min pitch must be < max pitch");
    static_assert(MIN_DISTANCE < MAX_DISTANCE, "InputManager: min distance must be < max distance");
    static_assert(MOUSE_SENSITIVITY > 0.0f, "InputManager: mouse sensitivity must be > 0");
    static_assert(ZOOM_SPEED > 0.0f, "InputManager: zoom speed must be > 0");
    static_assert(std::is_floating_point_v<decltype(settings::cameraYawDegrees)>, "InputManager: camera yaw must be a float");
    static_assert(std::is_floating_point_v<decltype(settings::cameraPitchDegrees)>, "InputManager: camera pitch must be a float");
    static_assert(std::is_floating_point_v<decltype(settings::cameraDistance)>, "InputManager: camera distance must be a float");

    // Runtime input state.
    settings& _settings;
    bool      _firstMouseMove = true;
    double    _lastMouseX     = 0.0;
    double    _lastMouseY     = 0.0;

    // Private settings used by callback computations.
    float _mouseSensitivity = MOUSE_SENSITIVITY;
    float _zoomSpeed        = ZOOM_SPEED;
    float _minPitch         = MIN_PITCH;
    float _maxPitch         = MAX_PITCH;
    float _minDistance      = MIN_DISTANCE;
    float _maxDistance      = MAX_DISTANCE;

    // Helper function to apply zoom based on scroll input
    void applyZoom(float yoffset);
};
