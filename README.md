<p>
    <p align="center">
        <img src="https://i.imgur.com/ilAiPVd.png" width="250">
    </p>
    <h1 align="center">C library for Lua</h1>
    <h3 align="center">"LibFFI is awesome, but I don't like it."</h3>
    <br>
</p>

### Features
- Easy to use
- High performance
- [x] Multiple static types
- [x] Calling C shared function
- [x] Structure
- [x] Closures
- [x] Auto GC
- [ ] Classes
- [ ] Memory management

### OS Supported
- [x] Windows
- [ ] Linux

### Build

```bash
$ git clone https://github.com/wy3/milo.git
$ cd milo
$ gcc -shared -w -o milo.dll ./src/*.c -Iuser/lua/src -Luser/lua -llua53
```

- When build with **MSVC**, please disable **SDL Check** (or arg **/sld-**); if not closure argument being crash after first.

<img src="https://i.imgur.com/zb2ogcQ.png">

### My technology

- Calling function:

```c
void* ret = alloc_heap(size_of_ret_type);
void* args = alloc_stack(sizeof_args_type);
// args + 0 := param[1]
// args + sizeof(param[to n])) := param[n]

ret = ((void*(*)())func_addr)(*(struct_not_union_t*)args);
*(double*)&ret = ((void*(*)())func_addr)(*(struct_not_union_t*)args); // double
```

> `struct_not_union_t` like variant union, but it's pure struct, see [`milo_not_union_t`](https://github.com/wy3/milo/blob/master/src/types.h#L57).

- Closure proxy:

```c
void* __proxy__(void* args) {
    // param[1] := args + 0
    // param[n] := args + sizeof(param[n-1])
}
```

- Struct array: like args of calling function.

## Example 

### MsgBox

```lua
milo = require('milo')
types = milo.types -- C types

-- load dll/module/shared library
user32 = milo.load('user32')

-- define function                return  |  arguments                                  | stdcall
MsgBox = user32.MessageBoxA:def(types.int, {types.ptr, types.str, types.str, types.uint}, true)

-- MessageBox constant
local MB_OK,  MB_ICONINFORMATION, MB_YESNO, IDYES
    = 0x0000, 0x0040,             0x0004,   6

-- call & set return
ret = MsgBox(nil, 'Hello, world!', 'MsgBox', MB_OK + MB_ICONINFORMATION + MB_YESNO)

if (ret == IDYES) then
    print('Yes!')
else
    print('No!')
end
```

### Struct

```lua
milo = require('milo')
struct = milo.struct
t = milo.types

Point = struct {
    t.int, --> index 1
    t.int  --> index 2
}

Point {20, 55} 
--> Point[1] = 20
--> Point[2] = 55

print('address: '   .. tostring( -Point ))
print('lenght: '    .. tostring( #Point ))
print('sizeof: '    .. tostring( Point[0] ))
print('value: x = ' .. tostring( Point[1] )
        .. ', y = ' .. tostring( Point[2] )
)

-- sugar syntax

MyBool = struct {
    'value', t.bool;
}

MyBool.value = false --> MyBool[1] = false

print('MyBool is: ' .. tostring(MyBool.value))
                            --> MyBool[1]
```

### Closure

```lua
milo = require('milo')
t = milo.types

function add(a, b)
    return a + b
end

-- define closure   func | type | arguments
add_c = milo.closure(add, t.int, {t.int, t.int})

ret = add_c(4, 8) -- call, like Lua pure
print(ret)
print(-add_c) --> closure function address pointer, use for C closure/callback
```

### Types

`milo.types.`| description
:---:|:---
`u8`, `u16`,<br>`u32` (`uint32`),<br>`u64` (`uint64`)| unsigned integer
`i8`, `i16`,<br>`i32` (`int32`),<br>`i64` (`int64`)| signed integer
`ptr`| pointer
`f32`, `f64` (`num`)| floating number
`str`, `wstr`| string, unicode string
