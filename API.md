# API

## Create a script

### Example script

```lua
-- Path: scripts/example.lua

function init()
    -- This function is called once when the script is loaded.
    -- You can use it to initialize variables.
end

function update()
    -- This function is called every frame.
    -- You can use it to update variables.
end

function render()
    -- This function is called every frame.
    -- You can use it to draw on the screen.
end

function cleanup()
    -- This function is called once when the script is unloaded.
    -- You can use it to clean up variables.
end

module.exports = {
    name = "Example script",
    author = "ZoSand",
    description = "This is an example script",
    version = "1.0.0",
    wallpaperInit = Init,
    wallpaperUpdate = Update,
    wallpaperRender = Render,
    wallpaperCleanup = Cleanup
}
```