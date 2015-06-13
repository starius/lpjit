local lpeg = require 'lpeg'
local lpjit = require 'lpjit'

lpeg.setmaxstack(10000)

local V, R, S, L = lpeg.V, lpeg.R, lpeg.S, lpeg.locale()

local rules = {
    "Tree",
    Tree = (V("Subtree") + V("Branch")) * ";",
    Subtree = V("Leaf") + V("Internal"),
    Leaf = V("Name"),
    Internal = "(" * V("BranchSet") * ")" * V("Name")^-1,
    BranchSet = V("Branch") * ("," * V("BranchSet")) ^ -1,
    Branch = V("Subtree") * V("Length")^-1,
    Name = (1 - S(";,():"))^1,
    Length = (":" * L.digit^1 * ("." * L.digit^1)^-1),
}

rules.Name = lpeg.Cg(rules.Name, "name")
rules.Length = lpeg.Cg(rules.Length, "length")
rules.Branch = lpeg.Ct(rules.Branch)
rules.Tree = lpeg.Ct(rules.Tree)

local pattern = lpeg.P(rules)

local text = io.open('benchmark/sample.nwk'):read('*all')

local pattern2 = lpjit.compile(pattern)
local m2 = pattern2:match(text)

local t1 = os.clock()
local m = pattern:match(text)
local t2 = os.clock()
print('lpeg ', t2 - t1)

local t1 = os.clock()
local m = pattern2:match(text)
local t2 = os.clock()
print('lpjit ', t2 - t1)
