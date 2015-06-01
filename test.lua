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

local pattern = 1 - lpeg.S 'abc'
--lpeg.pcode(pattern)
local pattern2 = lpjit.compile(pattern)
assert(pattern2:match('x') == 2)
assert(pattern2:match('arb') == nil)
assert(pattern2:match('bcb') == nil)

local pattern = 1 - lpeg.P 'abc'
--lpeg.pcode(pattern)
local pattern2 = lpjit.compile(pattern)
assert(pattern2:match('x') == 2)
assert(pattern2:match('arb') == 2)
assert(pattern2:match('abc') == nil)
assert(pattern2:match('') == nil)

local pattern = lpeg.P {
    "(" * (lpeg.V(1))^0 * ")"
}
local pattern2 = lpjit.compile(pattern)
assert(pattern2:match('()') == 3)
assert(pattern2:match('(()())') == 7)
assert(pattern2:match('((') == nil)
