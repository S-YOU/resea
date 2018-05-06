name := test
objs := main.o
libs :=
requires := logging exit

include servers/server.mk
