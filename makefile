CC = gcc
CFLAGS = -W -Wall -O2
BINDIR = /usr/local/bin
NAME = VocabC

install:
	mkdir -p ~/.config/vocabc
	rm -f ~/.config/vocabc/config
	$(CC) $(CFLAGS) -o $(NAME) vocabc.c
	sudo cp $(NAME) $(BINDIR)
	$(NAME) -i

init:
	mkdir -p ~/.config/vocabc
	rm -f ~/.config/vocabc/config
	chmod -R a+rw ~/.config/vocabc
	$(NAME) -i

clean:
	rm $(NAME)

uninstall:
	rm $(BINDIR)/$(NAME)
