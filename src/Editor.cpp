#include "Editor.h"
#include "LuaManager.h"
#include "Terminal.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <chrono>
#include <iomanip>

#ifdef _WIN32
#include <direct.h>
#define mkdir(dir, mode) _mkdir(dir)
#else
#include <sys/stat.h>
#endif

namespace fs = std::filesystem;

static std::string getHomeDir() {
    const char* home = std::getenv("HOME");
    if (home) return std::string(home);
#ifdef _WIN32
    const char* userProfile = std::getenv("USERPROFILE");
    if (userProfile) return std::string(userProfile);
#endif
    return "./.config/vox";
}

std::string Editor::getConfigDir() {
    return getHomeDir() + "/.config/vox";
}

std::string Editor::getPluginsDir() {
    return getConfigDir() + "/plugins";
}

void Editor::ensureConfigDirs() {
    std::string configDir = getConfigDir();
    std::string pluginsDir = getPluginsDir();

    std::error_code ec;

    if (!fs::exists(configDir)) {
        fs::create_directories(configDir, ec);
        std::cout << "Created config directory: " << configDir << std::endl;
    }

    if (!fs::exists(pluginsDir)) {
        fs::create_directories(pluginsDir, ec);
        std::cout << "Created plugins directory: " << pluginsDir << std::endl;

        std::string examplePlugin = pluginsDir + "/example.lua";
        if (!fs::exists(examplePlugin)) {
            std::ofstream f(examplePlugin);
            f << R"(
editor.log("✨ Example plugin loaded!")
editor.bind_command(":hello", function()
    editor.log("👋 Hello from plugin!")
    editor.set_status_message("Plugin says hello!")
    local lines = editor.get_lines()
    table.insert(lines, "✨ Hello from Lua plugin!")
end)
)";
            f.close();
            std::cout << "Created example plugin: " << examplePlugin << std::endl;
        }
    }
}

Editor::Editor() {
    m_lines.push_back("");
    ensureConfigDirs();
}

std::vector<std::string>& Editor::getLines() {
    return m_lines;
}

void Editor::insertLine(int index, const std::string& line) {
    if (index < 0 || index > static_cast<int>(m_lines.size())) return;
    m_lines.insert(m_lines.begin() + index, line);
    setStatusMessage("Inserted line at " + std::to_string(index));
}

void Editor::deleteLine(int index) {
    if (index < 0 || index >= static_cast<int>(m_lines.size())) return;
    m_lines.erase(m_lines.begin() + index);
    setStatusMessage("Deleted line " + std::to_string(index));
}

void Editor::replaceLine(int index, const std::string& line) {
    if (index < 0 || index >= static_cast<int>(m_lines.size())) return;
    m_lines[index] = line;
    setStatusMessage("Replaced line " + std::to_string(index));
}

int Editor::getCursorRow() const { return m_cursorRow; }
int Editor::getCursorCol() const { return m_cursorCol; }

void Editor::setCursor(int row, int col) {
    if (row >= 0 && row < static_cast<int>(m_lines.size())) m_cursorRow = row;
    if (col >= 0) m_cursorCol = col;
    setStatusMessage("Cursor moved to (" + std::to_string(row) + ", " + std::to_string(col) + ")");
}

void Editor::bindCommand(const std::string& cmd, std::function<void()> callback) {
    m_commands[cmd] = std::move(callback);
}

void Editor::triggerEvent(const std::string& event) {
    std::cout << "[Event] " << event << std::endl;
}

void Editor::setStatusMessage(const std::string& msg) {
    m_statusMessage = msg;
}

std::string Editor::getStatusMessage() const {
    return m_statusMessage;
}

void Editor::renderStatusLine() {
    Terminal::clearLine(); // Очистим текущую строку

    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");

    int lines = m_lines.size();
    std::string mode = "NORMAL";

    std::cout << "\033[48;5;236m\033[38;5;15m"; // Темно-серый фон, белый текст
    std::cout << " 🅥ᴼ🅧 ";
    std::cout << "\033[38;5;10m" << lines << "L" << " "; // Зеленый
    std::cout << "\033[38;5;14m" << mode << " "; // Голубой
    std::cout << "\033[38;5;13m" << m_cursorRow+1 << ":" << m_cursorCol+1 << " "; // Пурпурный
    std::cout << "\033[38;5;226m" << ss.str() << " "; // Желтый
    std::cout << "\033[38;5;208m" << " » " << m_statusMessage; // Оранжевый
    std::cout << "\033[0m"; // Сброс
    std::cout << std::string(10, ' '); // Дополнительное заполнение справа
    std::cout << "\r"; // Возврат каретки
    std::cout.flush();
}

void Editor::render() {
    Terminal::clear();
    Terminal::moveCursor(0, 0);

    const int maxLines = 20; // Ограничим вывод для красоты

    for (int i = 0; i < maxLines && i < static_cast<int>(m_lines.size()); ++i) {
        if (i == m_cursorRow) {
            std::cout << "\033[38;5;11m\033[1m> " << m_lines[i] << "\033[0m" << std::endl;
        } else {
            std::cout << "  " << m_lines[i] << std::endl;
        }
    }

    for (int i = m_lines.size(); i < maxLines; ++i) {
        std::cout << std::endl;
    }

    renderStatusLine();
    std::cout << "\n: ";
}

void Editor::handleInput() {
    std::string input;
    std::getline(std::cin, input);

    if (input.empty()) {
        setStatusMessage("Empty input");
        return;
    }

    if (input == ":q") {
        std::cout << "\033[0m\nBye! ✨\n";
        exit(0);
    } else if (input == ":help") {
        std::cout << "\nCommands: :q, :help, :lua <code>, :hello (from plugin)\n";
        setStatusMessage("Help shown");
    } else if (input.substr(0, 5) == ":lua ") {
        LuaManager::instance().executeString(input.substr(5));
        setStatusMessage("Executed Lua code");
    } else if (m_commands.count(input)) {
        m_commands[input]();
    } else {
        insertLine(m_cursorRow + 1, input);
        m_cursorRow++;
        setStatusMessage("Inserted new line");
    }
}

void Editor::loadConfig() {
    std::string configPath = getConfigDir() + "/init.lua";
    if (fs::exists(configPath)) {
        std::cout << "Loading config: " << configPath << std::endl;
        LuaManager::instance().executeFile(configPath);
    } else {
        std::ofstream f(configPath);
        f << R"(
-- ~/.config/vox/init.lua
editor.log("🚀 vox config loaded!")
editor.bind_command(":init", function()
    editor.log("Intialized via config!")
    editor.set_status_message("Config says: Hello!")
end)
editor.set_status_message("Config loaded")
)";
        f.close();
        std::cout << "Created default config: " << configPath << std::endl;
        LuaManager::instance().executeFile(configPath);
    }
}

void Editor::loadPlugins() {
    std::string pluginsDir = getPluginsDir();

    if (fs::exists(pluginsDir) && fs::is_directory(pluginsDir)) {
        for (const auto& entry : fs::directory_iterator(pluginsDir)) {
            if (entry.path().extension() == ".lua") {
                std::cout << "Loading plugin: " << entry.path().filename().string() << std::endl;
                LuaManager::instance().executeFile(entry.path().string());
            }
        }
    }
}

void Editor::run() {
    LuaManager::instance().initialize(this);
    loadConfig();
    loadPlugins();

    std::cout << "\nWelcome to \033[1;36mvox\033[0m — minimalist extensible editor.\n";
    setStatusMessage("Started successfully");

    while (true) {
        render();
        handleInput();
    }
}
