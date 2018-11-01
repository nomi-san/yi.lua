milo = require 'milo'
t = milo.types -- C types

-- load dll/module/shared library
user32 = milo.load('user32')

-- define function            return  |  arguments                  | is stdcall
MsgBox = user32.MessageBoxA:def(t.int, {t.ptr, t.str, t.str, t.uint}, true)

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
