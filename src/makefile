all: ./main.c ./serial.c ./show.c ./sqlite.c  
	arm-linux-gcc $^ -I /home/linux/sqlite3/install/include -L /home/linux/sqlite3/install/lib -o $@ -lpthread -lsqlite3

.PHONY:
clean:
	rm all
