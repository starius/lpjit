local lpeg = require 'lpeg'
local lpjit = require 'lpjit'

local V, R, S, L = lpeg.V, lpeg.R, lpeg.S, lpeg.locale()

local rules = {
    "number",
    number = (L.digit^1 * ("." * L.digit^1)^-1),
}

rules.number = lpeg.C(rules.number)

pattern = lpeg.P(rules)

--lpeg.pcode(pattern)

text = '56.78'
m = pattern:match(text)

pattern2 = lpjit.compile(pattern)
m2 = pattern2:match(text)

assert(m == m2)
