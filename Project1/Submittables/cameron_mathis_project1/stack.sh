#1/bin/bash
#----------------
# COMP 4300
# Cameron Mathis
# Project 1
# 9/11/20
# Stack Machine
#----------------

case $1 in
	compile)
		g++ stackSim.cpp -o stackSimulator.o
		g++ -c stackMem.cpp -o stackMemory.o
		;;
	run)
		./stackSimulator.o
		;;
	clean)
		rm stackSimulator.o stackMemory.o
		;;
	*)
		echo command not entered
		;;
esac
