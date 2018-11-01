milo = require('milo')
t = milo.types
closure = milo.closure

-- define closure     
add = closure (
	function(a, b)        -- function
    	return a + b
	end,
	t.int, {t.int, t.int} -- return type, arguments
)

ret = add(4, 8) -- call, like Lua pure
print(ret)
print(-add) --> closure function address pointer, use for C closure/callback
