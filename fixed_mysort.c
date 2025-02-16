#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>

#define MAXLEN 1024
#define TRUE  1
#define FALSE 0

typedef struct Link {
    int data;
    struct Link* next;
} Link;

Link* sort(Link*);

int start_alarm(int);
void catch_alarm();

// Function to validate if a string is a valid integer
int is_valid_integer(const char *s) {
    if (*s == '-' || *s == '+') s++; // Allow negative or positive sign
    if (!isdigit(*s)) return FALSE; // First character must be a digit

    while (*s) {
        if (!isdigit(*s) && *s != '\n') return FALSE;
        s++;
    }
    return TRUE;
}

int main(int argc, char* argv[]) {
    Link *head = NULL, *x;
    char s[MAXLEN];

    if (argc != 3) {
        printf("Usage: sort <input list> <timeout>\n");
        exit(1);
    }

    /* Read input file assuming one integer per line */
    FILE* fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Error: can't open file: %s\n", argv[1]);
        exit(1); 
    }

    while (fgets(s, MAXLEN, fp) != NULL) {
        // Validate input before converting to integer
        if (!is_valid_integer(s)) {
            printf("Error: Invalid input detected, skipping: %s\n", s);
            continue;
        }

        char *endptr;
        long num = strtol(s, &endptr, 10); // Convert string to long safely

        // Ensure conversion was successful
        if (*endptr != '\n' && *endptr != '\0') {
            printf("Error: Invalid integer format, skipping: %s\n", s);
            continue;
        }

        // Allocate memory for new list node
        x = (Link*) malloc(sizeof(Link));
        if (x == NULL) {
            printf("Error: malloc failed\n");
            exit(1);
        }

        x->data = (int)num; // Store validated integer
        x->next = head;
        head = x;
    }
    fclose(fp);

    /* Setup alarm */
    int timeout = atoi(argv[2]);

    if (signal(SIGVTALRM, catch_alarm) == SIG_ERR) {
        printf("Error: catch_alarm failed\n");
        exit(1);
    }

    if (start_alarm(timeout) == -1) {
        printf("Error: start_alarm failed\n");
        exit(1);
    }

    /* Sort input list */
    head = sort(head);

    /* Output sorted list */
    for (x = head; x != NULL; x = x->next)
        printf("%d\n", x->data);

    return 0;
}

/* Bubble Sort for Linked List - Swaps Data Instead of Nodes */
Link* sort(Link* x) {
    if (x == NULL || x->next == NULL) 
        return x; // Handle empty or single-node lists

    int swapped;
    Link *ptr, *lptr = NULL;

    do {
        swapped = 0;
        ptr = x;

        while (ptr->next != lptr) {
            if (ptr->data > ptr->next->data) {  // Only swap if out of order
                int temp = ptr->data;
                ptr->data = ptr->next->data;
                ptr->next->data = temp;
                swapped = 1;
            }
            ptr = ptr->next;
        }
        lptr = ptr; // Reduce traversal range
    } while (swapped);

    return x;
}

int start_alarm(int timeout) {
  struct itimerval old, new;
  new.it_interval.tv_usec = 0;
  new.it_interval.tv_sec = 0;
  new.it_value.tv_usec = 0;
  new.it_value.tv_sec = timeout;
  return setitimer(ITIMER_VIRTUAL, &new, &old);
}

void catch_alarm() {
    printf("Error: timeout\n");
    exit(1);
}
