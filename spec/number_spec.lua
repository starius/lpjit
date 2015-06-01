describe("lpjit", function()
    it("parses a number as a part of a grammar", function()
        local lpeg = require 'lpeg'
        local lpjit = require 'lpjit'
        local V, R, S, L = lpeg.V, lpeg.R, lpeg.S, lpeg.locale()

        local rules = {
            "number",
            number = (L.digit^1 * ("." * L.digit^1)^-1),
        }
        rules.number = lpeg.C(rules.number)

        local pattern = lpeg.P(rules)
        local text = '56.78'

        local m = pattern:match(text)

        local pattern2 = lpjit.compile(pattern)
        local m2 = pattern2:match(text)

        assert.equal(m,m2)
    end)
end)
