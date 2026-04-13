#include "Managers/InputManager.hpp"
#include <algorithm>

// -------- CONSTRUCTOR --------
InputManager::InputManager(settings& settingsRef)
    : _settings(settingsRef)
{
}

// -------- CALLBACKS --------
void InputManager::onCursorPosition(double xpos, double ypos)
{
    // Read button state directly from ImGui
    if (!ImGui::IsMouseDown(ImGuiMouseButton_Right))
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

// -------- HELPERS --------
void InputManager::applyZoom(float yoffset)
{
    if (_settings.cameraPieceTarget) return;

    _settings.cameraDistance -= yoffset * _zoomSpeed;
    _settings.cameraDistance = std::clamp(_settings.cameraDistance, _minDistance, _maxDistance);
}