#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int system(const char *command);

int main() {
	char str[1000000];
    char *filename = "history.txt";
    FILE *fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("Error: could not open file %s", filename);
        return 1;
    }

    // read one character at a time and
    // display it to the output
    char ch;
    while ((ch = fgetc(fp)) != EOF) {
    	size_t len = strlen(str);
   
    /* one for extra char, one for trailing zero */
    char *str2 = malloc(len + 1 + 1);

    strcpy(str2, str);
    str2[len] = ch;
    str2[len + 1] = '\0';
    strcpy(str, str2);
    }
        
    // close the file
    fclose(fp);
    strcat(str, "</div></body></html>");
    
    FILE *wFile = fopen("hist.html", "w");
    	if(wFile==NULL) {
		perror("Error opening file.");
	} else { 
		fprintf(wFile, "%s", str);
	}
	fclose(wFile);
	system("xdg-open hist.html");

    return 0;
}