# API

## Create a script

### Example script

```lua
-- Path: scripts/example.lua

local exports = {
    name = "Example script",
    author = "ZoSand",
    description = "This is an example script",
    version = "1.0.0"
}

function exports.init()
    -- This function is called once when the script is loaded.
    -- You can use it to initialize variables.
end

function exports.update()
    -- This function is called every frame.
    -- You can use it to update variables.
end

function exports.render()
    -- This function is called every frame.
    -- You can use it to draw on the screen.
end

function exports.cleanup()
    -- This function is called once when the script is unloaded.
    -- You can use it to clean up variables.
end

return exports
```