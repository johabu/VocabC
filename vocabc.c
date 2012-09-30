// #####	VocabC v1.5	##### //

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#define MAX_WORDS 5
#define MAX_TRIES 2
#define MAX_LENGTH 150

char *errors[] = {"VocabC requires argument -f <file>","Error in opening vocabulary file","too high argument of option -n","Error in reading vocabulary file",
			"Error in input"};

int Error(int error) {
	printf("Error - %s!\n",errors[error]);
	exit(EXIT_FAILURE);
}


int main(int argc, char **argv) {
	//variables for getopt
	char *fvalue = NULL;
	char *dvalue = "1";
	char *nvalue = "all";
	int rvalue = 0;
	int cvalue = 0;
	int CHAR;
	opterr = 0;
	//variables for query and output
	FILE *vocabfile;
	char lang1_word[MAX_LENGTH], source_str[MAX_LENGTH], input_str[MAX_LENGTH], line[MAX_LENGTH], temp[MAX_LENGTH], temp_word[MAX_LENGTH];
	char lang2_word[MAX_WORDS][MAX_LENGTH];
	char *ptr;
	char token[] = "=,\n";
	int pairs = 0, pair = 1;
	int lines = 0;
	int word = 0;
	int i,j,k;
	int is_giv;
	int index_a, index_b, temp_rand;
	int right = 0, correct = 0, tries = 0;
	float percent;
	float bar_num;
	float bar_loop;

	if (argc < 2) {
		Error(0);
	}
	while ((CHAR = getopt (argc, argv, "hrf:d:n:c")) != -1) {
		switch (CHAR) {
          		case 'h':
				printf("\nVocabC v1.4\n");
            			printf("\nUse:\tVocabC -f <file>\n");
				printf("\nOptional arguments:\n-h\tShow this help\n-r\tRandomize the order of the words\n");
				printf("-d1\tThe program asks the first word\n-d2\tThe program asks the second word\n-dr\tthe program asks randomly\n");
				printf("-n <num>\tAsk only <num> words\n");
				printf("-c\tCase sensitive\n");
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
			case 'c':
				cvalue = 1;
				break;
           		case '?':
             			if (optopt == 'f' || optopt == 'd') {
               				fprintf (stderr, "Error - Option -%c requires an argument.\n", optopt);
					return EXIT_FAILURE;
             			} else if (isprint (optopt)) {
               				fprintf (stderr, "Error - Unknown option `-%c'.\n", optopt);
					return EXIT_FAILURE;
             			} else {
               				fprintf (stderr,"Error - Unknown option character `\\x%x'.\n",optopt);
             				return EXIT_FAILURE;
				}
           		default:
             			return EXIT_FAILURE;
           	}
	}
	//Clear screen
	system("clear");
	//Open vocabulary file
	vocabfile = fopen(fvalue,"r");
	if (NULL == vocabfile) {
		Error(1);
	}
	//Count lines
	while ((fgets(line,MAX_LENGTH,vocabfile)) != NULL) {
		lines++;
	}
	srand(time(NULL));
	int rand_lines[lines-1];
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
	printf("Word pairs: %d\n",pairs);
	//main loop with query
	for (i = 0; i < pairs; i++) {
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
		//save lang1_word
		ptr = strtok(source_str,token);
		strcpy(lang1_word,ptr);
		//save following words in lang2_word[]
		while (ptr != NULL) {
			ptr = strtok(NULL,token);
			if (ptr != NULL) {
				strcpy(lang2_word[word],ptr);
			}
			word++;
		}
		is_giv = 0;
		// if -d2 is set
		if (strcmp(dvalue,"2") == 0) {
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
		while (tries < MAX_TRIES && correct != 1) {
			tries++;
			//ask user a word
			printf("\n(%d/%d)\t[", pair, pairs);
			bar_num = (float) pair / (float) pairs * 10;
			for (bar_loop=1; bar_loop<=10; bar_loop++) {
				(bar_loop<=bar_num) ? printf("=") : printf(" ");
			}
			printf("]  :?:  %s\n>>>  ",lang1_word);
			//user gives answer
			if(fgets(input_str, MAX_LENGTH, stdin) == NULL) {
				Error(4);
			}
			//removing '\n'
			input_str[strlen(input_str)-1] = '\0';
			//is any of the meanings equal to the user`s answer?
			for (j = 0; j < 5; j++) {
				if (cvalue == 1) {
					if (strcmp(lang2_word[j], input_str) == 0) {
						correct = 1;
					}
				} else {
					if (strcasecmp(lang2_word[j], input_str) == 0) {
						correct = 1;
					}
				}
			}
			//display "correct" or "wrong"
			if (correct == 1) {
				printf("Correct!\n");
				right++;
			} else {
				printf("Wrong!");
			}
		}
		//too many tries? -> next word
		if (correct != 1) {
			printf("\tCorrect was: \"%s\"",lang2_word[0]);
		}
		printf("\n");
		pair++;
	}
	percent = (float) right / (float) pairs * 100;
	printf("\nYou have known %g%% (%d/%d) of the words.\n\n",percent,right,pairs);
	//End of program, close file
	fclose(vocabfile);
       	return EXIT_SUCCESS;
}
