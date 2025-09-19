-- scripts/example.lua
-- Example script for 'vox' editor

print("Hello from Lua!")

-- Set custom status on startup
vox.status("Welcome to vox + Lua!")

-- Insert text at cursor
-- vox.insert("Hello Lua!\n")

-- Bind Ctrl+T to insert timestamp
function insert_timestamp()
    local now = os.date("%Y-%m-%d %H:%M:%S")
    vox.insert(now)
end

-- Note: key binding system would require extending C++ side
-- This is a placeholder — you'd need to implement keymap in C++
print("Press Ctrl+T to insert timestamp (not bound yet)")
