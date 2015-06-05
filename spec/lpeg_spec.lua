describe("lpjit.lpeg", function()
    it("has enough stack size", function()
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

    it("applies metamethods to 3d party objects", function()
        local m = require 'lpjit.lpeg'
        local mt = getmetatable(m.P(1))
        local p = mt.__add(function (s, i)
            return i
        end, function (s, i)
            return nil
        end)
        assert.truthy(m.match(p, "alo"))
    end)

    it("can apply match-time captures", function()
        local m = require 'lpjit.lpeg'
        local mt = getmetatable(m.P(1))
        local p = mt.__mul(function (s, i)
            return i
        end, function (s, i)
            return nil
        end)
        assert.falsy(m.match(p, "alo"))
    end)

    it("doesn't crash if match-time capture returns bad pos",
    function()
        local m = require"lpjit.lpeg"
        -- test for error messages
        local function checkerr(msg, f, ...)
            local st, err = pcall(f, ...)
            assert(not st and m.match({
                m.P(msg) + 1 * m.V(1)
            }, err))
        end
        local s = "hi, this is a test"
        checkerr("invalid position", m.match, function()
            return 2^20
        end, s)
    end)
end)
