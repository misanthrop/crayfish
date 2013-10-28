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

cf.key(cf.lalt,true)
cf.press(cf.tab)
cf.key(cf.lalt,false)
