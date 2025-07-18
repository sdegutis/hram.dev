-- foo

print('in foo!')

require 'bar'



local m = memory.malloc(3*3*4)
for i = 0,3*3*4-1 do memory.set(m+i, 8, math.random(0xff)-1) end
local img = image.create(m, 3, 3)
memory.free(m)


image.copy(nil, img, 2, 3)



function mousemove(x, y)
	image.copy(nil, img, x, y)
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
