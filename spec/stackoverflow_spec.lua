describe("lpjit", function()
    local function works(pattern, text)
        return pcall(function()
            pattern:match(text)
        end)
    end

    it("restrict stack size", function()
        local lpeg = require 'lpeg'
        local lpjit = require 'lpjit'
        local pattern = lpeg.P {
            "(" * (lpeg.V(1))^0 * ")"
        }
        local INITBACK = 400 -- depends on lpeg setting
        lpeg.setmaxstack(INITBACK + 100) -- must be > INITBACK
        local lpeg_max
        local lpjit_max
        for i = 1, INITBACK + 100 do
            -- generate strings like (((())))
            local text = ('('):rep(i) .. (')'):rep(i)
            -- lpjit.compile depends on lpeg.setmaxstack
            local pattern2 = lpjit.compile(pattern)
            if not lpeg_max and not works(pattern, text) then
                lpeg_max = i
            end
            if not lpjit_max and not works(pattern2, text) then
                lpjit_max = i
            end
        end
        assert.equal(lpeg_max, lpjit_max)
        assert.truthy(lpeg_max > 1)
    end)
end)
