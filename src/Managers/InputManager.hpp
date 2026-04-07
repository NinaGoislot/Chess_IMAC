#pragma once

class Board;

class InputManager {
public:
    // Constructors
    static InputManager& instance();
    InputManager(const InputManager&)            = delete;
    InputManager(InputManager&&)                 = delete;
    InputManager& operator=(const InputManager&) = delete;
    InputManager& operator=(InputManager&&)      = delete;
    ~InputManager()                              = default;

    void onCaseClicked(Board& board, int x, int y);

private:
    InputManager() = default;
};
