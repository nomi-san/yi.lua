milo = require 'milo'
enum = milo.enum

OS = enum {
    'MSDos',   -- 0, count from 0
    'Windows', -- 1

    'Unix', 5, -- 5, set is 5
    'Ubuntu',  -- 6
    'OSX',     -- 7

    'Android', -- 8
    'iOS', -6, -- -6, set is -6
    'Linage'   -- -5
}

print(OS.MSDos, OS.Unix, OS.Linage)
print(#OS) -- count len
print(-OS) -- get address pointer
