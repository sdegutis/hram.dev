-- foo

print('in foo!')

require 'bar'

local memory = require "memory"


local m = memory.alloc(10)

for i = 0,9 do print(m[i]) end

print("#m", #m)
print("m[3]", m[3])
m[3] = 24
print("m[3]", m[3])

for i = 0,9 do print(m[i]) end


function mousemove(x, y)
	print("mouse moved", x, y)
end

function mousewheel(d)
	print("mouse wheel", d)
end

function mouseup(b)
	print("mouse up", b)
end

function mousedown(b)
	print("mouse up", b)
end

function keyup(k)
	print("key up", k)
end

function keydown(k)
	print("key up", k)
end

function keychar(s)
	print("key char", s)
end
