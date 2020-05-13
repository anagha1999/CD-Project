x=3
#this is a comment
y=2
z =[1,2,3]

for i in z:
	print(i)
	x= x+1 

for i in range(0,3):
	y = y+1

if(x <5):
	print("small")
elif(x==5):
	print("med")
else:
	print("large")

'''look, another comment!'''

while(x<=100):
	x = x+2*3/4-1

def sum(a,b):
	c= a+b
	return c

z= sum(x,y)
print(x)