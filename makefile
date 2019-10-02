all: blueper timeout

blueper: blueper.c
	gcc blueper.c -o blueper -lbluetooth

timeout: timeout.c
	gcc timeout.c -o timeout

install: 
	cp blueper /bin
	cp timeout /bin

clean:
	rm blueper timeout

