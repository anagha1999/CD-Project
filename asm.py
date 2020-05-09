import csv
import re
#stackStart = 800
mainStart = 100

flag = 0 #to skip column names
tVarRegEx = "^t\d+"
IDRegEx = "^[A-Za-z_][A-Za-z0-9_]*"
registers =[]
freeParamRegs =["R7", "R8", "R9", "R10"]
busyParamRegs =[]
funcLoc = dict() #key = name of the function, value = [ curr code area, startcode Area]
code =dict() #key = name of the function, value = assmebly code


currentFunction ="main"
funcStack=[]
funcStack.append(currentFunction) #stack used to keep track of caller and callee functions
startCodeArea = 100
currCodeArea = 100

funcLoc["main"] = [currCodeArea, startCodeArea]
code[currentFunction]=[]
code[".data"] =[]

#initializing "main" code
code["main"].append([funcLoc["main"][0], "LD", "$sp", "800"]) 
funcLoc["main"][0]+=8
code["main"].append([funcLoc["main"][0], "SUB", "$sp", "$sp", "20"])	
funcLoc["main"][0]+=8

def updatefuncLoc(arr):
	funcLoc[currentFunction][0]+=4
	for el in arr:
		if(el.isdigit()):
			funcLoc[currentFunction][0]+=4

with open('TAC.tsv') as csvfile:
	inp = csv.reader(csvfile, delimiter='\t')
	for row in inp:
		if(flag==0):
			flag =1 
			continue
		if(row[1] == "="):
			if(re.search(tVarRegEx,row[4])):
				#if result = temp variable
				if(row[2].isdigit()):
					#if you're moving a constant into a temp variable
					code[currentFunction].append([funcLoc[currentFunction][0], "MOV", row[4], row[2]])
					funcLoc[currentFunction][0]+=8
					continue
				else:
					#you're loading some other variable value(from memory) into a temp variable
					code[currentFunction].append([funcLoc[currentFunction][0], "LD", row[4], row[2]])
					funcLoc[currentFunction][0]+=8
					continue
			if(re.search(IDRegEx, row[4])):
				#if result = variable, you're storing a value in the memory
				code[".data"].append([row[4]+ ": "+ ".word"])
				code[currentFunction].append([funcLoc[currentFunction][0], "ST", row[4], row[2]])
				funcLoc[currentFunction][0]+=8
		if(row[1]=="ListDecl"):
			code[".data"].append([row[4]+ ": "+ ".BLKW  " + row[2]])
		if(row[1] == "+"):
			code[currentFunction].append([funcLoc[currentFunction][0], "ADD", row[4],row[2],row[3]])
			updatefuncLoc([row[4],row[2],row[3]])
		if(row[1] == "binary-"):
			code[currentFunction].append([funcLoc[currentFunction][0], "SUB", row[4],row[2],row[3]])
			updatefuncLoc([row[4],row[2],row[3]])
		if(row[1] == "*"):
			code[currentFunction].append([funcLoc[currentFunction][0],"MUL", row[4],row[2],row[3]])
			updatefuncLoc([row[4],row[2],row[3]])
		if(row[1] == "/"):
			code[currentFunction].append([funcLoc[currentFunction][0],"DIV", row[4],row[2],row[3]])
			updatefuncLoc([row[4],row[2],row[3]])
		if(row[1] == "unary-"):
			currNum = code[currentFunction][-1][-1]
			code[currentFunction][-1][-1] = "-"+currNum
		if(row[1]== "=[]"):
			code[currentFunction].append([funcLoc[currentFunction][0],"MOV", row[4], row[3]+"("+row[2]+")"])
			updatefuncLoc([row[4], row[3]+"("+row[2]+")"])
		if(row[1]== "<"): 
			#or row[1]== "<=" or row[1]= ">" or row[1] = ">=" or row)
			code[currentFunction].append([funcLoc[currentFunction][0],"IfFalse pending", "SUB", row[4], row[2], row[3], "BGZ"])
			updatefuncLoc([row[4], row[2], row[3]])
		if(row[1]== "<="): 
			code[currentFunction].append([funcLoc[currentFunction][0],"IfFalse pending", "SUB", row[4], row[2], row[3], "BGEZ"])
			updatefuncLoc([row[4], row[2], row[3]])
		if(row[1]==">"):
			code[currentFunction].append([funcLoc[currentFunction][0],"IfFalse pending", "SUB", row[4], row[2], row[3], "BLZ"])
			updatefuncLoc([row[4], row[2], row[3]])
		if(row[1]==">="):
			code[currentFunction].append([funcLoc[currentFunction][0],"IfFalse pending", "SUB", row[4], row[2], row[3], "BLEZ"])
			updatefuncLoc([row[4], row[2], row[3]])
		if(row[1] == "=="):
			code[currentFunction].append([funcLoc[currentFunction][0],"IfFalse pending", "SUB", row[4], row[2], row[3], "BEQ"])
			updatefuncLoc([row[4], row[2], row[3]])
		if(row[1] == "!="):
			code[currentFunction].append([funcLoc[currentFunction][0],"IfFalse pending", "SUB", row[4], row[2], row[3], "BNE"])
			updatefuncLoc([row[4], row[2], row[3]])
		if(row[1] == "If False"):
			for i in reversed(code[currentFunction]):
				#reverse search to handle nested if-else's or nested loops
				if("IfFalse pending" in i):
					i.remove("IfFalse pending")
					code[currentFunction].append([funcLoc[currentFunction][0], i[-1], i[2], row[4]])
					updatefuncLoc([i[-1], i[2], row[4]])
					i.remove(i[-1])
		if(row[1] == "Label"):
			code[currentFunction].append([funcLoc[currentFunction][0], row[4]+ ":"])
			funcLoc[currentFunction][0]+=2
		if(row[1] == "goto"):
			code[currentFunction].append([funcLoc[currentFunction][0], "BR", row[4]])
			funcLoc[currentFunction][0]+=4

		if(row[1]== "BeginF"):
			currentFunction=row[2]
			code[currentFunction]=[]
			startCodeArea+=200
			currCodeArea+=200
			funcLoc[currentFunction] = [currCodeArea, startCodeArea]
			code[currentFunction].append([funcLoc[currentFunction][0], row[2]+ ":"])

		if(row[1]== "EndF"):
			#code[currentFunction].append([funcLoc[currentFunction][0], "BR", "0($sp)"])
			currentFunction = "main"
			#no support for nested function definitions

		if(row[1]== "Param"):
			if(len(freeParamRegs)==0):
				print("Too many parameters, ignoring ", row[2])
			else:
				
				temp = freeParamRegs[0]
				freeParamRegs.remove(freeParamRegs[0])
				busyParamRegs.append(temp)
				code[currentFunction].append([funcLoc[currentFunction][0], "MOV", temp, row[2]])
				updatefuncLoc([temp, row[2]])
			#print(freeParamRegs, busyParamRegs)
		if(row[1]== "Call"):
			busyParamRegs=[]
			freeParamRegs=["R7", "R8", "R9", "R10"]
			#subtract sp by size of AR of function being called
			code[currentFunction].append([funcLoc[currentFunction][0],"SUB", "$sp", "$sp", "20"])
			updatefuncLoc(["$sp", "$sp", "20"])

			#store return value in AR of function being called
			#return value = current location + 16 because
			#8 bytes: ST instruction 
			#8 bytes: BR instruction
			code[currentFunction].append([funcLoc[currentFunction][0], "ST", "0($sp)", str(funcLoc[currentFunction][0]+16)])
			updatefuncLoc(["0($sp)", str(funcLoc[currentFunction][0]+16)])

			calledFuncLoc = str(funcLoc[row[2]][1]) 
			code[currentFunction].append([funcLoc[currentFunction][0], "BR", calledFuncLoc])
			updatefuncLoc([calledFuncLoc])

			#making sure $sp points to caller's activation record
			code[currentFunction].append([funcLoc[currentFunction][0],"ADD", "$sp", "$sp", "20"])
			updatefuncLoc(["$sp", "$sp", "20"])

			if(currentFunction!="main"):
				#going back to the caller
				code[currentFunction].append([funcLoc[currentFunction][0],"BR", "0($sp)"])
				updatefuncLoc(["0($sp)"])




dataCode = code[".data"]
code.pop(".data")
print(".data")
for line in dataCode:
	print(line)

mainCode = code["main"]
code.pop("main")
print("main:")
for line in mainCode:
	print(line)

for key in code:
	print(key)
	for line in code[key]:
		print(line)
#print(code)