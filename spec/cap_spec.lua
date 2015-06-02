describe("lpjit", function()
    it("captures parts of pattern", function()
        local lpeg = require 'lpeg'
        local lpjit = require 'lpjit'

        local pattern = lpeg.P 'a' * lpeg.C('b') * lpeg.P('c')

        local pattern2 = lpjit.compile(pattern)
        assert.equal('b', pattern2:match('abc'))
        assert.falsy(pattern2:match('ff'))
    end)

    local allchar = {}
    for i = 0, 255 do
        allchar[i + 1] = i
    end
    allchar = string.char(unpack(allchar))
    assert(#allchar == 256)

    pending("can grow list of captures", function()
        local lpeg = require 'lpeg'
        local lpjit = require 'lpjit'

        local pattern = lpeg.Cs((lpeg.R'09' + lpeg.P(1)/"")^0)

        lpeg.pcode(pattern)

        local pattern2 = lpjit.compile(pattern)
        for i = 1, 1000 do
            assert.equal('0123456789', pattern2:match(allchar))
        end
    end)
end)
