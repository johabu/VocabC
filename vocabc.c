#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

int main(int argc, char **argv) {

	char *fvalue = NULL;
	char *dvalue = "1";
	char *nvalue = "all";
	int rvalue = 0;
	int CHAR;
	opterr = 0;
	
	FILE *vocabfile;
	char lang1_word[40], source_str[40], input_str[40], line[40], temp[40], temp_word[40];
	char lang2_word[5][40];
	char *ptr;
	char token[] = "=,\n";
	int pairs = 0, pair = 1;
	int lines = 0;
	int word = 0;
	int i,j,k,l,n;
	int index_a, index_b, temp_rand;
	int right = 0;
	float percent;
	
	if (argc < 2) {
		printf("Error - VocabC requires argument -f <file>\n");
		return EXIT_FAILURE;
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
		printf("Error in Opening...\n");
		return EXIT_FAILURE;
	}
	while ((fgets(line,40,vocabfile)) != NULL) {
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
		printf("Error - too high argument of option -n\n ");
		return EXIT_FAILURE;
	}
	printf("Word pairs: %d\n",pairs);
	for (k = 0; k < pairs; k++) {
		fseek(vocabfile,0L,SEEK_SET);
		word = 0;
		for (n = 0; n < rand_lines[k]-1; n++) {
			if (fgets(temp, 40, vocabfile) == NULL) 
				return EXIT_FAILURE;
		}
		if (fgets(source_str,40,vocabfile) == NULL)
			return EXIT_FAILURE;
		ptr = strtok(source_str,token);
		strcpy(lang1_word,ptr);
		while (ptr != NULL) {
			ptr = strtok(NULL,token);
			if (ptr != NULL) {
				strcpy(lang2_word[word],ptr);
			}
		word++;
		}
		if (strcmp(dvalue,"2") == 0) {
			strcpy(temp_word,lang1_word);
			strcpy(lang1_word,lang2_word[0]);
			strcpy(lang2_word[0],temp_word);
		}
		printf("\n(%d/%d)\t%s ?\n>>> ",pair,pairs,lang1_word);
		if(fgets(input_str,40,stdin) == NULL) {
			printf("Error in input...");
			return EXIT_FAILURE;
		}
		input_str[strlen(input_str)-1] = '\0';
		int correct = 0;
		for (l = 0; l < 5; l++) {
			if (strcmp(lang2_word[l], input_str) == 0) {
				correct = 1;
			}
		}
		if (correct == 1) {
			printf("Correct!\n");
			right++;
		} else {
			printf("Wrong!\tRight: %s\n",lang2_word[0]);
		}
		pair++;
	}
	percent = (float) right / (float) pairs * 100;
	printf("\nYou have known %g%% (%d/%d) of the words.\n\n",percent,right,pairs);
	fclose(vocabfile);
       	return EXIT_SUCCESS;
}

