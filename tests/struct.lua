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

print('value of MyBool is: ' .. tostring(MyBool.value))
                                     --> MyBool[1]
