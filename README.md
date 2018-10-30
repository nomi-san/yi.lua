# milo
**Milö - C library for Lua.**

### Features

- [x] Multiple types
- [x] Calling C shared function
- [ ] Structure
- [ ] Closures
- [ ] Memory management
- [ ] Auto GC

### OS Supported

- [x] Windows
- [ ] Linux

## Example 

### Struct

```lua
milo = require('milo')
struct = milo.struct
types = milo.types

Point = struct {
    types.int,
    types.int
}

Point {20, 55} 
--> Point[1] = 20 --> Point:set(1, 20)
--> Point[2] = 55 --> Point:set(2, 55)

print('Point -> x: ' .. Point[1] .. ', y: ' .. Point[2])
                    --> Point:get(1)      -->  Point:get(2)
```

### MsgBox

```lua
milo = require('milo')
types = milo.types -- C types

-- load dll/module/shared library
user32 = milo.load('user32')

-- define function                return  |  arguments                                  | stdcall
MsgBox = user32.MessageBoxA:def(types.int, {types.ptr, types.str, types.str, types.uint}, true)

-- MessageBox constant
local MB_OK, MB_ICONINFORMATION, MB_YESNO, IDYES
    = 0x0000, 0x0040, 0x0004, 6

-- call & set return
ret = MsgBox(nil, 'Hello, world!', 'MsgBox', MB_OK + MB_ICONINFORMATION + MB_YESNO)

if (ret == IDYES) then
    print('Yes!')
else
    print('No!')
end
```

### Types

`milo.types.`| description
:---:|:---
`u8`, `u16`,<br>`u32` (`uint32`),<br>`u64` (`uint64`)| unsigned integer
`i8`, `i16`,<br>`i32` (`int32`),<br>`i64` (`int64`)| signed integer
`ptr`| pointer
`f32`, `f64` (`num`)| floating number
`str`, `wstr`| string, unicode string




