local lpeg = package.loaded.lpeg
local re = package.loaded.re
package.loaded.lpeg = require 'lpjit.lpeg'
package.loaded.re = nil
local lpjit_re = require 're'
package.loaded.lpeg = lpeg
package.loaded.re = re
return lpjit_re
