#include "Managers/InputManager.hpp"

#include <algorithm>
#include <imgui.h>
#include <GLFW/glfw3.h>

InputManager::InputManager(settings& settingsRef)
    : _settings(settingsRef)
{
}

void InputManager::onMouseButton(int button, int action, int mods)
{
    (void)mods;

    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        _rightMouseDown = (action == GLFW_PRESS);
        if (_rightMouseDown)
            _firstMouseMove = true;
    }
}

void InputManager::onCursorPosition(double xpos, double ypos)
{
    _rightMouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Right);
    if (!_rightMouseDown)
    {
        _firstMouseMove = true;
        return;
    }

    if (_firstMouseMove)
    {
        _lastMouseX = xpos;
        _lastMouseY = ypos;
        _firstMouseMove = false;
        return;
    }

    const double deltaX = xpos - _lastMouseX;
    const double deltaY = ypos - _lastMouseY;

    _lastMouseX = xpos;
    _lastMouseY = ypos;

    _settings.cameraYawDegrees   += static_cast<float>(deltaX) * _mouseSensitivity;
    _settings.cameraPitchDegrees -= static_cast<float>(deltaY) * _mouseSensitivity;

    _settings.cameraPitchDegrees = std::clamp(_settings.cameraPitchDegrees, _minPitch, _maxPitch);
}

void InputManager::onScroll(double xoffset, double yoffset)
{
    (void)xoffset;
    applyZoom(static_cast<float>(yoffset));
}

void InputManager::update()
{
    const float wheel = ImGui::GetIO().MouseWheel;
    if (wheel != 0.0f)
        applyZoom(wheel);
}

void InputManager::applyZoom(float yoffset)
{
    _settings.cameraDistance -= yoffset * _zoomSpeed;
    _settings.cameraDistance = std::clamp(_settings.cameraDistance, _minDistance, _maxDistance);
}
