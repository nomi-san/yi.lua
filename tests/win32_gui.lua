milo = require('milo')
struct = milo.struct
closure = milo.closure
t = milo.types

user32 = milo.load('user32')

-- functions needed
ShowWindow          = user32.ShowWindow:def(t.int, {t.ptr, t.int}, true)
DefWindowProc       = user32.DefWindowProcA:def(t.int, {t.ptr, t.uint, t.uint, t.uint}, true)
CreateWindowEx      = user32.CreateWindowExA:def(t.ptr, 
                        {t.uint, t.str, t.str, t.uint, t.int, t.int, t.int, t.int, t.ptr, t.ptr, t.ptr, t.ptr}, true)
RegisterClassEx     = user32.RegisterClassExA:def(t.int, {t.ptr}, true)
GetMessage          = user32.GetMessageA:def(t.bool, {t.ptr, t.ptr, t.int, t.int}, true)
TranslateMessage    = user32.TranslateMessage:def(t.int, {t.ptr}, true)
DispatchMessage     = user32.DispatchMessageA:def(t.int, {t.ptr}, true)
MoveWindow          = user32.MoveWindow:def(t.int, {t.ptr, t.int, t.int, t.int, t.int, t.bool}, true)
PostQuitMessage     = user32.PostQuitMessage:def(t.none, {t.int}, true)

-- WNDCLASSEX
wcex = struct {
    'cbSize', t.uint;
    'style', t.uint;
    'lpfnWndProc', t.ptr;
    'cbClsExtra', t.int;
    'cbWndExtra', t.int;
    'hInstance', t.ptr;
    'hIcon', t.ptr;
    'hCursor', t.ptr;
    'hbrBackground', t.ptr;
    'lpszMenuName', t.ptr;
    'lpszClassName', t.str;
    'hIconSm', t.ptr;
}

msg = -struct { t.ptr, t.uint, t.uint, t.uint, t.uint, t.int, t.int, t.uint }

-- window procedure with closure
wndproc = closure (
    function(hwnd, msg, wp, lp)
        if (msg == 0x0010) then
            PostQuitMessage(0)
        end
        -- define for window message
        return DefWindowProc(hwnd, msg, wp, lp)
    end,
    t.int, {t.ptr, t.uint, t.uint, t.uint}
)

name = 'Hello, world!'
class = 'MyCustomWindow'

-- register class
wcex.cbSize = wcex[0]
wcex.style = 0x1 | 0x2
wcex.lpszClassName = class
wcex.lpfnWndProc = (-wndproc)
RegisterClassEx(-wcex)

-- create window, control
hwnd = CreateWindowEx(0, class, name, 0x10CF0000, 100, 100, 500, 300, nil, nil, nil, nil)
label = CreateWindowEx(0, 'static', 'Test static label!', 0x50000201, 50, 50, 200, 100, hwnd, nil, nil, nil)

-- show window
ShowWindow(hwnd, 1)

milo.sleep(250)

-- start animate window
for i=1,100 do
    MoveWindow(hwnd, 100+i, 100+i, 500, 300, false)
    milo.sleep(10)
end

for i=1,100 do
    MoveWindow(hwnd, 200+i, 200-i, 500, 300, false)
    milo.sleep(10)
end

for i=1,100 do
    MoveWindow(hwnd, 300+i, 100+i, 500, 300, false)
    milo.sleep(10)
end

-- window message loop
while GetMessage(msg, nil, 0, 0) do
    TranslateMessage(msg)
    DispatchMessage(msg)
end
