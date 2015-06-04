local lpjit_lpeg = {}

local lpjit = require 'lpjit'
local lpeg = require 'lpeg'

local mt = {}

local compiled = {}

mt.__index = lpjit_lpeg

local function rawWrap(pattern)
    local obj = {value = pattern}
    if newproxy and debug.setfenv then
        -- Lua 5.1 doesn't support __len for tables
        local obj2 = newproxy(true)
        debug.setmetatable(obj2, mt)
        debug.setfenv(obj2, obj)
        assert(debug.getfenv(obj2) == obj)
        return obj2
    else
        return setmetatable(obj, mt)
    end
end

local function rawUnwrap(obj)
    if type(obj) == 'table' then
        return obj.value
    else
        return debug.getfenv(obj).value
    end
end

local function wrapPattern(pattern)
    if getmetatable(pattern) == mt then
        -- already wrapped
        return pattern
    else
        return rawWrap(pattern)
    end
end

local function unwrapPattern(obj)
    if getmetatable(obj) == mt then
        return rawUnwrap(obj)
    else
        return obj
    end
end

local function wrapGenerator(E)
    return function(obj, ...)
        if type(obj) == 'table' and getmetatable(obj) ~= mt then
            -- P { grammar }
            -- unwrap all values
            local obj2 = {}
            for k, v in pairs(obj) do
                obj2[k] = unwrapPattern(v)
            end
            obj = obj2
        else
            obj = unwrapPattern(obj)
        end
        return wrapPattern(E(obj, ...))
    end
end

for _, E in ipairs {'B', 'S', 'R', 'Cf', 'Cs', 'Cmt', 'Carg',
        'Ct', 'P', 'Cc', 'Cp', 'Cg', 'Cb', 'V', 'C'} do
    lpjit_lpeg[E] = wrapGenerator(lpeg[E])
end

for _, binop in ipairs {'__unm', '__mul', '__add', '__sub',
        '__div', '__pow', '__len'} do
    mt[binop] = function(a, b)
        a = unwrapPattern(a)
        b = unwrapPattern(b)
        return wrapPattern(a, b)
    end
end

function lpjit_lpeg.match(obj, ...)
    if not compiled[obj] then
        compiled[obj] = lpjit.compile(unwrapPattern(obj))
    end
    return compiled[obj]:match(...)
end

function lpjit_lpeg.setmaxstack(...)
    lpeg.setmaxstack(...)
    -- clear cache ot compiled patterns
    compiled = {}
end

function lpjit_lpeg.locale(t)
    local funcs0 = lpeg.locale()
    local funcs = {}
    for k, v in pairs(funcs0) do
        funcs[k] = wrapPattern(v)
    end
    if t then
        for k, v in pairs(funcs) do
            t[k] = v
        end
    end
    return funcs
end

function lpjit_lpeg.version(t)
    return "lpjit with lpeg " .. lpeg.version()
end

function lpjit_lpeg.type(obj)
    if getmetatable(obj) == mt then
        return "pattern"
    end
    if lpeg.type(obj) then
        return "pattern"
    end
    return nil
end

if lpeg.pcode then
    function lpjit_lpeg.pcode(obj)
        return lpeg.pcode(unwrapPattern(obj))
    end
end

if lpeg.ptree then
    function lpjit_lpeg.ptree(obj)
        return lpeg.ptree(unwrapPattern(obj))
    end
end

return lpjit_lpeg
