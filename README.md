README for VocabC
==================
Developed by Johabu <http://johabu.spheniscida.de>; <http://github.com/johabu>
This file is part of VocabC.
VocabC is free software; you can modify it or redistribute it 
under the terms of the GNU General Public License as published by
the Free Software Foundation <http://www.fsf.org>, either version 3,
or (at your option) any later version.
See <http://www.gnu.org/licenses> for the license, if you haven't received a copy of it (GNU\_GPL.txt).
VocabC is distributed without any warranty!
Platforms
----------
Current version: 2.5

version 2.x works on GNU/Linux systems with Kernel 3.x and gcc 4.7 (tested on Ubuntu 13.04; x86-64)

Installing
-----------

First download the file (VocabC.zip or VocabC.tar.gz) and extract the files
or clone the repository.

1.	Compiling:
	
	Type the following commands in a shell:

		cd /PATH/TO/VocabC/
		make install 
	(you must have root rights)
	For a local install in one directory use
	`make localinstall`

2.	Configuration:
	When running `make install` the program creates an config file
	located in `HOME/.config/vocabc/` . If not, copy "conf.txt" in this
	directory and rename it into "config".
	In the config file you can save your default settings, e.g. random
	order by default. Look at this file to see which settings you can save.

Use
----

VocabC is a simple vocabulary trainer written in C. 
One point is that you can use it for learning words or phrases
of many different languages.

The output of VocabC is displayed in the language stored in $LANG.
(At the moment 3 languages: English, German and Spanish)

1.	Creating vocabulary file
	To use the program, first create a vocab file.
	*The vocab has to be in this format:*	
	`word1 = word2a, word2b`	
	Example: file name is e.g. "voc_1.txt"

		Personal =personnel,staff	
		#english-german/spanish			
		german   =deutsch,alemán	
		to fit   =caber#in english	

	several meanings possible

	comments with `#` (must be the first character in the line)

	many languages possible (ISO 8859-15)

	spaces possible

	comments on only one side  possible	

	See also the vocabulary file "example.txt"

2.	Start the program:

	type:
	
		VocabC -f <file>	

	If this doesn't work, try `./VocabC -f <file>` in the directory
	in which you installed the program.
	Check also your PATH variable.
	
	Optional arguments:

	`-h`	Show help

	`-r`	Random order of words

	`-d1`	Program asks the first word

	`-d2`	Program asks the second word

	`-dr`	Program sets direction randomly

	`-n <num>`	ask <num> words

	`-s`	case-sensitivity

	`-c`	don't show comments

	`-x`	ignore default settings stored in config file
	
	If you stored settings in a configuration file, you don't need to set 
	optional arguments.
	If you set some, settings from the configuration file are overwritten.

3.	Behaviour of the program:

	At first the program prints some information like the number of words 
	which are going to be asked.
	The program asks you for the word on the right side in the file.
	(when you set -d2 (-dr) , it asks you (maybe) for the left one)
	You have to type the word from the right side.
	Be careful with the spelling! You have two tries, e.g. if you misspelled
	a word. Before asking for a word the program gives you information about
	your progress and percentage of the words you knew.
	In the end it shows you statistics about your knowledge, which are stored
	in each vocabulary file as well.

Errors
-------

Following error codes may occur:

*	0	You didn't specify a vocabulary file using the -f option.
		You have to specify a file name using VocabC -f <file>.
*	0x1	The file you specified could not be opened. 
		Check if file exists and check permissions.
*	0x3	Error while reading the vocabulary file.
		Check if word pairs are in correct syntax and if file still
		exists.
*	0x4	The error occured in reading your input. Maybe you pressed
		a key combination like Crtl-D.
*	0x5	The program creats a temporary file. Program was unable to
		delete this file.
*	0x6	The program couldn't clear the screen of your console.
*	0x7	Internal error.
*	0x8	At the given line in your vocabulary file there isn't a
		readable word pair.
*	0x9	The program was unable to get the $HOME variable, thus
		your configuration file couldn't be located.
*	0xa	The given command-line option requieres an argument.
*	0xc	This error occurs if you entered an option character the
		program can't deal with.

