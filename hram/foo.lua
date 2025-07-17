-- foo

print('in foo!')

require 'bar'

function mousemove(x, y)
	print(string.format("mouse moved: %d x %d", x, y))
end
