-- foo

print('in foo!')

require 'bar'

local memory = require "memory"

local m = memory.new(10)
print("here", #m)
print("here", m[0])
m[0] = 24
print("here", m[0])


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
