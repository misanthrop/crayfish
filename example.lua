cf = require'crayfish'

print('width:', cf.width())
print('height:', cf.height())

cf.key(cf.lshift,true)
cf.press(cf.h)
cf.key(cf.lshift,false)
cf.press(cf.e)
cf.press(cf.l)
cf.press(cf.l)
cf.press(cf.o)
--[[
for i = 1,5 do
	cf.sleep(1000)
	local x = math.random(cf.width())
	local y = math.random(cf.height())
	cf.movepointer(x,y)
	cf.click(cf.lbutton)
end
]]
