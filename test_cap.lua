local lpeg = require 'lpeg'
local lpjit = require 'lpjit'

local pattern = lpeg.P 'a' * lpeg.C('b') * lpeg.P('c')
--lpeg.ptree(pattern)
--lpeg.pcode(pattern)

local pattern2 = lpjit.compile(pattern)
assert(pattern2:match('abc') == 'b')
assert(pattern2:match('ff') == nil)
