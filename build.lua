local build = {}
-- To do: varargs to main so these can be changed from the command line
COMPILER = 'gcc'
LANG = 'c'
LIBRARIES = {
    'server',
    'main'
}
BINARY_NAME = 'server.exe'
INCLUDE = "-lws2_32"
FLAGS = ""

local function clean(objects)
    for i in ipairs(objects) do
        os.remove(objects[i])
    end
end

local function compile_program(cmd)
    print("Preparing to run final build command: ", cmd)
    local m = os.execute(cmd)
    if m then
        print("Compiler reports success! Cleaning up...")
    else
        print("Something went wrong, please check that the build options are correct.")
    end
end

local function prep_program(arr)
    local str = COMPILER .. " " .. FLAGS .. " "
    for i in ipairs(arr) do
        str = str .. arr[i] .. " "
    end
    str = str .. " -o " ..  BINARY_NAME .. " " .. INCLUDE
    return str
end

local function prep_cmds()
    local t = {}
    local o = {}
    for i in ipairs(LIBRARIES) do
        local out = LIBRARIES[i] .. ".o"
        t[i] = COMPILER .. " -c " .. LIBRARIES[i] .. "." .. LANG .. " -o " .. out
        o[i] = out
    end
    print("Preparing to run the following commands: ")
    for i in ipairs(t) do
        print(t[i])
    end
    return t, o
end

local function compile_objects(arr)
    for i in ipairs(arr) do
        local m = os.execute(arr[i])
        print(arr[i] .. ": success = ", m)
    end
end


function build.main()
    local cmds, objects = prep_cmds()
    compile_objects(cmds)
    local final_cmd = prep_program(objects)
    compile_program(final_cmd)
    clean(objects)
end

build.main()

return build