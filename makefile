SRV=./server
CLN=./client
SRVOBJ=$(SRV)/obj
CLNOBJ=$(CLN)/obj

all: $(SRVOBJ) $(CLNOBJ) srv cln

# Creating folders
$(SRVOBJ):
	mkdir -p $(SRVOBJ)

$(CLNOBJ):
	mkdir -p $(CLNOBJ)

# Dynamik linking
srv: $(SRVOBJ)/server.o $(SRVOBJ)/server_display.o $(SRVOBJ)/server_objects.o
	gcc -g -Wall -pedantic $^ -o $@ -lcurses -lpthread -lrt -lm

cln: $(CLNOBJ)/client.o $(CLNOBJ)/client_display.o
	gcc -g -Wall -pedantic $^ -o $@ -lcurses -lpthread -lrt

# server
$(SRVOBJ)/server.o: $(SRV)/server.c $(SRV)/server_display.h $(SRV)/server_common.h $(SRV)/server_objects.h
	gcc -g -c -Wall -pedantic $< -o $@

$(SRVOBJ)/server_display.o: $(SRV)/server_display.c $(SRV)/server_display.h $(SRV)/server_objects.h $(SRV)/server_common.h
	gcc -g -c -Wall -pedantic $< -o $@

$(SRVOBJ)/server_objects.o: $(SRV)/server_objects.c $(SRV)/server_objects.h $(SRV)/server_common.h
	gcc -g -c -Wall $< -o $@

# client
$(CLNOBJ)/client.o: $(CLN)/client.c $(CLN)/client_common.h $(CLN)/client_display.h
	gcc -g -c -Wall -pedantic $< -o $@

$(CLNOBJ)/client_display.o:  $(CLN)/client_display.c $(CLN)/client_display.h
	gcc -g -c -Wall -pedantic $< -o $@

.PHONY: clean

clean:
	-rm srv $(SRVOBJ)/*.o
	-rm cln $(CLNOBJ)/*.o
