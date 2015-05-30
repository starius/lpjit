local lpeg = require 'lpeg'
local lpjit = require 'lpjit'

local pattern = lpeg.P 'abc'
--lpeg.ptree(pattern)
--lpeg.pcode(pattern)

local pattern2 = lpjit.compile(pattern)
print(pattern2:match('abc'))
