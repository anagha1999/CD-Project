def p():
	print("fn s()")


def s():
	p()

def q(i):
	if(i==0):
		x = i-1
		q(x)
		s()


y=5
q(y)