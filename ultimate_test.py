x=5
#this is a comment
y=2
z =[]
#this loop below doesn't work with optimization
for i in z:
	print(x) #will cause seg fault if x has not been declared!
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

