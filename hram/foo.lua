-- foo

print('in foo!')

require 'bar'

local memory = require "memory"
local image = require "image"


local m = memory.alloc(4*4*4)
for i = 0,#m-1 do m[i] = math.random(0xff)-1 end

local img = image.create(m, 4, 4)

-- img:copy(nil, 2, 3)

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
