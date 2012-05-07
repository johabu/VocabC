#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

int main(int argc, char **argv) {

	char *fvalue = NULL;
	int rvalue = 0;
	int CHAR;
	opterr = 0;
	
	FILE *vocabfile;
	char lang1_word[40];
	char lang2_word[40];
	char source_str[40];
	char newstr[40];
	char token[] = "=\n";
	char input_str[40];
	char line[40];
	int pairs = 0;
	int pair = 1;
	int i,j,k,n;
	char temp[40];
	int index_a, index_b, tmp;
	int right = 0;
	float percent;
	
	while ((CHAR = getopt (argc, argv, "hrf:")) != -1) {
		switch (CHAR) {
          		case 'h':
            			printf("\nUse:\tvocabc -f <file>\n");
				return EXIT_FAILURE;	
            		case 'f':
				fvalue = optarg;
             			break;
			case 'r':
				rvalue = 1;
				break;
           		case '?':
             			if (optopt == 'f') {
               				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
					return EXIT_FAILURE;
             			} else if (isprint (optopt)) {
               				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
					return EXIT_FAILURE;
             			} else {
               				fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
             				return EXIT_FAILURE;
				}
           		default:
             			return EXIT_FAILURE;
           	}
	}
	vocabfile = fopen(fvalue,"r");
	if (NULL == vocabfile) {
		printf("\nError in Opening...\n");
		return EXIT_FAILURE;
	}
	while ((fscanf(vocabfile,"%s\n",line)) != EOF) {
		pairs++;
	}
	srand(time(NULL));
	int rand_lines[pairs-1];
	for(i = 0; i < pairs; i++) {
		rand_lines[i] = i+1;
	}
	if (rvalue == 1) {
		for (j = 0; j < 1000; j++) {
			index_a = rand() % pairs;
			index_b = rand() % pairs;
			tmp = rand_lines[index_a];
			rand_lines[index_a] = rand_lines[index_b];
			rand_lines[index_b] = tmp;
		}
	}
	printf("Word pairs: %d\n",pairs);
	for (k = 0; k < pairs; k++) {
		fseek(vocabfile,0L,SEEK_SET);
		for (n = 0; n < rand_lines[k]-1; n++) {
			if (fgets(temp, 40, vocabfile) == NULL) 
				return EXIT_FAILURE;
		}
		if (fgets(source_str,40,vocabfile) == NULL)
			return EXIT_FAILURE;
		strcpy(newstr,strtok(source_str,token));
		strcpy(lang1_word,newstr);
		strcpy(newstr,strtok(NULL,token));
		strcpy(lang2_word,newstr);
		printf("(%d/%d)\t%s ?\n",pair,pairs,lang1_word);
		fscanf(stdin,"%s",input_str);
		if (strcmp(lang2_word, input_str) == 0) {
			printf("Correct!\n");
			right=right+1;
		} else {
			printf("Wrong!\n");
		}
		pair++;
	}
	percent = (float) right / (float) pairs * 100;
	printf("You have known %g%% (%d/%d) of the words.\n",percent,right,pairs);
	fclose(vocabfile);
       	return EXIT_SUCCESS;
     }

