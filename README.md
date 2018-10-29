# milo
**Milö - C library for Lua.**

### Features

- [x] Multiple types
- [x] Calling C shared function
- [ ] Structure
- [ ] Closures
- [ ] Memory management

### OS Supported

- [x] Windows
- [ ] Linux

### Example: MsgBox

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




