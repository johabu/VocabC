#ifndef VOCABC_LANG_H
#define VOCABC LANG_H
#include <stdlib.h>

#define CORRECT 0
#define WRONG 1
#define CORR_WAS 2
#define WORD 3
#define OF 4
#define KNOWN 5
#define PROGRESS 6
#define ANALYSIS1 7
#define ANALYSIS2 8
#define ALL_CORR 9

#define VERS 0
#define DEVELOP 1
#define STATUS 2
#define VOCFILE 3
#define RANDOM 4
#define CASE 5
#define DIRECTION 6
#define BOTH 7
#define	COMMENT 8
#define PAIRS 9
#define QUERY_NUM 10
#define AV_PER 11
#define TOP_PER 12
#define NEW_BEST 13
#define OLD 14
#define OLD_AV 15
#define NEW_AV 16

#define EXIT 0
#define USE 1
#define VFILE 2
#define OPTARG 3
#define HELP 4
#define RAND 5
#define D1 6
#define D2 7
#define DR 8
#define NUM 9
#define CASE_S 10
#define IGN_COMM 11
#define IGN_SET 12


//strings used during query
char *query_strings[3][10] = {
	{"Correct!","Wrong!","Correct was","word","of","known","progress","You have known","of the words","All correct meanings"},
	{"Richtig!","Falsch!","Korrekt ist","Wort","von","Bereits gewusst","Fortschritt","Du wusstest","der Wörter","Alle korrekten Bedeutungen"},
	{"¡Correcto!","¡Falso!","Correcto es","Palabra","de","Ya sabiendo","Progreso","Sabías","de las palabras","Todos conceptos correctos"},
};
//Status informations
char *status_strings[3][17] = {
	{"version","Developed by Johabu","Status","Vocabulary file","Random order set","Case sensitivity set","Direction","both","Comments will be displayed","pairs will be asked","Number of queries","Average percentage","Best percentage","NEW best percentage","Old","Old average","New average"},
	{"Version","Entwickelt von Johabu","Status","Vokabeldatei","Zufällige Wortreihenfolge","Groß-/Kleinschreibung wird unterschieden","Abfragerichtung","beide","Kommentare werden angezeigt","Wortpaare werden abgefragt","Anzahl von Abfragen","Durchschnittlicher Prozentsatz","Bester Prozentsatz","NEUER bester Prozentsatz","Alter","Alter Durchschnitt","Neuer Durchschnitt"},
	{"versión","Desarrollado de Johabu","Estado","Archivo de vocablos","Orden casual","Sensible al uso de mayúsculas y minúsculas","Dirección","ambos","comentarios se muestran","pares","Numero de interrogaciónes","Porcentaje de promedio","Porcentaje mejor","NUEVO porcentaje mejor","Antiguo","Promedia antiguo","Promedio nuevo"},
};
//General strings
char *program_strings[3][13] = {
	{"Press 'Enter' to exit","Usage","file","Optional Arguments","Show this help","Randomize the order of word pairs","The program asks the word from the left side of the vocabulary file","The program asks one word from the right side of the vocabulary file","The program asks randomly from the left and the right side","Ask only <num> words","Case sensitive","Don't display comments","Ignore settings stored in configuration file (~/.config/vocabc/config)"},
	{"Drücke 'Enter' zum Beenden","Usage","file","Optional Arguments","Show this help","Randomize the order of word pairs","The program asks the word from the left side of the vocabulary file","The program asks one word from the right side of the vocabulary file","The program asks randomly from the left and the right side","Ask only <num> words","Case sensitive","Don't display comments","Ignore settings stored in configuration file (~/.config/vocabc/config)"},
	{"Oprima la tecla 'Enter'","Use","file","Optional Arguments","Show this help","Randomize the order of word pairs","The program asks the word from the left side of the vocabulary file","The program asks one word from the right side of the vocabulary file","The program asks randomly from the left and the right side","Ask only <num> words","Case sensitive","Don't display comments","Ignore settings stored in configuration file (~/.config/vocabc/config)"},
};

//Errors which will be displayed
char *errors[3][15] = {
	{"VocabC requires argument -f <file>","Error in opening vocabulary file","too high argument of option -n","Error in reading vocabulary file","Error in input","Error in deleting temporary file","Unable to clear screen","Internal Error","Vocabfile doesn't contain a word pair in line","Unable to read $HOME","Err10","Err11","Err12"},
	{"VocabC benötigt das Argument -f <Dateiname>","Fehler beim Öffnen der Vokabeldatei","Argument bei -n zu hoch","Fehler beim Einlesen der Vokabeldatei","Fehler in der Eingabe","Fehler beim Löschen der temporären Datei","Unfähig den Bildschirm zu löschen","Interner Fehler","Die Vokabeldatei enthält kein Wortpaar in Zeile","Unfähig $HOME einzulesen","Err10","Err11","Err12"},
	{"VocabC requires argument -f <file>","Error in opening vocabulary file","too high argument of option -n","Error in reading vocabulary file","Error in input","Error in deleting temporary file","Unable to clear screen","Internal Error","Vocabfile doesn't contain a word pair in line","Unable to read $HOME","Err10","Err11","Err12"},
};

#endif
