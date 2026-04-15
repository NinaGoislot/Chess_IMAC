#include "Managers/InputManager.hpp"
#include <algorithm>
#include <stdexcept>

InputManager& InputManager::instance()
{
    static InputManager manager;
    return manager;
}

void InputManager::bindSettings(settings& settingsRef)
{
    _settings       = &settingsRef;
    _firstMouseMove = true;
}

void InputManager::onCursorPosition(double xpos, double ypos)
{
    if (!ImGui::IsMouseDown(ImGuiMouseButton_Right))
    {
        _firstMouseMove = true;
        return;
    }

    if (_firstMouseMove)
    {
        _lastMouseX     = xpos;
        _lastMouseY     = ypos;
        _firstMouseMove = false;
        return;
    }

    const double deltaX = xpos - _lastMouseX;
    const double deltaY = ypos - _lastMouseY;

    _lastMouseX = xpos;
    _lastMouseY = ypos;

    settings& settings = settingsRef();
    settings.cameraYawDegrees += static_cast<float>(deltaX) * _mouseSensitivity;
    settings.cameraPitchDegrees -= static_cast<float>(deltaY) * _mouseSensitivity;

    settings.cameraPitchDegrees = std::clamp(settings.cameraPitchDegrees, _minPitch, _maxPitch);
}

void InputManager::onScroll(double xoffset, double yoffset)
{
    (void)xoffset;

    applyZoom(static_cast<float>(yoffset));
}

settings& InputManager::settingsRef()
{
    if (_settings == nullptr)
        throw std::logic_error("InputManager used before bindSettings().");

    return *_settings;
}

void InputManager::applyZoom(float yoffset)
{
    settings& settings = settingsRef();

    if (settings.cameraPieceTarget)
        return;

    settings.cameraDistance -= yoffset * _zoomSpeed;
    settings.cameraDistance = std::clamp(settings.cameraDistance, _minDistance, _maxDistance);
}
