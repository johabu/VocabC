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

#define EXIT 0

char *query_strings[3][10] = {
	{"Correct!","Wrong!","Correct was","word","of","known","progress","You have known","of the words"},
	{"Richtig!","Falsch!","Korrekt ist","Wort","von","Bereits gewusst","Fortschritt","Du wusstest","der Wörter"},
	{"¡Correcto!","¡Falso!","Correcto es","Palabra","de","Ya sabiendo","Progreso","Sabías","de las palabras"},
};

char *status_strings[3][13] = {
	{"version","Developed by Johabu","Status","Vocabulary file","Random order set","Case sensitivity set","Direction","both","Comments will be displayed","pairs will be asked","Number of queries","Average percentage","Best percentage"},
	{"Version","Entwickelt von Johabu","Status","Vokabeldatei","Zufällige Wortreihenfolge","Groß-/Kleinschreibung wird unterschieden","Abfragerichtung","beide","Kommentare werden angezeigt","Wortpaare werden abgefragt","Number of queries","Average percentage","Best percentage"},
	{"versión","Desarrollado de Johabu","Estado","Archivo de vocablos","Orden casual","Sensible al uso de mayúsculas y minúsculas","Dirección","ambos","comentarios se muestran","pares","Number of queries","Average percentage","Best percentage"},
};

char *program_strings[3][1] = {
	{"Press 'Enter' to exit"},
	{"Drücke 'Enter' zum Beenden"},
	{"Oprima la tecla 'Enter'"},
};

#endif
