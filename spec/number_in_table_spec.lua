describe("lpjit", function()
    it("uses named groups", function()
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

        local pattern = lpeg.P(rules)

        local text = '4.56'
        local m = pattern:match(text)

        local pattern2 = lpjit.compile(pattern)
        local m2 = pattern2:match(text)

        assert.equal(m.number, '4.56')
        assert.equal(m2.number, '4.56')
        assert.same(m2, m)
    end)
end)
