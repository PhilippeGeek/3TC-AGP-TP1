
all : unmultiplexer 
unmultiplexer: unmultiplexer.o
	gcc -o unmultiplexer unmultiplexer.o
