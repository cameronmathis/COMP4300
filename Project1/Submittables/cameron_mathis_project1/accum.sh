#1/bin/bash
#----------------
# COMP 4300
# Cameron Mathis
# Project 1
# 9/11/20
# Accumulator Machine
#----------------

case $1 in
	compile)
		g++ accumSim.cpp -std=c++11 -o accumSimulator.o
		g++ -c accumMem.cpp -std=c++11 -o accumMemory.o
		;;
	run)
		./accumSimulator.o
		;;
	clean)
		rm accumSimulator.o accumMemory.o
		;;
	*)
		echo command invalid or missing
		;;
esac
