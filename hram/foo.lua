-- foo

print('in foo!')

require 'bar'



local m = memory.malloc(10)
print(m)

memory.fill(m, 0x0, 10)

--[[
print(memory.get(m, 8))
print(memory.set(m, -16, 256))
print(memory.get(m, 8))
--]]

--for i = 0,#m-1 do m[i] = math.random(0xff)-1 end

print()

for i=0,10-1 do print(i, memory.get(m+i, 8)) end
memory.fill(m, 0x3, 10)
memory.set(m+3, 32, 32769)
for i=0,10-1 do print(i, memory.get(m+i, 8)) end


local img = image.create(m, 4, 4)

-- img:draw(nil, 2, 3)

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
