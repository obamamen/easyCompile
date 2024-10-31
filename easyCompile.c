#include <stdio.h>
#include <stdlib.h>



typedef struct thing {
    char* string;
    int start;
    int length;
} thing;

int sameString(char* string1, char* string2) {
    int i = 0;
    while (string1[i]!= '\0' && string2[i]!= '\0') {
        if (string1[i]!= string2[i]) {
            return 0;
        }
        i++;
    }
    if (string1[i] == '\0' && string2[i] == '\0') {
        return 1;
    }
    return 0;
}

thing getThing(FILE* file, char start, char end, int priority) {
    char c;
    thing t;
    t.string = NULL;
    t.length = -1;
    t.start = -1;

    int started = 0;
    int length = 0;
    int startPos = 0;

    int oldCursorPos = (int)ftell(file);
    while ((c = getc(file)) != EOF) {
        if ((c == ';') && (priority == 0)) {
            break;
        }
        if (started == 1) { 
            if (c == end) {
                break;
            }
            length++;
        }

        if (c == start) {
            started = 1;
            startPos = (int)ftell(file); // get current position
        }
    }
    if (length == 0) {
        fsetpos(file, &(fpos_t){oldCursorPos}); 
        return t;
    }


    t.string = (char*)malloc(length + 1);
    if (t.string == NULL) {
        perror("Error allocating memory");
        exit(1);
    }
    t.string[length] = '\0';

    fsetpos(file, &(fpos_t){startPos}); 
    for (int i = 0; i < length; i++) {
        c = getc(file);
        t.string[i] = c;
    }

    t.start = startPos;
    t.length = length;
    return t;
}

int main(int arc, char **argv) {
    FILE *file = fopen("easycompile.ec", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    char* sepName = "";
    if (arc == 2) {
        sepName = argv[1];
    }


    int condition = 1;
    int found = 0;
    int startPos = 0;
    int limit = 0;
    while (condition) {
        thing sep = getThing(file, '*','*', 1);
        if (sep.string == NULL) {
            fsetpos(file, &(fpos_t){0}); 
            condition = 0;
            break;
        }
        if (found == 1) {
            limit = sep.start + sep.length;
            break;
        }
        if (sameString(sep.string, sepName) && (!found)) {
            free(sep.string);
            found = 1;
            startPos = sep.start;
        }     
        free(sep.string);
    }
    fsetpos(file, &(fpos_t){startPos});

    if (limit == 0) {
        condition = 1;
        found = 0;
        startPos = 0;
        limit = 0;
        while (condition) {
            thing sep = getThing(file, '*','*', 1);
            if (sep.string == NULL) {
                fsetpos(file, &(fpos_t){0}); 
                condition = 0;
                break;
            }
            if (found == 1) {
                limit = sep.start + sep.length;
                break;
            }
            if ((sep.string != NULL) && (!found)) {
                free(sep.string);
                found = 1;
                startPos = sep.start;
            }     
            free(sep.string);
        }
        fsetpos(file, &(fpos_t){startPos});
    }


    thing t = getThing(file, '{', '}', 0);
    if (t.string != NULL) {
        printf("name: %s", t.string);
        printf("\n");
    } else {
        printf("Name not found\n");
    }
    free(t.string);

    printf("flags: ");
    int start = 1;
    thing c;
    condition = 1;
    while (condition) {
        c = getThing(file, '[', ']',0); 
        if (c.string == NULL) {
            condition = 0;
            break;
        }
        if (c.start >= limit) {
            fsetpos(file, &(fpos_t){startPos});
            break;
        }
        if (c.string != NULL) {
            if (start == 1) {
                printf("%s", c.string);
            } else {
                printf(", %s", c.string);
            }     
        }
        free(c.string);
        start = 0;
    }
    printf("\n");


    printf("sources: ");
    start = 1;
    condition = 1;
    while (condition) {
        c = getThing(file, '(', ')',0); 
        if (c.string == NULL) {
            condition = 0;
            break;
        }
        if (c.start >= limit) {
            fsetpos(file, &(fpos_t){startPos});
            break;
        }
        if (c.string != NULL) {
            if (start == 1) {
                printf("%s", c.string);
            } else {
                printf(", %s", c.string);
            }     
        }
        free(c.string);
        start = 0;
    }
    printf("\n");


    fclose(file);
    return 0;
}
