describe("lpjit", function()

    local lpeg = require 'lpeg'
    local lpjit = require 'lpjit'

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

    it("parses tree topology in newick format", function()
        local text = '((A,B):7,(C:5,D:2.3));'
        local m = pattern:match(text)

        local pattern2 = lpjit.compile(pattern)
        local m2 = pattern2:match(text)
        assert.same(m, m2)
    end)
end)
