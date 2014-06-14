#!/usr/bin/python
# -*- coding: ISO-8859-2 -*-

#Maszyna wirtualna realizująca podstawowe funkcje 
# potrzebne do przetestowania kompilatora
# 29.05.2008 - Piotr Błaszyński - piotrb@gmail.com
# Wszelkie modyfikacje i uwagi mile widziane
 
#Nazwa pliku wejściowego stała: 
#	filename = 'app.asm'

#Obsługiwane instrukcje: 
# JMP, JZ, JNZ, JG, JNG, JL, JNL, PUSH, POP (tylko do funkcji)
# MOV, ADD, SUB, MUL, DIV
# FMOV, FADD, FSUB, FMUL, FDIV 
# (te odróżnienie FLOAT-INT jest tylko formalne,
# bo w tej maszynie wirtualnej typ jest określany przez użyty rejestr (R|F)
# można więc pisać MOV F1,0.9 
# w architekturach rzeczywistych rozróżnienie to jest jednak istotne  

#Obsługiwane tryby i rejestry:
# R0-R7 - rejestry operujące na liczbach całkowitych 
# F0-F7 - rejestry operujące na liczbach zmiennoprzecinkowych
# #0-#n - pamieć maszyny wirtualnej
# 0-n	- stałe całkowite
# 0.0-r - stałe zmiennoprzecinkowe
# pierwszy argument jest miejscem docelowym
# drugi argument miejscem źródłowym
# !!!nie ma spacji po przecinku pomiędzy argumentami!!!
# przykład: MOV R0,#5 - z adresu 5 wyślij wartość do rejestru R0

#Adresy przy skokach to wartość ip-Instruction Pointer
# można:
# - (podstawowa metoda) - posłużyć sie stałą
# - podać adres w rejestrze lub pamięci
# Adresy przy skokach są liczone od 0
# warunek skoku opiera się na wyniku ostatniej operacji

from re import split
from random import randint
from string import ljust,rjust
from sys import argv
from math import floor
	
MEMSIZE = 128
		
def run_line(line):
	print line
	line_parts = split(' ', line)
	command = line_parts[0]
	args = split(',', line_parts[1])
	if command.endswith("MOV"):
		vm.mov_args(args)
	elif command.endswith("ADD"):
		vm.operate_binary(args, '+')
	elif command.endswith("SUB"):
		vm.operate_binary(args, '-')
	elif command.endswith("MUL"):
		vm.operate_binary(args, '*')
	elif command.endswith("DIV"):
		vm.operate_binary(args, '/')
	elif command.endswith("POW"):
		vm.operate_binary(args, '**')
	elif command == 'PUSH':
		vm.push(args)
	elif command == 'POP':
		vm.pop(args)
	elif command == 'JMP':
		vm.jump(args)
	elif command == 'JZ':	
		vm.jump_conditional(args, '==')
	elif command == 'JNZ':
		vm.jump_conditional(args, '<>')
	elif command == 'JG':	
		vm.jump_conditional(args, '>')
	elif command == 'JNG':
		vm.jump_conditional(args, '<=')
	elif command == 'JL':	
		vm.jump_conditional(args, '<')
	elif command == 'JNL':
		vm.jump_conditional(args, '>=')
	elif command.startswith('#'):
		pass
		
def run_file():
	if len(argv)>1:
		filename = argv[1]
	else:
		filename = 'app.asm'
	f=file(filename)
	program = f.readlines()
	f.close()
	vm.ip=0
	program_len = len(program)
	while vm.ip < program_len:
		line = program[vm.ip].rstrip().upper()
		vm.show_registers()
		print
		print "LINENO:" + rjust(str(vm.ip),3) + ':',
		vm.ip += 1
		run_line(line)
	vm.show_state()

class VirtualMachine:
	def __init__(self):
		self.execution_time = 0
		self.registers=[]
		self.float_registers=[]
		self.stack = []
		for i in range(8):
			self.registers.append(0)
			self.float_registers.append(0.0)
		self.memory = []
		for i in range(MEMSIZE):
			self.memory.append(randint(0, 255))
	
	def show_registers(self):
		column_width = 3
		print 'R:',
		for register in self.registers:
			print rjust(str(register), column_width),
		print '  F:',
		for float_register in self.float_registers:
			print rjust(str(float_register), column_width),
		print

	def print_row(self, row):
		print rjust(str(row) + '0',4) + ': ',
				
	def show_memory(self):
		print "memory:"
		row = 0
		column = 0
		self.print_row(row) 
		for byte in self.memory:
			print rjust(str(byte),5), 
			column += 1
			if column>9:
				column = 0
				row += 1
				print 
				self.print_row(row)
		print

	def show_time(self):
		print "time:"
		print self.execution_time
		print
	
	def show_state(self):
		self.show_registers()
		self.show_memory()
		self.show_time()
	
	def get_mem_index(self, argument):
			self.execution_time+=3
			return int(argument.lstrip('#'))

	def get_reg_mem_index(self, argument):
			self.execution_time+=1
			return int(argument.lstrip('@R'))

	def get_reg_index(self, argument):
			self.execution_time+=1
			return int(argument.lstrip('R'))

	def get_freg_index(self, argument):
			self.execution_time+=2
			return int(argument.lstrip('F'))
			
	def get_freg_mem_index(self, argument):
			self.execution_time+=2
			return float(argument.lstrip('@F'))
			
	def getval(self, argument):
		if argument.startswith('@R'):
			value = self.memory[int(self.registers[self.get_reg_mem_index(argument)])]
		elif argument.startswith('@F'):
			value = self.memory[float(self.registers[self.get_freg_mem_index(argument)])]
		elif argument.startswith('#'):
			value = self.memory[self.get_mem_index(argument)]
		elif argument.startswith('R'):
			value = self.registers[self.get_reg_index(argument)]
		elif argument.startswith('F'):
			value = float(self.float_registers[self.get_freg_index(argument)])
		else:
			self.execution_time+=1
			value = argument
		return str(value)
		
	def setval(self, argument, value):
		if argument.startswith('@R'):
			self.memory[int(self.registers[self.get_reg_mem_index(argument)])] = value
		elif argument.startswith('@F'):
			self.memory[float(self.registers[self.get_freg_mem_index(argument)])] = value
		elif argument.startswith('#'):
			self.memory[self.get_mem_index(argument)] = value
		elif argument.startswith('R'):
			self.registers[self.get_reg_index(argument)] = int(floor(float(value)))
		elif argument.startswith('F'):
			self.float_registers[self.get_freg_index(argument)] = value
		self.last_value = value

	def mov_args(self, args):
		self.execution_time += 1
		self.setval(args[0], self.getval(args[1]))

	def operate_binary(self, args, operator):
		self.execution_time += 2
		op1=self.getval(args[0])
		op2=self.getval(args[1])
		self.setval(args[0], eval("(" + op1 + ")" + operator + op2))

	def jump(self, args):
		self.execution_time += 3
		self.ip = int(self.getval(args[0]))	 	
			
	def jump_conditional(self, args, condition):
		self.execution_time += 2
		if eval(str(self.last_value) + condition  + '0'):
			self.jump(args)
	
	def pop(self, args):
			self.execution_time += 2
			self.setval(args[0], self.stack.pop())
	
	def push(self, args):
		self.execution_time += 2
		op1=self.getval(args[0])
		self.stack.append(op1)

		
print "Start"
vm = VirtualMachine()
run_file()	
print "Stop"
