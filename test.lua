local lpeg = require 'lpeg'
local lpjit = require 'lpjit'

local pattern = lpeg.P 'abc'
--lpeg.ptree(pattern)
--lpeg.pcode(pattern)

local pattern2 = lpjit.compile(pattern)
assert(pattern2:match('abc') == 4)
assert(pattern2:match('ff') == nil)

local pattern = lpeg.P 'abc' + lpeg.P 'arb'
--lpeg.pcode(pattern)
local pattern2 = lpjit.compile(pattern)
assert(pattern2:match('abc') == 4)
assert(pattern2:match('arb') == 4)
assert(pattern2:match('acb') == nil)
