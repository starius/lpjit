local lpeg = require 'lpeg'
local lpjit = require 'lpjit'

local function genParenthesis()
    local abc = 'qwertyuiop[sdfghjklvbnmxcv56789\n'
    local t = {}
    math.randomseed(0)
    for i = 1, 50 do
        table.insert(t, '(')
    end
    for i = 1, 10000 do
        if math.random(1, 2) == 1 then
            table.insert(t, '(')
        else
            table.insert(t, ')')
        end
        if math.random(1, 2) == 1 then
            local c = math.random(1, #abc)
            table.insert(t, abc:sub(c, c))
        end
    end
    return table.concat(t)
end

local text = genParenthesis()

local report = '%s %d %.10f'

local pattern = lpeg.P {
    "(" * ((1 - lpeg.S"()") + lpeg.V(1))^0 * ")"
}
local t1 = os.clock()
local res = pattern:match(text)
local t2 = os.clock()
print(report:format('lpeg', res, t2 - t1))

local pattern2 = lpjit.compile(pattern)
local t1 = os.clock()
local res = pattern2:match(text)
local t2 = os.clock()
print(report:format('asm ', res, t2 - t1))

local t1 = os.clock()
local check = text:match('%b()')
local t2 = os.clock()
print(report:format('lua ', #check, t2 - t1))
assert(res == #check + 1)
