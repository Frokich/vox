#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ncurses.h>
#include <algorithm>
#include <stack>
#include <chrono>
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

struct Config {
    bool line_number = true;
    bool expandTab = true;
    int tabWidth = 2;
    bool autoSave = true;
    int autoSaveInterval = 60;
    void load(const std::string& path) {
        lua_State* L = luaL_newstate();
        luaL_openlibs(L);
        if(luaL_dofile(L, path.c_str()) != LUA_OK) { lua_close(L); return; }
        lua_getglobal(L, "line_number"); if(lua_isboolean(L,-1)) line_number = lua_toboolean(L,-1); lua_pop(L,1);
        lua_getglobal(L,"expandTab"); if(lua_isboolean(L,-1)) expandTab = lua_toboolean(L,-1); lua_pop(L,1);
        lua_getglobal(L,"tabWidth"); if(lua_isinteger(L,-1)) tabWidth = lua_tointeger(L,-1); lua_pop(L,1);
        lua_getglobal(L,"autoSave"); if(lua_isboolean(L,-1)) autoSave = lua_toboolean(L,-1); lua_pop(L,1);
        lua_getglobal(L,"autoSaveInterval"); if(lua_isinteger(L,-1)) autoSaveInterval = lua_tointeger(L,-1); lua_pop(L,1);
        lua_close(L);
    }
};

struct EditorState {
    std::vector<std::string> lines;
    int cursorX;
    int cursorY;
};

class Editor {
public:
    Editor(const std::string& fname) : filename(fname) { config.load(std::string(getenv("HOME"))+"/.config/vox/init.lua"); }
    void open() {
        lines.clear();
        std::ifstream file(filename);
        std::string line;
        while(std::getline(file,line)) lines.push_back(line);
        if(lines.empty()) lines.push_back("");
        lastAutoSave = std::chrono::steady_clock::now();
    }
    void save() {
        std::ofstream file(filename);
        for(auto &line: lines) file << line << "\n";
        showMessage("Auto-saved");
    }
    void run() {
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        int ch;
        while(true) {
            draw();
            ch = getch();
            handleInput(ch);
            auto now = std::chrono::steady_clock::now();
            if(config.autoSave && std::chrono::duration_cast<std::chrono::seconds>(now - lastAutoSave).count() >= config.autoSaveInterval){
                save();
                lastAutoSave = now;
            }
        }
        endwin();
    }
private:
    std::string filename;
    std::vector<std::string> lines;
    Config config;
    int cursorX = 0;
    int cursorY = 0;
    int offsetY = 0;
    enum Mode { NORMAL, INSERT, CMD } mode = NORMAL;
    std::string cmdBuffer;
    std::stack<EditorState> undoStack;
    std::chrono::time_point<std::chrono::steady_clock> lastAutoSave;
    std::string message;
    std::chrono::time_point<std::chrono::steady_clock> messageTime;
    void pushUndo() { undoStack.push(EditorState{lines,cursorX,cursorY}); }
    void draw() {
        clear();
        int maxY, maxX;
        getmaxyx(stdscr, maxY, maxX);
        if(cursorY < offsetY) offsetY = cursorY;
        if(cursorY >= offsetY + maxY -1) offsetY = cursorY - maxY + 2;
        for(int i = 0; i < maxY-1 && i + offsetY < lines.size(); ++i) {
            if(config.line_number) printw("%4d ", i + 1 + offsetY);
            printw("%s\n", lines[i + offsetY].c_str());
        }
        move(maxY-1,0);
        clrtoeol();
        if(mode==NORMAL) printw("-- NORMAL --");
        else if(mode==INSERT) printw("-- INSERT --");
        else if(mode==CMD) printw(":%s", cmdBuffer.c_str());
        if(!message.empty()){
            auto now = std::chrono::steady_clock::now();
            if(std::chrono::duration_cast<std::chrono::seconds>(now - messageTime).count() < 2){
                mvprintw(maxY-1, maxX - message.size() - 1, "%s", message.c_str());
            } else message="";
        }
        move(cursorY - offsetY, cursorX + (config.line_number ? 5 : 0));
        refresh();
    }
    void showMessage(const std::string& msg){ message = msg; messageTime = std::chrono::steady_clock::now(); }
    void handleInput(int ch) {
        if(mode == NORMAL){
            static bool dPressed = false;
            switch(ch){
                case 'h': case KEY_LEFT: if(cursorX>0) cursorX--; break;
                case 'l': case KEY_RIGHT: if(cursorX<lines[cursorY].size()) cursorX++; break;
                case 'k': case KEY_UP: if(cursorY>0) cursorY--; cursorX=std::min(cursorX,(int)lines[cursorY].size()); break;
                case 'j': case KEY_DOWN: if(cursorY<lines.size()-1) cursorY++; cursorX=std::min(cursorX,(int)lines[cursorY].size()); break;
                case '0': cursorX=0; break;
                case '$': cursorX=lines[cursorY].size(); break;
                case 'i': mode=INSERT; break;
                case 'a': cursorX++; mode=INSERT; break;
                case 'o': pushUndo(); lines.insert(lines.begin()+cursorY+1,""); cursorY++; cursorX=0; mode=INSERT; break;
                case 'O': pushUndo(); lines.insert(lines.begin()+cursorY,""); cursorX=0; mode=INSERT; break;
                case 'x': if(cursorX<lines[cursorY].size()){ pushUndo(); lines[cursorY].erase(cursorX,1); } break;
                case 'd': if(dPressed){ pushUndo(); lines.erase(lines.begin()+cursorY); if(cursorY>=lines.size()) cursorY=lines.size()-1; cursorX=0; dPressed=false; } else dPressed=true; break;
                case 'u': undo(); break;
                case ':': mode=CMD; cmdBuffer=""; break;
                case 27: endwin(); exit(0); break;
                default: dPressed=false; break;
            }
        } else if(mode == INSERT){
            switch(ch){
                case 27: mode=NORMAL; break;
                case KEY_BACKSPACE: case 127:
                    if(cursorX>0){ pushUndo(); lines[cursorY].erase(cursorX-1,1); cursorX--; }
                    else if(cursorY>0){ pushUndo(); cursorX=lines[cursorY-1].size(); lines[cursorY-1]+=lines[cursorY]; lines.erase(lines.begin()+cursorY); cursorY--; }
                    break;
                case '\n': pushUndo(); lines.insert(lines.begin()+cursorY+1, lines[cursorY].substr(cursorX)); lines[cursorY]=lines[cursorY].substr(0,cursorX); cursorY++; cursorX=0; break;
                case '\t': pushUndo(); if(config.expandTab){ lines[cursorY].insert(cursorX, config.tabWidth, ' '); cursorX += config.tabWidth; } else { lines[cursorY].insert(cursorX,1,'\t'); cursorX++; } break;
                case KEY_LEFT: if(cursorX>0) cursorX--; break;
                case KEY_RIGHT: if(cursorX<lines[cursorY].size()) cursorX++; break;
                case KEY_UP: if(cursorY>0) cursorY--; cursorX=std::min(cursorX,(int)lines[cursorY].size()); break;
                case KEY_DOWN: if(cursorY<lines.size()-1) cursorY++; cursorX=std::min(cursorX,(int)lines[cursorY].size()); break;
                default: if(ch>=32 && ch<=126){ pushUndo(); lines[cursorY].insert(cursorX,1,(char)ch); cursorX++; } break;
            }
        } else if(mode==CMD){
            int maxY,maxX;
            getmaxyx(stdscr,maxY,maxX);
            if(ch==10){ processCommand(cmdBuffer); mode=NORMAL; }
            else if(ch==KEY_BACKSPACE || ch==127){ if(!cmdBuffer.empty()) cmdBuffer.pop_back(); }
            else if(ch>=32 && ch<=126){ cmdBuffer.push_back(ch); }
            else if(ch==27){ mode=NORMAL; }
        }
    }
    void processCommand(const std::string& cmd){
        if(cmd=="w") save();
        else if(cmd=="q") endwin(), exit(0);
        else if(cmd.substr(0,2)=="e "){ filename=cmd.substr(2); open(); cursorX=cursorY=offsetY=0; }
    }
    void undo(){ if(!undoStack.empty()){ EditorState state = undoStack.top(); undoStack.pop(); lines = state.lines; cursorX = state.cursorX; cursorY = state.cursorY; if(cursorY<offsetY) offsetY=cursorY; } }
};

int main(int argc, char** argv){
    if(argc<2){ std::cout<<"Usage: "<<argv[0]<<" <file>\n"; return 1; }
    Editor editor(argv[1]);
    editor.open();
    editor.run();
    return 0;
}

