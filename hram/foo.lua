-- foo

print('in foo!')

require 'bar'

local memory = require "memory"

local m = memory.new(10)
print("here", m:len())
print("here", m:get(0))
print("here", m:set(0, 24))
print("here", m:get(0))


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
