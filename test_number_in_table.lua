local lpeg = require 'lpeg'
local lpjit = require 'lpjit'

local V, R, S, L = lpeg.V, lpeg.R, lpeg.S, lpeg.locale()

local rules = {
    "G",
    number = (L.digit^1 * ("." * L.digit^1)^-1),
    G = V("number"),
}

rules.number = lpeg.Cg(rules.number, "number")
rules.G = lpeg.Ct(rules.G)

pattern = lpeg.P(rules)

--lpeg.pcode(pattern)

text = '4.56'
m = pattern:match(text)

pattern2 = lpjit.compile(pattern)
m2 = pattern2:match(text)

assert(m.number == '4.56' and m.number == m2.number)
