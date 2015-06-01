local lpeg = require 'lpeg'
local lpjit = require 'lpjit'

local pattern = lpeg.P(('abc'):rep(10000))
for i = 0, 1000 do
    if i % 20 == 0 then
        print(i)
    end
    lpjit.compile(pattern)
end
io.read()
collectgarbage()
io.read()
