#!/bin/sh
gcc wm.c -o "bin/cocoa" -lX11
if [ $? -eq 0 ]; then
	mkdir -p /atm
	cp bin/cocoa /atm/cocoa
fi