describe("lpjit", function()
    local allchar = {}
    for i = 0, 255 do
        allchar[i + 1] = i
    end
    allchar = string.char(unpack(allchar))
    assert(#allchar == 256)

    it("works with lpeg.locale().lower", function()
        local lpeg = require 'lpeg'
        local lpjit = require 'lpjit'

        local L = {}
        lpeg.locale(L)

        local lower = assert(L.lower)

        local pattern = lpeg.Cs((lower + lpeg.P(1)/"")^0)

        local pattern2 = lpjit.compile(pattern)

        for i = 1, 1000 do
            assert.equal('abcdefghijklmnopqrstuvwxyz',
                pattern2:match(allchar))
        end
    end)
end)
