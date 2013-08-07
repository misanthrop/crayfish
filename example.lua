require'luada'

print('width:', luada.width)
print('height:', luada.height)

luada.key(luada.keys.lshift,true)
luada.press(luada.keys.h)
luada.key(luada.keys.lshift,false)
luada.press(luada.keys.e)
luada.press(luada.keys.l)
luada.press(luada.keys.l)
luada.press(luada.keys.o)

for i = 1,5 do
	luada.sleep(1000)
	local x = math.random(luada.width)
	local y = math.random(luada.height)
	luada.movepointer(x,y)
	luada.click(1)
	luada.sleep(50)
	luada.button(1)
	luada.sleep(100)
end

