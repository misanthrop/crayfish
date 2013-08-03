require'libluada'

print('width:', luada.width)
print('height:', luada.height)

for i = 1,5 do
	luada.sleep(1000)
	local x = math.random(luada.width)
	local y = math.random(luada.height)
	luada.movepointer(x,y)
	luada.button(1,true)
	luada.sleep(50)
	luada.button(1,false)
end
