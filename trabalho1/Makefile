CFLAGS=-Wall
 
all: servidor_info_tcp.c servidor_info_udp.c cliente_info_tcp.c cliente_info_udp.c
	$(CC) -o servidor_info_tcp servidor_info_tcp.c
	$(CC) -o servidor_info_udp servidor_info_udp.c
	$(CC) -o cliente_info_tcp cliente_info_tcp.c
	$(CC) -o cliente_info_udp cliente_info_udp.c
  
clean:
	rm -f servidor_info_tcp servidor_info_udp cliente_info_tcp cliente_info_udp

