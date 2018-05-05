name := benchmark-server
objs := main.o
libs :=
requires := discovery

include servers/server.mk
