import csv
import re

flag = 0 #to skip column names
tVarRegEx = "^t\d+"
IDRegEx = "^[A-Za-z_][A-Za-z0-9_]*"
registers =[]
code =dict()
currentFunction ="main"
labelNum = 0
#code[".word"] = []
code[currentFunction]=[]
with open('TAC.csv') as csvfile:
	inp = csv.reader(csvfile, delimiter=',')
	for row in inp:
		if(flag==0):
			flag =1 
			continue
		if(row[1] == "="):
			if(re.search(tVarRegEx,row[4])):
				if(row[2].isdigit()):
					code[currentFunction].append(["MOV", row[4], row[2]])
					continue
				else:
					code[currentFunction].append(["LD", row[4], row[2]])
					continue
			if(re.search(IDRegEx, row[4])):
					code[currentFunction].append(["ST", row[4], row[2]])
		if(row[1] == "+"):
			code[currentFunction].append(["ADD", row[4],row[2],row[3]])
		if(row[1] == "binary-"):
			code[currentFunction].append(["SUB", row[4],row[2],row[3]])
		if(row[1] == "*"):
			code[currentFunction].append(["MUL", row[4],row[2],row[3]])
		if(row[1] == "/"):
			code[currentFunction].append(["DIV", row[4],row[2],row[3]])
		if(row[1] == "unary-"):
			currNum = code[currentFunction][-1][-1]
			code[currentFunction][-1][-1] = "-"+currNum
		if(row[1]== "=[]"):
			code[currentFunction].append(["MOV", row[4], row[3]+"("+row[2]+")"])
		if(row[1]== "<"): 
			#or row[1]== "<=" or row[1]= ">" or row[1] = ">=" or row)
			code[currentFunction].append(["IfFalse pending", "SUB", row[4], row[2], row[3], "BGZ"])
		if(row[1]== "<="): 
			code[currentFunction].append(["IfFalse pending", "SUB", row[4], row[2], row[3], "BGEZ"])
		if(row[1]==">"):
			code[currentFunction].append(["IfFalse pending", "SUB", row[4], row[2], row[3], "BLZ"])
		if(row[1]==">="):
			code[currentFunction].append(["IfFalse pending", "SUB", row[4], row[2], row[3], "BLEZ"])
		if(row[1] == "=="):
			code[currentFunction].append(["IfFalse pending", "SUB", row[4], row[2], row[3], "BEQ"])
		if(row[1] == "!="):
			code[currentFunction].append(["IfFalse pending", "SUB", row[4], row[2], row[3], "BNE"])
		if(row[1] == "If False"):
			for i in reversed(code[currentFunction]):
				#reverse search to handle nested if-else's or nested loops
				#
				if("IfFalse pending" in i):
					i.remove("IfFalse pending")
					code[currentFunction].append([i[-1], i[1], row[4]])
					i.remove(i[-1])
		

print(code)