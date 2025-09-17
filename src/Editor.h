#pragma once
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <filesystem>

class Editor {
public:
    Editor();
    void run();

    std::vector<std::string>& getLines();
    void insertLine(int index, const std::string& line);
    void deleteLine(int index);
    void replaceLine(int index, const std::string& line);
    int getCursorRow() const;
    int getCursorCol() const;
    void setCursor(int row, int col);
    void bindCommand(const std::string& cmd, std::function<void()> callback);
    void triggerEvent(const std::string& event);

    static std::string getConfigDir();
    static std::string getPluginsDir();

    void setStatusMessage(const std::string& msg);
    std::string getStatusMessage() const;

private:
    std::vector<std::string> m_lines;
    int m_cursorRow = 0;
    int m_cursorCol = 0;
    std::string m_statusMessage = "Ready.";
    std::unordered_map<std::string, std::function<void()>> m_commands;

    void render();
    void handleInput();
    void loadConfig();
    void loadPlugins();
    void ensureConfigDirs();
    void renderStatusLine();
};
