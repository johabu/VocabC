CC = gcc
CFLAGS = -W -Wall -O2 -g3
BINDIR = /usr/local/bin
NAME = VocabC

localinstall: 
	mkdir -p ~/.config/vocabc
	$(CC) $(CFLAGS) -o $(NAME) vocabc.c vocabc_lang.c
	./$(NAME) -i
install: 
	mkdir -p ~/.config/vocabc
	$(CC) $(CFLAGS) -o $(NAME) vocabc.c vocabc_lang.c
	sudo cp $(NAME) $(BINDIR)
	$(NAME) -i
clean:
	rm $(NAME)

uninstall:
	rm $(BINDIR)/$(NAME)
