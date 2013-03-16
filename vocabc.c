#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "vocabc_lang.h"

#define MAX_WORDS 5
#define MAX_TRIES 2
#define MAX_LENGTH 256
#define VERSION "2.2"


FILE *vocabfile;
FILE *sourcefile;
FILE *config;
int glo_var;
int lang;

char *errors[] = {"VocabC requires argument -f <file>","Error in opening vocabulary file","too high argument of option -n","Error in reading vocabulary file",
			"Error in input","Error in deleting temporary file","Unable to clear screen","Internal Error","Err8","Unable to read $HOME",
			"Err10","Err11","Err12"};
int init(void);

int Error(int error) {
	if (error == 8) {
		printf("| Error %#x - Vocabfile doesn't contain a word pair at line %d!\n",error,glo_var);
	} else {
		printf("| Error %#x - %s!\n",error,errors[error]);
	}
	if ((error != 0) && (remove("vocab.tmp") < 0)) {
		fclose(vocabfile);
		remove("vocab.tmp");
	}
	printf("| %s...\n",program_strings[lang][EXIT]);
	getchar();
	exit(EXIT_FAILURE);
}


int main(int argc, char **argv) {
	/*****VARIABLES*****/
	//variables for getopt
	char *fvalue = NULL, *dvalue = "1", *nvalue = "all";
	int rvalue = 0, svalue = 0, ivalue = 0, cvalue = 1;
	int CHAR;
	opterr = 0;
	//variables for query and output
	char buffer[MAX_LENGTH], source_str[MAX_LENGTH], input_str[MAX_LENGTH];
	char settings[10][MAX_LENGTH];
	char comm_str[MAX_LENGTH], lang1_comm[MAX_LENGTH], lang2_comm[MAX_LENGTH];
	char lang1_word[MAX_LENGTH], lang1_str[MAX_LENGTH], lang2_str[MAX_LENGTH];
	char line[MAX_LENGTH], temp[MAX_LENGTH], temp_word[MAX_LENGTH], lang2_temp_str[MAX_LENGTH];
	char lang2_word[MAX_WORDS][MAX_LENGTH];
	char *ptr;
	char setting[MAX_LENGTH];
	//different tokens for dividing strings
	char lang_token[] = "=\n", word_token[] = ",", comm_token[] = "#";
	unsigned int pairs = 0, pair = 1, lines = 0, word = 0;
	//loop variables
	unsigned int i, j, l, k = 0;
	int is_giv, index_a, index_b, temp_rand;
	unsigned int right = 0, correct = 0, tries = 0;
	char direction[2] = "1";
	float percent;
	//variables for bar
	float bar_num, bar_loop;
	/*****END OF VARIABLES*****/
	if (argc < 2) {
		Error(0);
	}
	//Should the init-function be executed?
	if (strcmp(argv[1],"-i") == 0) {
		init();
	} else {
		if (system("clear") == -1) {
			Error(6);
		}
	}
	//get LANG variable
	char *lang_code = getenv("LANG");
	if (strstr(lang_code,"de") != NULL) {
		lang = 1;
	} else {
		lang = 0;
	}
	//get the HOME variable to locate the config file
	char *conf_dir;
	conf_dir = getenv("HOME");
	if (conf_dir == NULL) {
		Error(9);
	}
	strncat(conf_dir,"/.config/vocabc/config",100);
	//open config file
	config = fopen(conf_dir,"r");
	if (config == NULL) {
		printf("\nUnable to open configuration file. Try VocabC -i to fix the problem.\n");
	} else {
		for (i = 0; i < 10; i++) {
			strcpy(settings[i],"NULL");
		}
		i = 0;
		j = 0;
		//read config file and store settings in strings
		while ((fgets(setting, sizeof(setting), config) != NULL) && (i < 10)) {
			if (setting[0] != '#') {
				strcpy(settings[i],setting);
				j++;
			}
			settings[i][strlen(settings[i])-1] = '\0';
			i = j;
		}
	}
	//is 'random = true' set in the config file?
	for (i = 0; i < 9; i++) {
		if (strncmp(settings[i],"random = ",9) == 0) {
			if (strstr(settings[i],"true") != NULL) {
				rvalue = 1;
			}
		}
	}
	//which value has 'direction' in the config file?
	for (i = 0; i < 9; i++) {
		if (strncmp(settings[i],"direction = ",12) == 0) {
			if (settings[i][13] == '1') {
				strcpy(dvalue,"1");
			} else if (settings[i][13] == '2') {
				strcpy(dvalue,"2");
			} else if (settings[i][13] == 'r') {
				strcpy(dvalue,"r");
			}
		}
	}
	//How many words should be asked?
	for (i = 0; i < 9; i++) {
		if (strncmp(settings[i],"pairs = ",8) == 0) {
			if (strstr(settings[i],"all") != NULL) {
				strcpy(nvalue,"all");
			} else {
				ptr = strtok(settings[i], "=");
				ptr = strtok(NULL, "=");
				strcpy(nvalue,ptr);
				while (nvalue[0] == ' ') {
					for (j = 0; j < strlen(nvalue); j++) {
						nvalue[j] = nvalue[j+1];
					}
				}
				while (nvalue[strlen(nvalue)-1] == ' ') {
					nvalue[strlen(nvalue)-1] = '\0';
				}
			}
		}
	}
	//is case sensitivity set to 'true' in the config file?
	for (i = 0; i < 9; i++) {
		if (strncmp(settings[i],"sensitivity = ",14) == 0) {
			if (strstr(settings[i],"true") != NULL) {
				svalue = 1;
			}
		}
	}
	//are comments allowed?
	for (i = 0; i < 9; i++) {
		if (strncmp(settings[i],"comments = ",11) == 0) {
			if (strstr(settings[i],"true") != NULL) {
				cvalue = 1;
			}
		}
	}

	while ((CHAR = getopt (argc, argv, "hrf:d:n:sci")) != -1) {
		switch (CHAR) {
          		case 'h':
				printf("\nVocabC %s\n",VERSION);
            			printf("\nUse:\tVocabC -f <file>\n");
				printf("\nOptional arguments:\n-h\tShow this help\n-r\tRandomize the order of the words\n");
				printf("-d1\tThe program asks the first word\n-d2\tThe program asks the second word\n-dr\tthe program asks randomly\n");
				printf("-n <num>\tAsk only <num> words\n");
				printf("-s\tCase sensitive\n");
				printf("-c\tdon't display comments\n");
				return EXIT_FAILURE;	
            		case 'f':
				fvalue = optarg;
             			break;
			case 'r':
				rvalue = 1;
				break;
			case 'd':
				dvalue = optarg;
				break;
			case 'n':
				nvalue = optarg;
				break;
			case 's':
				svalue = 1;
				break;
			case 'c':
				cvalue = 0;
				break;
			case 'i':
				ivalue = 1;
				break;
           		case '?':
             			if (optopt == 'f' || optopt == 'd') {
               				fprintf (stderr, "Error 0xa - Option -%c requires an argument.\n%s...\n", optopt, program_strings[lang][EXIT]);
					getchar();
             			} else if (isprint (optopt)) {
               				fprintf (stderr, "Error 0xb - Unknown option `-%c'.\n%s...\n", optopt, program_strings[lang][EXIT]);
					getchar();
             			} else {
               				fprintf (stderr,"Error 0xc - Unknown option character `\\x%x'.\n%s...\n",optopt, program_strings[lang][EXIT]);
             				getchar();
				}
           		default:
             			return EXIT_FAILURE;
           	}
	}
	// if -i, execute init()
	if (ivalue == 1) {
		init();
	}
	//Open vocabulary file
	sourcefile = fopen(fvalue,"r");
	vocabfile = fopen("vocab.tmp","w");
	if (NULL == vocabfile || NULL == sourcefile) {
		Error(1);
	}
	lines = 0;
	//copy lines from source to temporary file
	while((fgets(buffer, sizeof(buffer), sourcefile)) != NULL) {
		lines++;
		if (buffer[0] != '#') {
			if (strchr(buffer,'=') == NULL) {
				glo_var = lines;
				Error(8);
			}
			fputs(buffer, vocabfile);
		}
	}
	//reopen temporary file to read lines
	if (freopen("vocab.tmp","r",vocabfile) == NULL) {
		Error(7);
	}
	//Count lines
	lines = 0;
	while ((fgets(line,MAX_LENGTH,vocabfile)) != NULL) {
		lines++;
	}
	srand(time(NULL));
	unsigned int rand_lines[lines-1];
	//fill array with line numbers
	for(i = 0; i < lines; i++) {
		rand_lines[i] = i+1;
	}
	//if -r is set, shuffle array
	if (rvalue == 1) {
		for (i = 0; i < 5000; i++) {
			index_a = rand() % lines;
			index_b = rand() % lines;
			temp_rand = rand_lines[index_a];
			rand_lines[index_a] = rand_lines[index_b];
			rand_lines[index_b] = temp_rand;
		}
	}
	//How many words should be asked?
		//ask all pairs
	if (strcmp(nvalue, "all") == 0) {
		pairs = lines;
	} else {
		//ask only -n <num> words
		pairs = atoi(nvalue);
	}
	if (pairs > lines) {
		Error(2);
	}
	//First output of the program - options which set are displayed
	printf("-----------------------------------------------------------------------\n");
	printf("| VocabC - %s %s   %s <https://github.com/johabu>\n|\n",status_strings[lang][VERS],VERSION,status_strings[lang][DEVELOP]);
	printf("| %s:\n| %s: %s\n|",status_strings[lang][STATUS],status_strings[lang][VOCFILE],fvalue);
	if (rvalue == 1) { printf(" %s\n|",status_strings[lang][RANDOM]); }
	if (svalue == 1) { printf(" %s\n|",status_strings[lang][CASE]); }
	printf(" %s: ",status_strings[lang][DIRECTION]);
	if (strcmp(dvalue,"r") == 0) {
		printf("%s\n|",status_strings[lang][BOTH]);
	} else {
		printf("%s\n|",dvalue);
	}
	if (cvalue == 1) { printf(" %s\n|",status_strings[lang][COMMENT]); }
	printf(" %d %s\n",pairs,status_strings[lang][PAIRS]);
	printf("-----------------------------------------------------------------------\n\n");
	//main loop with query
	for (i = 0; i < pairs; i++) {
		strcpy(comm_str,"NULL");
		strcpy(lang1_comm,"NULL");
		strcpy(lang2_comm,"NULL");
		strcpy(lang2_temp_str,"NULL");
		strcpy(temp_word,"NULL");
		strcpy(lang1_word,"NULL");
		for (j = 0; j < MAX_WORDS; j++) {
			strcpy(lang2_word[j],"NULL");
		}
		fseek(vocabfile,0L,SEEK_SET);
		word = 0;
		//read lines until line i
		for (j = 0; j < rand_lines[i]-1; j++) {
			if (fgets(temp, MAX_LENGTH, vocabfile) == NULL) 
				Error(3);
		}
		//read line i
		if (fgets(source_str, MAX_LENGTH, vocabfile) == NULL)
			Error(3);
		//divide into lang1 and lang2
		ptr = strtok(source_str,lang_token);
		strcpy(lang1_str,ptr);
		ptr = strtok(NULL,lang_token);
		strcpy(lang2_str,ptr);
		//divide lang1 into word and comment
		ptr = strtok(lang1_str,comm_token);
		strcpy(lang1_word,ptr);
		while (ptr != NULL) {
			ptr = strtok(NULL,comm_token);
			if (ptr != NULL) {
				strcpy(lang1_comm,ptr);
			}
		}
		//divide lang2 into words and comment
		ptr = strtok(lang2_str,comm_token);
		strcpy(lang2_temp_str,ptr);
		while (ptr != NULL) {
			ptr = strtok(NULL,comm_token);
			if (ptr != NULL) {
				strcpy(lang2_comm,ptr);
			}
		}
		//save words of second language in lang2_word[]
		ptr = strtok(lang2_temp_str,word_token);
		strcpy(lang2_word[0],ptr);
		word = 1;
		while (ptr != NULL) {
			ptr = strtok(NULL,word_token);
			if (ptr != NULL) {
				strcpy(lang2_word[word],ptr);
			}
			word++;
		}
		is_giv = 0;
		//is -dr set?
		if (strcmp(dvalue,"r") != 0) {
				strcpy(direction,dvalue);
		} else {
			if ((rand() % 2) != 0) {
				strcpy(direction,"2");
			} else {
				strcpy(direction,"1");
			}
		}
		//if direction is "2"
		if (strcmp(direction,"2") == 0) {
			//choose one meaning of a word
			k = rand() % MAX_WORDS;
			while (is_giv != 1) {
				k = rand() % MAX_WORDS;
				if (strcmp(lang2_word[k],"NULL") != 0) {
					is_giv = 1;
				}
			}
			//swap lang1_word and lang2_word
			strcpy(temp_word,lang1_word);
			strcpy(lang1_word,lang2_word[k]);
			strcpy(lang2_word[k],temp_word);
		}
		correct = 0;
		tries = 0;
		//query of one word
		while (tries < MAX_TRIES && correct != 1) {
			tries++;
			//remove " " at the end of word for which is asked
			while (lang1_word[strlen(lang1_word)-1] == ' ' || lang1_word[strlen(lang1_word)-1] == '\t') {
				lang1_word[strlen(lang1_word)-1] = '\0';
			}
			//remove " " at the end of all meanings
			for (j = 0; j < MAX_WORDS; j++) {
				while(lang2_word[j][strlen(lang2_word[j])-1] == ' ' || lang2_word[j][strlen(lang2_word[j])-1] == '\t') {
					lang2_word[j][strlen(lang2_word[j])-1] = '\0';
					}
			}
			//remove " " at the beginning of word for which is asked
			while (lang1_word[0] == ' ' || lang1_word[0] == '\t') {
				for (j = 0; j < strlen(lang1_word); j++) {
					lang1_word[j] = lang1_word[j+1];
				}
			}
			//remove " " at the beginning of all meanings
			for (j = 0; j < MAX_WORDS; j++) {
				while (lang2_word[j][0] == ' ' || lang2_word[j][0] == '\t') {
					for (l = 0; l < strlen(lang2_word[j]); l++) {
						lang2_word[j][l] = lang2_word[j][l+1];
					}
				}
			}
			//ask user a word
			if (pair == 1) {
				percent = 0;
			} else {
				percent = (float) right / (float) (pair-1) * 100;
			}
			printf("\n\n");
			if (tries == 1) {
				printf("| %s %d %s %d ", query_strings[lang][WORD], pair, query_strings[lang][OF], pairs);
				printf("|| %s: %g%% || %s: [", query_strings[lang][KNOWN], percent, query_strings[lang][PROGRESS]);
				bar_num = (float) pair / (float) pairs * 10;
				for (bar_loop=1; bar_loop<=10; bar_loop++) {
					(bar_loop<=bar_num) ? printf("=") : printf(" ");
				}
				printf("]\n");
			}
			//is lang1_comm or lang2_comm not "NULL"?
			if ((strcmp(lang1_comm,"NULL") != 0) && (strcmp(direction,"1") == 0)) {
				strcpy(comm_str,lang1_comm);
			}
			if ((strcmp(lang2_comm,"NULL") != 0) && (strcmp(direction,"2") == 0)) {
				strcpy(comm_str,lang2_comm);
			}
			//ask for a word and print the comment (if one exists)
			if (strcmp(comm_str, "NULL") != 0) {
				printf("| %s:  \"%s\"\t#: %s\n| >>>  ",query_strings[lang][WORD],lang1_word,comm_str);
			} else {
				printf("| %s:  \"%s\"\n| >>>  ",query_strings[lang][WORD],lang1_word);
			}
			//user gives answer
			if(fgets(input_str, MAX_LENGTH, stdin) == NULL) {
				Error(4);
			}
			//removing '\n'
			input_str[strlen(input_str)-1] = '\0';
			//remove " " at the end of input
			while (input_str[strlen(input_str)-1] == ' ' || input_str[strlen(input_str)-1] == '\t') {
				input_str[strlen(input_str)-1] = '\0';
			}
			//remove " " at the beginning of input
			while (input_str[0] == ' ' || input_str[0] == '\t') {
				for (j = 0; j < strlen(input_str); j++) {
					input_str[j] = input_str[j+1];
				}
			}

			//is any of the meanings equal to the user`s answer?
			if (strcmp(direction,"2") != 0) {
				for (j = 0; j < 5; j++) {
					if (svalue == 1) {
						if (strcmp(lang2_word[j], input_str) == 0) {
							correct = 1;
						}
					} else {
						if (strcasecmp(lang2_word[j], input_str) == 0) {
							correct = 1;
						}
					}
				}
			} else {
				if (cvalue == 1) {
					if (strcmp(lang2_word[k], input_str) == 0) {
						correct = 1;
					}
				} else {
					if (strcasecmp(lang2_word[k], input_str) == 0) {
						correct = 1;
					}
				}
			}
			
			//display "correct" or "wrong"
			if (correct == 1) {
				printf("| %s",query_strings[lang][CORRECT]);
				right++;
			} else {
				printf("| %s",query_strings[lang][WRONG]);
			}
		}
		//too many tries? -> next word
		if (correct != 1) {
			if (strcmp(direction,"2") != 0) {
				printf("\t%s: \"%s\"",query_strings[lang][CORR_WAS],lang2_word[0]);
			} else {
				printf("\t%s: \"%s\"",query_strings[lang][CORR_WAS],lang2_word[k]);
			}
		}
		printf("\n-------------------------------------------------------------------------\n");
		pair++;
	}
	percent = (float) right / (float) pairs * 100;
	printf("\n| %s %g%% (%d/%d) %s.\n\n",query_strings[lang][ANALYSIS1],percent,right,pairs,query_strings[lang][ANALYSIS2]);
	//End of program, close files, remove temporary file
	fclose(vocabfile);
	fclose(sourcefile);
	if (remove("vocab.tmp") < 0) {
		Error(5);
	}
	printf("| %s...\n",program_strings[lang][EXIT]);
	getchar();
       	return EXIT_SUCCESS;
}

int init(void) {
	FILE *configfile;
	FILE *config_source;
	char buf[1024];
	char *conf_dir;
	char overwrite[5] = "no";
	int new = 0;
	printf("\n---------------------------------------------------------------------");
	printf("\nVocabC init function\n\nGetting HOME variable ... ");
	conf_dir = getenv("HOME");
	if (conf_dir == NULL) {
		printf("Error.\n");
		exit(EXIT_FAILURE);
	}
	printf("OK.\nHome is '%s'\n\n",conf_dir);
	strncat(conf_dir,"/.config/vocabc/config",100);
	printf("Open config file ... ");
	configfile = fopen(conf_dir,"r");
	if (configfile != NULL) {
		printf("Configfile already exists. Do you want to overwrite it? (yes/no)\n");
		fgets(overwrite, 5, stdin);
		size_t len=strlen(overwrite);
		overwrite[len-1]='\0';
		if (strcmp(overwrite,"yes") == 0) {
			printf("Generating new config file. ... ");
			new = 1;
		}
		fclose(configfile);
	} else {
		printf("No config file found. Generating new. ... ");
		new = 1;
	}
	if (new == 1) {
		configfile = fopen(conf_dir,"w+");
		if (configfile == NULL) {
			printf("Error.\n\n");
			exit(EXIT_FAILURE);
		}
		printf("OK.\n\nOpen settings source file ... ");
		config_source = fopen("conf.txt","r+");
		if (config_source == NULL) {
			printf("Error.\n\n");
			exit(EXIT_FAILURE);
		}
		printf("OK.\n\nCopy settings into config file ... ");
		while((fgets(buf, sizeof(buf), config_source)) != NULL) {
			fputs(buf, configfile);
		}
		fclose(config_source);
		fclose(configfile);
		printf("Done.\n\n");
	}
	printf("----------------------------------------------------------------------");
	printf("\nInitialisation completed.\nConfiguration file is '%s'\nChange default settings by editing this file\n",conf_dir);
	printf("----------------------------------------------------------------------\n");
	exit(EXIT_SUCCESS);
}
