describe("lpjit", function()
    it("returns index after last character matched", function()
        local lpeg = require 'lpeg'
        local lpjit = require 'lpjit'
        local pattern = lpeg.P 'abc'
        local pattern2 = lpjit.compile(pattern)
        assert.equal(4, pattern2:match('abc'))
        assert.falsy(pattern2:match('ff'))
    end)

    it("works with choices", function()
        local lpeg = require 'lpeg'
        local lpjit = require 'lpjit'
        local pattern = lpeg.P 'abc' + lpeg.P 'arb'
        local pattern2 = lpjit.compile(pattern)
        assert.equal(4, pattern2:match('abc'))
        assert.equal(4, pattern2:match('arb'))
        assert.falsy(pattern2:match('acb'))
    end)

    it("works with char sets", function()
        local lpeg = require 'lpeg'
        local lpjit = require 'lpjit'
        local pattern = 1 - lpeg.S 'abc'
        local pattern2 = lpjit.compile(pattern)
        assert.equal(2, pattern2:match('x'))
        assert.falsy(pattern2:match('arb'))
        assert.falsy(pattern2:match('bcb'))
    end)

    it("works with 'fail twice'", function()
        local lpeg = require 'lpeg'
        local lpjit = require 'lpjit'
        local pattern = 1 - lpeg.P 'abc'
        local pattern2 = lpjit.compile(pattern)
        assert.equal(2, pattern2:match('x'))
        assert.equal(2, pattern2:match('arb'))
        assert.falsy(pattern2:match('abc'))
        assert.falsy(pattern2:match(''))
    end)

    it("matches balanced parenthesis", function()
        local lpeg = require 'lpeg'
        local lpjit = require 'lpjit'
        local pattern = lpeg.P {
            "(" * (lpeg.V(1))^0 * ")"
        }
        local pattern2 = lpjit.compile(pattern)
        assert.equal(3, pattern2:match('()'))
        assert.equal(7, pattern2:match('(()())'))
        assert.falsy(pattern2:match('(('))
    end)

    it("matches balanced parenthesis with extra chars",
    function()
        local lpeg = require 'lpeg'
        local lpjit = require 'lpjit'
        local pattern = lpeg.P {
            "(" * ((1 - lpeg.S"()") + lpeg.V(1))^0 * ")"
        }
        local pattern2 = lpjit.compile(pattern)
        assert.equal(3, pattern2:match('()'))
        assert.equal(5, pattern2:match('(dd)'))
        assert.equal(11, pattern2:match('(d()dd(f))'))
        assert.falsy(pattern2:match('(('))
    end)

    it("matches balanced parenthesis (lpeg/test.lua)",
    function()
        local m = require 'lpeg'
        local lpjit = require 'lpjit'
        local pattern = m.P {
            "(" * (((1 - m.S"()") + #m.P"(" * m.V(1))^0) * ")"
        }
        local pattern2 = lpjit.compile(pattern)
        assert.equal(7, pattern2:match('(al())()'))
    end)
end)
