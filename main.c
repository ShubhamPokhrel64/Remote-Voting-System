#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <ctype.h>

/* --- DATA STRUCTURES --- */
struct Voter {
    char id[20];
    char pin[10];
    char name[100];  
    char dob[30];        
    char constituency[50]; 
    int hasVoted;
};

struct Candidate {
    char name[60];         
    char constituency[40];  
    int votes;
};

/* --- SECURITY: HASHING --- */
unsigned long hash_key(char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

/* --- LOGGING SYSTEM --- */
void log_action(char *message) {
    FILE *fp = fopen("log.txt", "a");
    if (!fp) return;
    time_t now = time(NULL);
    char *t = ctime(&now);
    t[strlen(t) - 1] = '\0';
    fprintf(fp, "[%s] %s\n", t, message);
    fclose(fp);
}

/* --- MASKED PIN ENTRY --- */
void get_masked_pin(char *pin) {
    int i = 0;
    char ch;
    while (1) {
        ch = getch();
        if (ch == 13 || ch == 10) break;
        if (ch == 8 || ch == 127) {
            if (i > 0) { i--; printf("\b \b"); }
        } else if (i < 9 && isdigit(ch)) {
            pin[i++] = ch;
            printf("*");
        }
    }
    pin[i] = '\0';
    printf("\n");
}

/* --- RESULTS & WINNER ANNOUNCEMENT --- */
void display_results() {
    struct Candidate c[100];
    struct Voter v;
    int cCount = 0, totalVoters = 0, totalVotesCast = 0;
    float turnout = 0.0;

    // Calculate Turnout
    FILE *fv = fopen("voters.txt", "r");
    if (fv != NULL) {
        while (fscanf(fv, "%s %s %s %s %s %d", v.id, v.pin, v.name, v.dob, v.constituency, &v.hasVoted) != EOF) {
            totalVoters++;
            if (v.hasVoted == 1) totalVotesCast++;
        }
        fclose(fv);
    }

    if (totalVoters > 0) {
        turnout = ((float)totalVotesCast / totalVoters) * 100;
    }

    //Load Candidates
    FILE *fc = fopen("candidates.txt", "r");
    if (!fc) return;
    while (fscanf(fc, "%s %s %d", c[cCount].name, c[cCount].constituency, &c[cCount].votes) != EOF) {
        cCount++;
    }
    fclose(fc);

    system("cls");
    printf("====================================================\n");
    printf("           ELECTION COMMISSION OF NEPAL             \n");
    printf("                OFFICIAL RESULTS                    \n");
    printf("====================================================\n");
    
    // Display Turnout Statistics
    printf(" TOTAL REGISTERED: %-5d | ACTUAL VOTES: %-5d\n", totalVoters, totalVotesCast);
    printf(" OVERALL VOTER TURNOUT: %.2f%%\n", turnout);
    printf("----------------------------------------------------\n");
    
    printf("%-20s %-20s %-10s\n", "CANDIDATE", "CONSTITUENCY", "VOTES");
    printf("----------------------------------------------------\n");
    for (int i = 0; i < cCount; i++) {
        printf("%-20s %-20s %-10d\n", c[i].name, c[i].constituency, c[i].votes);
    }

    printf("\n====================================================\n");
    printf("                WINNERS BY CONSTITUENCY             \n");
    printf("====================================================\n");

    char processed[20][30];
    int pCount = 0;

    for (int i = 0; i < cCount; i++) {
        int seen = 0;
        for (int k = 0; k < pCount; k++) if (strcmp(processed[k], c[i].constituency) == 0) seen = 1;
        if (seen) continue;

        int maxV = -1, winIdx = -1, ties = 0;
        for (int j = 0; j < cCount; j++) {
            if (strcmp(c[i].constituency, c[j].constituency) == 0) {
                if (c[j].votes > maxV) { maxV = c[j].votes; winIdx = j; ties = 0; }
                else if (c[j].votes == maxV && maxV != 0) { ties = 1; }
            }
        }

        if (maxV <= 0) printf("Area: %-12s | No votes cast.\n", c[i].constituency);
        else if (ties) printf("Area: %-12s | Result: TIE (No winner)\n", c[i].constituency);
        else printf("Area: %-12s | Winner: %-15s (%d)\n", c[i].constituency, c[winIdx].name, maxV);
        
        strcpy(processed[pCount++], c[i].constituency);
    }
    printf("====================================================\n");
    printf("\nPress any key to exit...");
    getch();
}

/* --- VOTING LOGIC --- */
void cast_vote(struct Voter *v) {
    struct Candidate c[20];
    int count = 0, choice;
    FILE *fp = fopen("candidates.txt", "r");
    
    if (fp == NULL) {
        printf("Error: candidates.txt not found!\n");
        return;
    }

    system("cls");
    printf("Constituency: [%s]\n", v->constituency);
    printf("Select Candidate:\n");

    while (fscanf(fp, "%s %s %d", c[count].name, c[count].constituency, &c[count].votes) != EOF) {
        
        if (strcmp(c[count].constituency, v->constituency) == 0) {
            printf("%d. %s\n", count + 1, c[count].name);
            count++;
        }
    }
    fclose(fp);

    if (count == 0) {
        printf("\n[!] No candidates found for area: %s\n", v->constituency);
        printf("Check candidates.txt for spelling errors.\n");
        getch();
        return;
    }

    printf("\nEnter Choice (1-%d): ", count);
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > count) {
        printf("Invalid choice. Vote cancelled.\n");
        return;
    }

    // --- REWRITING CANDIDATES FILE ---
    FILE *f1 = fopen("candidates.txt", "r");
    FILE *f2 = fopen("temp_c.txt", "w");
    struct Candidate temp;

    while (fscanf(f1, "%s %s %d", temp.name, temp.constituency, &temp.votes) != EOF) {
        
        if (strcmp(temp.name, c[choice-1].name) == 0) {
            temp.votes++; 
            printf("\nDEBUG: Vote added to %s! New total: %d\n", temp.name, temp.votes);
        }
        fprintf(f2, "%s %s %d\n", temp.name, temp.constituency, temp.votes);
    }
    fclose(f1);
    fclose(f2);
    remove("candidates.txt");
    rename("temp_c.txt", "candidates.txt");

    // --- REWRITING VOTERS FILE (Marking as Voted) ---
    FILE *v1 = fopen("voters.txt", "r");
    FILE *v2 = fopen("temp_v.txt", "w");
    struct Voter tv;

    while (fscanf(v1, "%s %s %s %s %s %d", tv.id, tv.pin, tv.name, tv.dob, tv.constituency, &tv.hasVoted) != EOF) {
        if (strcmp(tv.id, v->id) == 0) {
            tv.hasVoted = 1;
        }
        fprintf(v2, "%s %s %s %s %s %d\n", tv.id, tv.pin, tv.name, tv.dob, tv.constituency, tv.hasVoted);
    }
    fclose(v1);
    fclose(v2);
    remove("voters.txt");
    rename("temp_v.txt", "voters.txt");
    
    char logMsg[100];
    sprintf(logMsg, "Vote successfully cast by %s (%s)", v->name, v->id);
    log_action(logMsg);

    printf("\nVote Recorded Successfully! Press any key...");
}

/* --- MAIN SYSTEM --- */
int main() {
    char inputID[30], inputPIN[10];
    unsigned long stored_hash;
    struct Voter v;

    // Load Master Key Hash
    FILE *fAdmin = fopen("admin.dat", "rb");
    if (!fAdmin) { printf("Error: admin.dat missing!"); return 1; }
    fread(&stored_hash, sizeof(unsigned long), 1, fAdmin);
    fclose(fAdmin);

    while (1) {
        system("cls");
        printf("--- REMOTE VOTING SYSTEM (NEPAL) ---\n");
        printf("Enter ID: ");
        scanf("%s", inputID);

        // FIRST CHECK: Is it the Master Key?
        if (hash_key(inputID) == stored_hash) {
            printf("Master Key Accepted. Close Polls? (Y/N): ");
            if (tolower(getch()) == 'y') { display_results(); break; }
            continue;
        }

        //SECOND CHECK: Voter Login
        printf("Enter PIN: ");
        get_masked_pin(inputPIN);

        int found = 0;
        FILE *fp = fopen("voters.txt", "r");
        while (fscanf(fp, "%s %s %s %s %s %d", v.id, v.pin, v.name, v.dob, v.constituency, &v.hasVoted) != EOF) {
            if (strcmp(v.id, inputID) == 0 && strcmp(v.pin, inputPIN) == 0) {
                found = 1; break;
            }
        }
        fclose(fp);

if (found) {
            if (v.hasVoted) {
                printf("\nACCESS DENIED: You have already voted.\n");
                getch();
                continue;
            }
            
            printf("\n--- IDENTITY VERIFIED ---\n");
            printf("Voter Name:    %s\n", v.name);
            printf("Date of Birth: %s\n", v.dob);
            printf("Constituency:  %s\n", v.constituency); 
            printf("-------------------------\n");
            
            printf("\nProceed to ballot? (Y/N): ");
            char choice = tolower(getch());
            if (choice == 'y') {
                cast_vote(&v);
                getch();
            }
        } 
        /* --- UPDATE STARTS HERE --- */
        else {
            printf("\n[!] ERROR: Invalid Voter ID or PIN.\n");
            printf("Press any key to try again...");
            log_action("Failed login attempt recorded.");
            getch();
        }
        /* --- UPDATE ENDS HERE --- */
    }
    return 0;
}