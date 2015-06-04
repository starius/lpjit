describe("lpjit.lpeg", function()
    it("has enough stack size", function()
        lpeg.setmaxstack(5)
        local m = require 'lpjit.lpeg'
        local b = {[1] =
            "(" * (((1 - m.S"()") + #m.P"(" * m.V(1))^0) * ")"
        }
        local p1 = (#m.P(b) * 1) * m.Cp()
        local p2 = m.P {
            [1] = p1 + (1 * m.V(1))
        }
        assert.equal(7, m.match(p2, "  (  (a)"))
    end)

    it("pass same number of args to impl", function()
        local m = require 'lpjit.lpeg'
        assert.equal(nil, m.match(m.Cc(nil), ""))
    end)

    it("passes argument captures", function()
        local m = require 'lpjit.lpeg'
        assert.equal(print, m.match(m.Carg(1), 'a', 1, print))
    end)
end)