import csv
import re
#stackStart = 800
mainStart = 100

flag = 0 #to skip column names
tVarRegEx = "^t\d+"
IDRegEx = "^[A-Za-z_][A-Za-z0-9_]*"

code =dict() #key = name of the function, value = assmebly code

registerDescriptor = {
			'R1' : None, 'R2' : None, 'R3' : None, 'R4' : None, 'R5' : None, 'R11' : None, 
			'R12' : None, 'R13' : None, 'R14' : None, 'R15' : None}
freeRegisters = []
for register in registerDescriptor.keys():
	freeRegisters.append(register)
busyRegisters = []

#Registers for passing function parameters
freeParamRegs =["R7", "R8", "R9", "R10"]
busyParamRegs =[]





funcLoc = dict() #key = name of the function, value = [curr code area, startcode Area]


currentFunction ="main"
#funcStack=[]
#funcStack.append(currentFunction) #stack used to keep track of caller and callee functions
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


def getRegister(temp,currRow):
	#print(registerDescriptor)

	for key in registerDescriptor:
		if(registerDescriptor[key] == temp):
			return key
		

	if len(freeRegisters) == 0:
		foundCurrRow =0 #need to scan the code from current row onwards to find which register can be replaced
		indexToReplace =0 #index of register in busyRegisters[] to replace with new value
		with open('TAC.tsv', 'r') as tsvfile:
			inp2 = csv.reader(tsvfile, delimiter='\t')
			for row in inp2:
				if(row == currRow):
					foundCurrRow=1					

				if(foundCurrRow==1):
					try:
						regToReplace = busyRegisters[indexToReplace] #part linked to 
						tVarToReplace = registerDescriptor[regToReplace] #temp variable of register to replace
						if(tVarToReplace in row):
							indexToReplace+=1 #if the temp variable is used later in the code, you can't replace the register associated with it
							#so find a new register
					except:
						#will probably occur when indexToReplace > len(busyRegisters)
						#this means that no other register can afford to be replaced
						print(sys.exc_info()[0])
						print("Cannot compile code. Architecture supports only 10 GPRs")
						exit(0)
				else:
					continue

			registerDescriptor[regToReplace] = temp 
			return regToReplace
				

	else:
		register = freeRegisters.pop(0)
		busyRegisters.append(register)
		registerDescriptor[register] = temp
		return register

	

def updatefuncLoc(arr):
	funcLoc[currentFunction][0]+=4
	for el in arr:
		if(el.isdigit()):
			funcLoc[currentFunction][0]+=4

def getOpRegs(a1, a2, r, row):
	#in case all the operands in a binary operation are not temp variables
	#we need to add separate instructions to handle them
	op1 = getRegister(a1, row)
	op2 = getRegister(a2, row)
	res = getRegister(r, row)

	if(re.search(tVarRegEx,a1)):
		#argument 1 is a temp variable
		pass
	elif(a1.isdigit()):
		#if arg1 is a digit
		code[currentFunction].append([funcLoc[currentFunction][0], "MOV", op1, a1])
		funcLoc[currentFunction][0]+=8
	else:
		#if arg1 is an ID		
		code[currentFunction].append([funcLoc[currentFunction][0], "LD", op1, a1])
		funcLoc[currentFunction][0]+=8

	if(re.search(tVarRegEx,a2)):
		#argument 2 is a temp variable
		pass
	elif(a2.isdigit()):
		#if arg2 is a digit
		code[currentFunction].append([funcLoc[currentFunction][0], "MOV", op2, a2])
		funcLoc[currentFunction][0]+=8
	else:
		#if arg2 is an ID
		code[currentFunction].append([funcLoc[currentFunction][0], "LD", op2, a2])
		funcLoc[currentFunction][0]+=8
		

	if(re.search(tVarRegEx,r)):
		#r is a temp variable
		pass		
	elif(a2.isdigit()):
		#if r is a digit
		code[currentFunction].append([funcLoc[currentFunction][0], "MOV", res, r])
		funcLoc[currentFunction][0]+=8
	else:
		#r is an ID 
		code[currentFunction].append([funcLoc[currentFunction][0], "LD", res, r])
		funcLoc[currentFunction][0]+=8
		pass

	return op1,op2,res

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
					code[currentFunction].append([funcLoc[currentFunction][0], "MOV", getRegister(row[4], row), row[2]])
					funcLoc[currentFunction][0]+=8
					continue
				else:
					#you're loading some other variable value(from memory) into a temp variable
					code[currentFunction].append([funcLoc[currentFunction][0], "LD", getRegister(row[4], row), row[2]])
					funcLoc[currentFunction][0]+=8
					continue
			if(re.search(IDRegEx, row[4])):
				#if result = variable, you're storing a value in the memory
				if(row[2].isdigit()):
					code[currentFunction].append([funcLoc[currentFunction][0], "MOV", getRegister(row[2],row), row[2]])
					code[currentFunction].append([funcLoc[currentFunction][0], "ST", row[4],  getRegister(row[2],row)])
				else:
					code[currentFunction].append([funcLoc[currentFunction][0], "ST", row[4], getRegister(row[2], row)])
				funcLoc[currentFunction][0]+=8

				#adding to .data section + preventing repeat additions of the same variable
				toAppend = [row[4]+ ": "+ ".word"]
				flag2=0
				for el in code[".data"]:
					if(el == toAppend):
						flag2=1
						break
				if(flag2==0):
					code[".data"].append([row[4]+ ": "+ ".word"])
				
		if(row[1]=="ListDecl"):
			code[".data"].append([row[4]+ ": "+ ".BLKW  " + row[2]])
		if(row[1] == "+"):
			op1, op2, res = getOpRegs(row[2].strip(), row[3].strip(), row[4].strip(), row)
			code[currentFunction].append([funcLoc[currentFunction][0], "ADD", res, op1, op2])
			updatefuncLoc([row[4],row[2],row[3]])
		if(row[1] == "binary-"):
			op1, op2, res = getOpRegs(row[2].strip(), row[3].strip(), row[4].strip(), row)
			code[currentFunction].append([funcLoc[currentFunction][0], "SUB", res, op1, op2])
			updatefuncLoc([row[4],row[2],row[3]])
		if(row[1] == "*"):
			op1, op2, res = getOpRegs(row[2].strip(), row[3].strip(), row[4].strip(), row)
			code[currentFunction].append([funcLoc[currentFunction][0],"MUL", res, op1, op2])
			updatefuncLoc([row[4],row[2],row[3]])
		if(row[1] == "/"):
			op1, op2, res = getOpRegs(row[2].strip(), row[3].strip(), row[4].strip(), row)
			code[currentFunction].append([funcLoc[currentFunction][0],"DIV", res, op1, op2])
			updatefuncLoc([row[4],row[2],row[3]])
		if(row[1] == "and"):
			op1, op2, res = getOpRegs(row[2].strip(), row[3].strip(), row[4].strip(), row)
			code[currentFunction].append([funcLoc[currentFunction][0],"AND", res, op1, op2])
			updatefuncLoc([row[4],row[2],row[3]])
		if(row[1] == "or"):
			op1, op2, res = getOpRegs(row[2].strip(), row[3].strip(), row[4].strip(), row)
			code[currentFunction].append([funcLoc[currentFunction][0],"OR", res, op1, op2])
			updatefuncLoc([row[4],row[2],row[3]])
		if(row[1] == "unary-"):
			#the number would've been stored as +ve in the asm code and register descriptor
			#correcting +ve value of num to -ve in code
			currNum = code[currentFunction][-1][-1]
			code[currentFunction][-1][-1] = "-"+currNum

			#correcting +ve value of num to -ve in registerDescriptor
			reg = code[currentFunction][-1][-2]
			for key in registerDescriptor:
				if(key == reg):
					registerDescriptor[key] = row[4]
					break

		if(row[1]== "=[]"):
			code[currentFunction].append([funcLoc[currentFunction][0],"MOV", getRegister(row[4],row), row[3]+"("+row[2]+")"])
			updatefuncLoc([row[4], row[3]+"("+row[2]+")"])
		if(row[1]== "<"): 
			op1,op2, res = getOpRegs(row[2].strip(),row[3].strip(),row[4].strip(),row)

			code[currentFunction].append([funcLoc[currentFunction][0],"IfFalse pending", "SUB", res, op1, op2, "BGZ"])
			updatefuncLoc([row[4], row[2], row[3]])
		if(row[1]== "<="): 
			op1, op2, res = getOpRegs(row[2].strip(), row[3].strip(), row[4].strip(), row)
			code[currentFunction].append([funcLoc[currentFunction][0],"IfFalse pending", "SUB", res, op1, op2, "BGEZ"])
			updatefuncLoc([row[4], row[2], row[3]])
		if(row[1]==">"):
			op1, op2, res = getOpRegs(row[2].strip(), row[3].strip(), row[4].strip(), row)
			code[currentFunction].append([funcLoc[currentFunction][0],"IfFalse pending", "SUB", res, op1, op2, "BLZ"])
			updatefuncLoc([row[4], row[2], row[3]])
		if(row[1]==">="):
			op1, op2, res = getOpRegs(row[2].strip(), row[3].strip(), row[4].strip(), row)
			code[currentFunction].append([funcLoc[currentFunction][0],"IfFalse pending", "SUB", res, op1, op2, "BLEZ"])
			updatefuncLoc([row[4], row[2], row[3]])
		if(row[1] == "=="):
			op1, op2, res = getOpRegs(row[2].strip(), row[3].strip(), row[4].strip(), row)
			code[currentFunction].append([funcLoc[currentFunction][0],"IfFalse pending", "SUB", res, op1, op2, "BEQ"])
			updatefuncLoc([row[4], row[2], row[3]])
		if(row[1] == "!="):
			op1, op2, res = getOpRegs(row[2].strip(), row[3].strip(), row[4].strip(), row)
			code[currentFunction].append([funcLoc[currentFunction][0],"IfFalse pending", "SUB", res, op1, op2, "BNE"])
			updatefuncLoc([row[4], row[2], row[3]])

		if(row[1] == "If False"):
			for i in reversed(code[currentFunction]):
				#reverse search to handle nested if-else's or nested loops
				if("IfFalse pending" in i):
					i.remove("IfFalse pending")
					code[currentFunction].append([funcLoc[currentFunction][0], i[-1], i[2] , row[4]])
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

			#return back to the CALLER
			code[currentFunction].append([funcLoc[currentFunction][0],"BR", "0($sp)"])
			updatefuncLoc(["0($sp)"])

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
			#return value = current location + 20 because
			#12 bytes: ST instruction 
			#8 bytes: BR instruction
			code[currentFunction].append([funcLoc[currentFunction][0], "ST", "0($sp)", str(funcLoc[currentFunction][0]+20)])
			#updatefuncLoc(["0($sp)", str(funcLoc[currentFunction][0]+20)])
			funcLoc[currentFunction][0]+=12

			calledFuncLoc = str(funcLoc[row[2]][1]) 
			code[currentFunction].append([funcLoc[currentFunction][0], "BR", calledFuncLoc])
			updatefuncLoc([calledFuncLoc])

			#making sure $sp points to caller's activation record
			code[currentFunction].append([funcLoc[currentFunction][0],"ADD", "$sp", "$sp", "20"])
			updatefuncLoc(["$sp", "$sp", "20"])

		if(row[1]=="Print"):
			#print(row)
			reg= getRegister(row[2], row)
			code[currentFunction].append([funcLoc[currentFunction][0], "MOV", "R0", reg ])
			updatefuncLoc(["MOV", "R0", reg])
			code[currentFunction].append([funcLoc[currentFunction][0], "SYSCALL"])
			funcLoc[currentFunction][0]+=4





dataCode = code[".data"]
code.pop(".data")
print(".data")
for line in dataCode:
	#print(line)
	for el in line:
		print(el)
print()
mainCode = code["main"]
code.pop("main")
print("main:")
for line in mainCode:
	#print(line)
	print(line[0], ":", end=" ")
	print(line[1], end = " ")
	#print(line[2:-1])
	for el in line[2:-1]:
		print(el, end = ", ")
	if(line[1:-1]!=[]):
		print(line[-1])
	else:
		print()
print()
for key in code:
	print(key)
	for line in code[key]:
		print(line[0], ":", end=" ")
		print(line[1], end = " ")
		#print(line[2:-1])
		for el in line[2:-1]:
			print(el, end = ", ")
		if(line[1:-1]!=[]):
			print(line[-1])
		else:
			print()
#print(code)