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

    it("doesn't crash if match-time capture returns #bad pos",
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

    it("works with grammars with 'strange values'", function()
        local m = require"lpjit.lpeg"
        local p = m.P {
            "print",
            print = m.V(print),
            [print] = m.V(_G),
            [_G] = m.P"a",
        }
    end)

    it("doesn't crash if fail after closeruntime", function()
        local m = require"lpjit.lpeg"
        local function id(s, i, ...)
            return true, ...
        end
        local p = m.Cmt(m.S'abc' / {a = 'x', c = 'y'}, id)
        local p2 = (p + m.R'09'^1 / string.char + m.P(1))^0
        local p3 = m.Cmt(m.Cs(p2), id)
        assert.equal("xyb\98+\68y", p3:match("acb98+68c"))
    end)

    it("doesn't crash if runtime capture throws", function()
        local lpeg = require 'lpjit.lpeg'
        pcall(function()
            lpeg.match(function()
                pcall(function()
                    error 'test'
                end)
            end, '')
        end)
    end)

    it("doesn't crash if fail after closeruntime (min)",
    function()
        local m = require"lpjit.lpeg"
        local function id(s, i, ...)
            return true, ...
        end
        local p = m.Cmt(m.S'abc', id)
        local p2 = (p + m.R'09'^1 + m.P(1))^0
        assert(p2:match("acb98+68c"))
    end)

    it("doesn't crash if fail after closeruntime (min min)",
    function()
        local m = require"lpjit.lpeg"
        local function id(s, i, ...)
            return i, ...
        end
        local p = m.Cmt(m.P'a', id) * m.P'b'
        p:match("ac")
    end)

    it("doesn't panic (#luajit, bad runtime capture position)",
    function()
        local m = require "lpjit.lpeg"
        local function f()
            return 2^20
        end
        assert.has_error(function()
            m.match(f, "hi, this is a test")
        end)
    end)
end)
