describe("lpjit", function()
    it("captures parts of pattern", function()
        local lpeg = require 'lpeg'
        local lpjit = require 'lpjit'

        local pattern = lpeg.P 'a' * lpeg.C('b') * lpeg.P('c')

        local pattern2 = lpjit.compile(pattern)
        assert.equal('b', pattern2:match('abc'))
        assert.falsy(pattern2:match('ff'))
    end)
end)
