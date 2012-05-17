#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#define MAX_WORDS 5
#define MAX_TRIES 2
#define MAX_LENGTH 40

char *errors[] = {"VocabC requires argument -f <file>","Error in opening vocabulary file","too high argument of option -n","Error in reading vocabulary file",
			"Error in input"};

int Error(int error) {
	printf("Error - %s!\n",errors[error]);
	exit(EXIT_FAILURE);
}


int main(int argc, char **argv) {

	char *fvalue = NULL;
	char *dvalue = "1";
	char *nvalue = "all";
	int rvalue = 0;
	int CHAR;
	opterr = 0;
	
	FILE *vocabfile;
	char lang1_word[MAX_LENGTH], source_str[MAX_LENGTH], input_str[MAX_LENGTH], line[MAX_LENGTH], temp[MAX_LENGTH], temp_word[MAX_LENGTH];
	char lang2_word[MAX_WORDS][MAX_LENGTH];
	char *ptr;
	char token[] = "=,\n";
	int pairs = 0, pair = 1;
	int lines = 0;
	int word = 0;
	int i,j,k,l,m,n;
	int is_giv;
	int index_a, index_b, temp_rand;
	int right = 0, correct = 0, tries = 0;
	float percent;

	for (i = 0; i < MAX_WORDS; i++) {
		strcpy(lang2_word[i],"NULL");
	}
		if (argc < 2) {
			Error(0);
	}
	while ((CHAR = getopt (argc, argv, "hrf:d:n:")) != -1) {
		switch (CHAR) {
          		case 'h':
				printf("\nVocabC v1.3\n");
            			printf("\nUse:\tVocabC -f <file>\n");
				printf("\nOptional arguments:\n-h\tShow this help\n-r\tRandomize the order of the words\n");
				printf("-d1\tThe program asks the first word\n-d2\tThe program asks the second word\n");
				printf("-n <num>\tAsk only <num> words\n");
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
	vocabfile = fopen(fvalue,"r");
	if (NULL == vocabfile) {
		Error(1);
	}
	while ((fgets(line,MAX_LENGTH,vocabfile)) != NULL) {
		lines++;
	}
	srand(time(NULL));
	int rand_lines[lines-1];
	for(i = 0; i < lines; i++) {
		rand_lines[i] = i+1;
	}
	if (rvalue == 1) {
		for (j = 0; j < 1000; j++) {
			index_a = rand() % lines;
			index_b = rand() % lines;
			temp_rand = rand_lines[index_a];
			rand_lines[index_a] = rand_lines[index_b];
			rand_lines[index_b] = temp_rand;
		}
	}
	if (strcmp(nvalue, "all") == 0) {
		pairs = lines;
	} else {
		pairs = atoi(nvalue);
	}
	if (pairs > lines) {
		Error(2);
	}
	printf("Word pairs: %d\n",pairs);
	for (k = 0; k < pairs; k++) {
		fseek(vocabfile,0L,SEEK_SET);
		word = 0;
		for (n = 0; n < rand_lines[k]-1; n++) {
			if (fgets(temp, MAX_LENGTH, vocabfile) == NULL) 
				Error(3);
		}
		if (fgets(source_str, MAX_LENGTH, vocabfile) == NULL)
			Error(3);
		ptr = strtok(source_str,token);
		strcpy(lang1_word,ptr);
		while (ptr != NULL) {
			ptr = strtok(NULL,token);
			if (ptr != NULL) {
				strcpy(lang2_word[word],ptr);
			}
		word++;
		}
		is_giv = 0;
		if (strcmp(dvalue,"2") == 0) {
			m = rand() % MAX_WORDS;
			while (is_giv != 1) {
				m = rand() % MAX_WORDS;
				if (strcmp(lang2_word[m],"NULL") != 0) {
					is_giv = 1;
				}
			}
			strcpy(temp_word,lang1_word);
			strcpy(lang1_word,lang2_word[m]);
			strcpy(lang2_word[m],temp_word);
		}
		correct = 0;
		tries = 0;
		while (tries < MAX_TRIES && correct != 1) {
			tries++;
			printf("\n(%d/%d)\t%s ?\n>>> ", pair, pairs, lang1_word);
			if(fgets(input_str, MAX_LENGTH, stdin) == NULL) {
				Error(4);
			}
			input_str[strlen(input_str)-1] = '\0';
			for (l = 0; l < 5; l++) {
				if (strcmp(lang2_word[l], input_str) == 0) {
					correct = 1;
				}
			}
			if (correct == 1) {
				printf("Correct!\n");
				right++;
			} else {
				printf("Wrong!");
			}
		}
		if (correct != 1) {
			printf("\tCorrect was %s",lang2_word[0]);
		}
		printf("\n");
		pair++;
	}
	percent = (float) right / (float) pairs * 100;
	printf("\nYou have known %g%% (%d/%d) of the words.\n\n",percent,right,pairs);
	fclose(vocabfile);
       	return EXIT_SUCCESS;
}

