# milo
Milö - C library for Lua.

### Features
- Call C shared function
- Structure
- Closures
- Memory management
- Multiple types

### OS Supported

- [x] Windows
- [ ] Linux

### 
```lua
milo = require('milo')
types = milo.types -- C types

-- load dll/module/shared library
user32 = milo.load('user32')

-- define function                return  |  arguments                                  | stdcall
MsgBox = user32.MessageBoxA:def(types.int, {types.ptr, types.str, types.str, types.uint}, true)

-- MessageBox constant
MB_OK, MB_ICONINFORMATION, MB_YESNO = 0x0000, 0x0040, 0x0004
IDYES = 6

-- call & set return
ret = MsgBox(nil, 'Hello, world!', 'MsgBox', MB_OK + MB_ICONINFORMATION + MB_YESNO)

if (ret == IDYES) then
    print('Yes!')
else
    print('No!')
end

```
