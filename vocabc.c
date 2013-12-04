#include <stdlib.h>

#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include "vocabc_lang.h"

#define MAX_WORDS 5
#define MAX_TRIES 2
#define MAX_LENGTH 256
#define VERSION "2.5"
//for getopt
#define no_argument 0
#define required_argument 1 
#define optional_argument 2

//functions
int init(void);
int Error(int);
void Print_help(void);
int Get_LANG (void);
void Get_conf_dir (void);
struct Options Read_user_options (int, char**, struct Options);
struct Options Read_user_defaults (char*, struct Options);
struct Statistics Read_file_stats(struct Statistics, struct Options);
void Print_general_status(struct Options, struct Statistics, unsigned int);
struct Statistics Print_statistics(struct Statistics, unsigned int, unsigned int);
void Write_file_stats(struct Statistics, struct Options);
unsigned int Main_query(unsigned int, struct Options, unsigned int, unsigned int*);

//gobal variables
FILE *vocabfile;
FILE *sourcefile;
FILE *source_temp;
FILE *config;
int error_line;
int lang;
char conf_dir[100];

struct Options {
	char fvalue[96];// NULL
	char dvalue[6];	// "1"
	char nvalue[6];	// "all"
	int rvalue;	// 0
	int svalue;	// 0
	int cvalue;	// 1
	int xvalue;	// 0
};

struct Statistics {
	unsigned int stat_line_num;
	unsigned int query_num;
	float best_percentage;
	float average_percentage;
};

/********************main function********************/
int main(int argc, char **argv) {
	/*****VARIABLES*****/
	//structure with options set by user
	struct Options User_settings = {
		.fvalue = "NULL",
		.dvalue = "1",
		.nvalue = "all",
		.rvalue = 0,
		.svalue = 0,
		.cvalue = 1,
		.xvalue = 0
	};
	struct Statistics File_statistics = {
		.stat_line_num = 0,
		.query_num = 0,
		.best_percentage = 0,
		.average_percentage = 0,
	};
	char buffer[MAX_LENGTH];
	char line[MAX_LENGTH];
	unsigned int pairs = 0; // pair = 1, word = 0;
	unsigned int i = 0;
	int index_a, index_b, temp_line; //is_giv, 
	unsigned int right = 0;// correct = 0, tries = 0;
	unsigned int lines = 0;
	/*****END OF VARIABLES*****/

	//get LANG variable
	lang = Get_LANG();
	//Program needs option -f
	if (argc < 2) {
		Error(0);
	}
	//Should the init-function be executed?
	if (strcmp(argv[1],"-i") == 0) {
		init();
	}
	//locate config file
	Get_conf_dir();
	// check which options are set by the user
	User_settings = Read_user_options(argc, argv, User_settings);
	if (User_settings.xvalue != 1) {
		//Read the default configuration of the user
	        User_settings = Read_user_defaults(conf_dir, User_settings);
		// check which options are set by the user
	        User_settings = Read_user_options(argc, argv, User_settings);
	}
	if (system("clear") == -1) {
                printf("%s\n",errors[lang][6]);
        }
	//Open vocabulary file
	sourcefile = fopen(User_settings.fvalue,"r");
	vocabfile = fopen("vocab.tmp","w");
	if (NULL == vocabfile || NULL == sourcefile) {
		Error(1);
	}
	//copy lines from source to temporary file
	lines = 0;
	while((fgets(buffer, sizeof(buffer), sourcefile)) != NULL) {
		lines++;
		if (buffer[0] != '#') {
			if (strchr(buffer,'=') == NULL) {
				error_line = lines;
				Error(8);
			}
			fputs(buffer, vocabfile);
		}
	}
	
	//read statistics of vocabulary file
	File_statistics = Read_file_stats(File_statistics, User_settings);
	//reopen temporary file to read lines
	if (freopen("vocab.tmp","r",vocabfile) == NULL) {
		Error(7);
	}
	
	//Count lines
	lines = 0;
	while ((fgets(line,MAX_LENGTH,vocabfile)) != NULL) {
		lines++;
	}
	unsigned int rand_lines[lines-1];
	//fill array with line numbers
	for(i = 0; i < lines; i++) {
		rand_lines[i] = i+1;
	}
	srand(time(NULL));
	//if -r is set, shuffle array
	if (User_settings.rvalue == 1) {
		for (i = 0; i < 10000; i++) {
			index_a = rand() % lines;
			index_b = rand() % lines;
			temp_line = rand_lines[index_a];
			rand_lines[index_a] = rand_lines[index_b];
			rand_lines[index_b] = temp_line;
		}
	}
	pairs = lines;
	if (strcmp(User_settings.nvalue, "all") != 0) {
		pairs = atoi(User_settings.nvalue);
		if (pairs > lines) {
			//Error(2);
			pairs = lines;
		}
	}
	//status information  are displayed
	Print_general_status(User_settings, File_statistics, pairs);
	
	//main loop with query
	right = Main_query(pairs, User_settings, lines, rand_lines);
	
	
	//Print new statistics about this file
	File_statistics = Print_statistics(File_statistics, pairs, right);
	//Write new statictics in file using temporary file and renaming it later
	Write_file_stats(File_statistics, User_settings);
	//End of program, remove temporary file
	if (remove("vocab.tmp") < 0) {
		Error(5);
	}
	printf("\n| %s...\n",program_strings[lang][EXIT]);
	getchar();
       	return EXIT_SUCCESS;
}

/********************functions********************/
//Function for displaying an error
int Error(int error_num) {
	if (error_num == 8) {
		printf("| Error %#x - %s %d!\n", error_num, errors[lang][error_num], error_line);
	} else {
		printf("| Error %#x - %s!\n", error_num, errors[lang][error_num]);
	}
	if ((error_num != 0) && (remove("vocab.tmp") < 0)) {
		fclose(vocabfile);
		fclose(sourcefile);
		remove("vocab.tmp");
	}
	Print_help();
	exit(EXIT_FAILURE);
}
//Function for displaying a help text
void Print_help(void) {
	printf("\nVocabC %s\n",VERSION);
	printf("\n%s: VocabC -f <%s> [-h] [-r] [-d1|-d2|-dr] [-n <num>] [-s] [-c] [-x]\n", program_strings[lang][USE],program_strings[lang][VFILE]);
	printf("\n -f, --file\t\t\tSelect vocabulary file\n -h, --help\t\t\t %s\n -r, --random\t\t\t %s\n", program_strings[lang][HELP], program_strings[lang][RAND]);
	printf(" -d1, --direction1\t\t %s\n -d2, --direction2\t\t %s\n -dr, --directionr\t\t %s\n", program_strings[lang][D1], program_strings[lang][D2], program_strings[lang][DR]);
	printf(" -n, --word-number <num>\t\t %s\n", program_strings[lang][NUM]);
	printf(" -s, --case-sensitive\t\t %s\n", program_strings[lang][CASE_S]);
	printf(" -c, --comments\t\t\t %s\n", program_strings[lang][IGN_COMM]);
	printf(" -x, --ignore-defaults\t\t %s\n\n", program_strings[lang][IGN_SET]);
}
//Get the environment variable LANG
int Get_LANG (void) {
	int lang;
	char lang_code[20];
	strncpy(lang_code, getenv("LANG"), 19);
        lang_code[19] = '\0';
        if (strstr(lang_code,"de") != NULL) {
                lang = 1;
        } else if (strstr(lang_code,"es") != NULL) {
                lang = 2;
        } else {
                lang = 0;
        }
	return lang;
}
//Locate configuration directory ($HOME/.config/vocabc/config)
void Get_conf_dir (void) {
	strncpy(conf_dir, getenv("HOME"), 75);
	conf_dir[75] = '\0';
	if (conf_dir == NULL) {
		Error(9);
	}
	strncat(conf_dir,"/.config/vocabc/config",100);
}

//Read the options the user set when starting VocabC
struct Options Read_user_options (int argc, char **argv, struct Options User_options) {
	int c;
	while (1) {
		static struct option long_options[] =
			{
			{"help",     no_argument,      0, 'h'},
			{"random",  no_argument,       0, 'r'},
			{"direction",  required_argument, 0, 'd'},
			{"word-number",  required_argument, 0, 'n'},
			{"comments",  no_argument, 0, 'c'},
			{"case-sensitive",  no_argument, 0, 's'},
			{"ignore-default",  no_argument, 0, 'x'},
			{"file",    required_argument, 0, 'f'},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;
		c = getopt_long (argc, argv, "hrf:d:n:scx",
			long_options, &option_index);
		/* Detect the end of the options. */
		if (c == -1)
			break;
		switch (c) {
			case 0:
				/* If this option set a flag, do nothing else now. */
				if (long_options[option_index].flag != 0)
					break;
				printf ("option %s", long_options[option_index].name);
				if (optarg)
				printf (" with arg %s", optarg);
				printf ("\n");
				break;
			case 'h':
				Print_help();
				exit(EXIT_FAILURE);
				break;
			case 'f':
				strncpy(User_options.fvalue, optarg, 95);
				User_options.fvalue[95] = '\0';
				break;
			case 'r':
				User_options.rvalue = 1;
				break;
			case 'd':
				strncpy(User_options.dvalue, optarg, 5);
				User_options.dvalue[5] = '\0';
				break;
			case 'n':
				strncpy(User_options.nvalue, optarg, 5);
				User_options.nvalue[5] = '\0';
				break;
			case 's':
				User_options.svalue = 1;
				break;
			case 'c':
				User_options.cvalue = 0;
				break;
			case 'x':
				User_options.xvalue = 1;
				break;
			case '?':
				/* getopt_long already printed an error message. */
				/*if (optopt == 'f' || optopt == 'd') {
					fprintf (stderr, "| Error 0xa - Option -%c requires an argument.\n| %s...\n", optopt, program_strings[lang][EXIT]);
					getchar();
				} else if (isprint (optopt)) {
					fprintf (stderr, "| Error 0xb - Unknown option `-%c'.\n| %s...\n", optopt, program_strings[lang][EXIT]);
                                        getchar();
                                } else {
                                        fprintf (stderr,"| Error 0xc - Unknown option character `\\x%x'.\n| %s...\n",optopt, program_strings[lang][EXIT]);
                                        getchar();
                                }*/
				break;
			default:
				abort ();
		}
	}
	return User_options;
}

//Read configuration file and store settings in structure
struct Options Read_user_defaults (char *conf_dir, struct Options User_settings) {
	char settings[10][MAX_LENGTH];
	char setting[MAX_LENGTH];
	unsigned int i,j;
	char *ptr;
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
				User_settings.rvalue = 1;
			}
		}
	}
	//which value has 'direction' in the config file?
	for (i = 0; i < 9; i++) {
		if (strncmp(settings[i],"direction = ",12) == 0) {
			if (settings[i][13] == '1') {
				strcpy(User_settings.dvalue,"1");
			} else if (settings[i][13] == '2') {
				strcpy(User_settings.dvalue,"2");
			} else if (settings[i][13] == 'r') {
				strcpy(User_settings.dvalue,"r");
			}
		}
	}
	//How many words should be asked?
	for (i = 0; i < 9; i++) {
		if (strncmp(settings[i],"pairs = ",8) == 0) {
			if (strstr(settings[i],"all") != NULL) {
				strcpy(User_settings.nvalue, "all");
			} else {
				ptr = strtok(settings[i], "=");
				ptr = strtok(NULL, "=");
				strcpy(User_settings.nvalue, ptr);
				while (User_settings.nvalue[0] == ' ') {
					for (j = 0; j < strlen(User_settings.nvalue); j++) {
						User_settings.nvalue[j] = User_settings.nvalue[j+1];
					}
				}
				while (User_settings.nvalue[strlen(User_settings.nvalue)-1] == ' ') {
					User_settings.nvalue[strlen(User_settings.nvalue)-1] = '\0';
				}
			}
		}
	}
	//is case sensitivity set to 'true' in the config file?
	for (i = 0; i < 9; i++) {
		if (strncmp(settings[i],"sensitivity = ",14) == 0) {
			if (strstr(settings[i],"true") != NULL) {
				User_settings.svalue = 1;
			}
		}
	}
	//are comments allowed?
	for (i = 0; i < 9; i++) {
		if (strncmp(settings[i],"comments = ",11) == 0) {
			if (strstr(settings[i],"true") != NULL) {
				User_settings.cvalue = 1;
			}
		}
	}
	return User_settings;
}

struct Statistics Read_file_stats(struct Statistics File_statistics, struct Options User_settings) {
	char *ptr;
	char buffer[MAX_LENGTH];
	int i = 0;
	File_statistics.stat_line_num = 0, File_statistics.query_num = 0;
	char stat_line[MAX_LENGTH];
	char stats[5][MAX_LENGTH];
	File_statistics.best_percentage = 0, File_statistics.average_percentage = 0;
	fseek(sourcefile, 0L, SEEK_SET);
	while (fgets(buffer, MAX_LENGTH, sourcefile) != NULL ) {
		if (strstr(buffer,"#STATS#") != 0 ) {
			strcpy(stat_line,buffer);
			File_statistics.stat_line_num = i;
		}
		i++;
	}
	ptr = strtok(stat_line, "#");
	i = 0;
	while(ptr != NULL) {
		strcpy(stats[i], ptr);
		ptr = strtok(NULL, "#");
		i++;
	}
	File_statistics.query_num = atoi(stats[1]);
	File_statistics.average_percentage = strtof(stats[2], NULL) * 100;
	File_statistics.best_percentage = strtof(stats[3], NULL) * 100;
	//first query? -> adding statistic-string
	if (strcmp(stats[0], "STATS") != 0) {
		if (freopen(User_settings.fvalue,"a+",sourcefile) == NULL )
			Error(1);
		fseek(sourcefile, -1L, SEEK_END);
		if (fgetc(sourcefile) == '\n') {
			fputs("#STATS#0#0#0#\n", sourcefile);
		} else {
			fputs("\n#STATS#0#0#0#\n", sourcefile);
		}
		i = 0;
        	fseek(sourcefile, 0L, SEEK_SET);
        	while (fgets(buffer, MAX_LENGTH, sourcefile) != NULL ) {
        	        if (strstr(buffer,"#STATS#") != 0 ) {
        	                strcpy(stat_line,buffer);
        	                File_statistics.stat_line_num = i;
        	        }
        	        i++;
        	}
	}
	return File_statistics;
}

void Write_file_stats(struct Statistics File_statistics, struct Options User_settings) {
	char buffer[MAX_LENGTH];
	char source_temp_str[100];
	strncpy(source_temp_str, User_settings.fvalue, 100);
	strncat(source_temp_str, ".tmp", 10);
	source_temp = fopen(source_temp_str, "w");
	fseek(sourcefile,0L,SEEK_SET);
	unsigned int i = 0;
	while(((fgets(buffer, sizeof(buffer), sourcefile)) != NULL) && (i < File_statistics.stat_line_num))  {
		fputs(buffer, source_temp);
		i++;
	}
	File_statistics.query_num++;
	fprintf(source_temp, "#STATS#%d#%.3f#%.3f#\n",File_statistics.query_num, (File_statistics.average_percentage / 100), (File_statistics.best_percentage / 100));
	fclose(source_temp);
	fclose(vocabfile);
	fclose(sourcefile);
	rename(source_temp_str, User_settings.fvalue);
}

void Print_general_status(struct Options User_settings, struct Statistics File_statistics, unsigned int pairs) {
	printf("------------------------------------------------------------------------\n");
	printf("| VocabC - %s %s   %s <https://github.com/johabu>\n|\n",status_strings[lang][VERS],VERSION,status_strings[lang][DEVELOP]);
	printf("| %s:\n| %s: %s\n|",status_strings[lang][STATUS],status_strings[lang][VOCFILE],User_settings.fvalue);
	if (User_settings.rvalue == 1) { printf(" %s\n|",status_strings[lang][RANDOM]); }
	if (User_settings.svalue == 1) { printf(" %s\n|",status_strings[lang][CASE]); }
	printf(" %s: ",status_strings[lang][DIRECTION]);
	if (strcmp(User_settings.dvalue,"r") == 0) {
		printf("%s\n|",status_strings[lang][BOTH]);
	} else {
		printf("%s\n|",User_settings.dvalue);
	}
	if (User_settings.cvalue == 1) { printf(" %s\n|",status_strings[lang][COMMENT]); }
	printf(" %d %s\n",pairs,status_strings[lang][PAIRS]);
	printf("|\n| %s: %d\n",status_strings[lang][QUERY_NUM], File_statistics.query_num);
	printf("| %s: %g%%\n| %s: %g%%\n", status_strings[lang][AV_PER], File_statistics.average_percentage, status_strings[lang][TOP_PER], File_statistics.best_percentage);
	printf("------------------------------------------------------------------------\n\n");
}

unsigned int Main_query(unsigned int pairs, struct Options User_settings, unsigned int lines, unsigned int rand_lines[lines-1]) {
	//variables for query and output
	char source_str[MAX_LENGTH], input_str[MAX_LENGTH];
	char comm_str[MAX_LENGTH], lang1_comm[MAX_LENGTH], lang2_comm[MAX_LENGTH];
	char lang1_word[MAX_LENGTH], lang1_str[MAX_LENGTH], lang2_str[MAX_LENGTH];
	char temp[MAX_LENGTH], temp_word[MAX_LENGTH], lang2_temp_str[MAX_LENGTH];
	char lang2_word[MAX_WORDS][MAX_LENGTH];
	char *ptr;
	//different tokens for dividing strings
	char lang_token[] = "=\n", word_token[] = ",", comm_token[] = "#";
	unsigned int pair = 1, word = 0;
	//loop variables
	unsigned int i, j, l, k = 0;
	int is_giv;
	unsigned int right = 0, correct = 0, tries = 0;
	char direction[2] = "1";
	float percent;
	//variables for bar
	float bar_num, bar_loop;
	
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
		if (strcmp(User_settings.dvalue,"r") != 0) {
				strcpy(direction,User_settings.dvalue);
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
					if (User_settings.svalue == 1) {
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
				if (User_settings.svalue == 1) {
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
				if (strcmp(direction,"2") != 0) {
					printf("\t%s: \"%s\"",query_strings[lang][ALL_CORR] ,lang2_str);
				}
				right++;
			} else {
				printf("| %s",query_strings[lang][WRONG]);
			}
		}
		//too many tries? -> next word
		if (correct != 1) {
			if (strcmp(direction,"2") != 0) {
				printf("\t%s: \"%s\"",query_strings[lang][CORR_WAS],lang2_str);
			} else {
				printf("\t%s: \"%s\"",query_strings[lang][CORR_WAS],lang2_word[k]);
			}
		}
		printf("\n-------------------------------------------------------------------------\n");
		pair++;
	}
	return right;
}


struct Statistics Print_statistics(struct Statistics File_statistics, unsigned int pairs, unsigned int right) {
	float percent;
	percent = (float) right / (float) pairs * 100;
	printf("\n| %s %g%% (%d/%d) %s.\n",query_strings[lang][ANALYSIS1],percent,right,pairs,query_strings[lang][ANALYSIS2]);

	if (percent > File_statistics.best_percentage) {
		printf("| %s: %.1f%%\t%s: %.1f%%\n",status_strings[lang][NEW_BEST], percent, status_strings[lang][OLD], File_statistics.best_percentage);
		File_statistics.best_percentage = percent;
	}
	printf("| %s: %.1f%%\n", status_strings[lang][OLD_AV], File_statistics.average_percentage);
	File_statistics.average_percentage = ((File_statistics.average_percentage * File_statistics.query_num) + percent) / (File_statistics.query_num + 1);
	printf("| %s: %.1f%%\n", status_strings[lang][NEW_AV], File_statistics.average_percentage);
	return File_statistics;
}

//Init function; exucute when installing VocabC; generating configuartion file for default user settings
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
		printf("Configfile already exists. Do you want to overwrite it? (yes/NO)\n");
		if(fgets(overwrite, 5, stdin) == NULL)
			strcpy(overwrite, "NO");
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