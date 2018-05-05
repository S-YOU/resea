name := benchmark-client
objs := main.o
libs :=
requires := discovery

include servers/server.mk
