local lpjit_lpeg = {}

local lpjit = require 'lpjit'
local lpeg = require 'lpeg'

local unpack = unpack or table.unpack

local mt = {}

local wrapped = setmetatable({}, {__mode='k'})
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

local function cachedRawWrap(pattern)
    if wrapped[pattern] then
        return wrapped[pattern]
    end
    local obj = rawWrap(pattern)
    wrapped[pattern] = obj
    return obj
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
        return cachedRawWrap(pattern)
    end
end

local function unwrapPattern(obj)
    if getmetatable(obj) == mt then
        return rawUnwrap(obj)
    else
        return obj
    end
end

local function unwrapGrammar(obj)
    -- unwrap all values
    local obj2 = {}
    for k, v in pairs(obj) do
        obj2[k] = unwrapPattern(v)
    end
    return obj2
end

function unwrap(obj)
    if type(obj) == 'table' and getmetatable(obj) ~= mt then
        -- P { grammar }
        return unwrapGrammar(obj)
    else
        return unwrapPattern(obj)
    end
end

local function wrapGenerator(E, unwrap_func)
    unwrap_func = unwrap_func or unwrap
    return function(...)
        local args = {...}
        local first = unwrap_func(args[1])
        local nargs = select('#', ...)
        if nargs == 0 then
            return wrapPattern(E())
        elseif nargs == 1 then
            return wrapPattern(E(first))
        else
            return wrapPattern(E(first, select(2, ...)))
        end
    end
end

for _, E in ipairs {'B', 'S', 'R', 'Cf', 'Cs', 'Cmt', 'Carg',
        'Ct', 'P', 'Cc', 'Cp', 'Cg', 'Cb', 'C'} do
    lpjit_lpeg[E] = wrapGenerator(lpeg[E])
end
lpjit_lpeg.V = wrapGenerator(lpeg.V, unwrapPattern)

local lpeg_mt = getmetatable(lpeg.P(1))

for _, binop in ipairs {'__unm', '__mul', '__add', '__sub',
        '__div', '__pow', '__len'} do
    mt[binop] = function(a, b)
        a = unwrap(a)
        b = unwrap(b)
        local f = assert(lpeg_mt[binop])
        return wrapPattern(f(a, b))
    end
end

function lpjit_lpeg.match(obj, ...)
    if not compiled[obj] then
        obj = unwrapPattern(obj)
        if lpeg.type(obj) ~= 'pattern' then
            obj = unwrapPattern(lpjit_lpeg.P(obj))
        end
        compiled[obj] = lpjit.compile(obj)
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
    local funcs = t or {}
    for k, v in pairs(funcs0) do
        funcs[k] = wrapPattern(v)
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
    if lpeg.type(obj) == 'pattern' then
        return "pattern"
    end
    return nil
end

function lpjit_lpeg.pcode(obj)
    return lpeg.pcode(unwrapPattern(obj))
end

function lpjit_lpeg.ptree(obj)
    return lpeg.ptree(unwrapPattern(obj))
end

return lpjit_lpeg
