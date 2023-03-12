#!/bin/sh
g++ -c main.cpp Facets.cpp TaskManager.cpp UserInterface.cpp BasicStructs.cpp Utility.cpp RawSTL.cpp Project.cpp Errors.cpp Contours.cpp
g++ main.o Facets.o TaskManager.o UserInterface.o BasicStructs.o Utility.o RawSTL.o Project.o Errors.o Contours.o