#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Voter {
    char id[20], pin[10], name[50], dob[20], constituency[30];
    int hasVoted;
};

struct Candidate {
    char name[50], constituency[30];
    int votes;
};

int main() {
    FILE *f1, *f2;
    struct Voter v;
    struct Candidate c;

    // 1. Reset Voters
    f1 = fopen("voters.txt", "r");
    f2 = fopen("temp_v.txt", "w");
    if (f1 != NULL && f2 != NULL) {
        while (fscanf(f1, "%s %s %s %s %s %d", v.id, v.pin, v.name, v.dob, v.constituency, &v.hasVoted) != EOF) {
            fprintf(f2, "%s %s %s %s %s 0\n", v.id, v.pin, v.name, v.dob, v.constituency);
        }
        fclose(f1); fclose(f2);
        remove("voters.txt");
        rename("temp_v.txt", "voters.txt");
        printf("Voter flags reset to 0.\n");
    }

    // 2. Reset Candidates
    f1 = fopen("candidates.txt", "r");
    f2 = fopen("temp_c.txt", "w");
    if (f1 != NULL && f2 != NULL) {
        while (fscanf(f1, "%s %s %d", c.name, c.constituency, &c.votes) != EOF) {
            fprintf(f2, "%s %s 0\n", c.name, c.constituency);
        }
        fclose(f1); fclose(f2);
        remove("candidates.txt");
        rename("temp_c.txt", "candidates.txt");
        printf("Candidate vote counts reset to 0.\n");
    }

    // 3. Clear Audit Log
    f1 = fopen("log.txt", "w"); 
    if (f1 != NULL) {
        fprintf(f1, "--- NEW ELECTION SESSION STARTED ---\n");
        fclose(f1);
        printf("Audit log cleared.\n");
    }

    printf("\nSystem reset complete. Ready for a fresh run!\n");
    return 0;
}