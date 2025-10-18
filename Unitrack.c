#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MP 1000
#define ML 500

int IDCheck(char inputID[], char inputPass[]) {
    FILE *file;
    char fileID[20], filePass[50];

    file = fopen("login.txt", "r");  // open file in read mode
    if (file == NULL) {
        printf("Error: Files Missing.\n");
        return 0;
    }

    // Read file line by line and compare
    while (fscanf(file, "%s %s", fileID, filePass) != EOF) {
        if (strcmp(fileID, inputID) == 0 && strcmp(filePass, inputPass) == 0) {
            fclose(file);
            return 1; // credentials matched
        }
    }

    fclose(file);
    return 0; // credentials not found
}

int getNextSerialNumber() {
    FILE *file = fopen("found_items.txt", "r");
    if (file == NULL)
        return 1; // If file doesn’t exist, start from 1

    int count = 0;
    char line[256];

    // Count how many serial numbers (first lines of each entry) exist
    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) > 1 && line[0] >= '0' && line[0] <= '9')
            count++;
    }

    fclose(file);
    return count + 1;
}

void saveFoundItem() {
    char name[100];
    char details[200];
    char location[100];
    int serial = getNextSerialNumber();

    while (getchar() != '\n'); // clear input buffer

    // Take input from user
    printf("Enter Item Name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    printf("Enter Item Details: ");
    fgets(details, sizeof(details), stdin);
    details[strcspn(details, "\n")] = '\0';

    printf("Enter Location: ");
    fgets(location, sizeof(location), stdin);
    location[strcspn(location, "\n")] = '\0';

    FILE *file = fopen("found_items.txt", "a");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    fprintf(file, "%d\n", serial);
    fprintf(file, "%s\n", name);
    fprintf(file, "%s\n", details);
    fprintf(file, "%s\n\n", location); // blank line after each entry

    fclose(file);
    printf("\nItem saved successfully with Serial Number %d!\n", serial);
}

void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

void searchItem(char word[]) {
    FILE *fp = fopen("found_items.txt", "r");
    if (fp == NULL) {
        printf("Error: Cannot open file.\n");
        return;
    }

    char paragraph[MP] = "";
    char line[ML];
    int found = 0;
    char lowerParagraph[MP];
    char lowerWord[100];

    // Copy and lowercase the search word
    strcpy(lowerWord, word);
    toLowerCase(lowerWord);

    while (fgets(line, sizeof(line), fp)) {
        // Check for paragraph separator (empty line)
        if (strcmp(line, "\n") == 0) {
            // Make lowercase copy for searching
            strcpy(lowerParagraph, paragraph);
            toLowerCase(lowerParagraph);

            if (strstr(lowerParagraph, lowerWord)) {
                printf("Hey! We found your item!\n\n%s\n", paragraph);
                found = 1;
            }
            paragraph[0] = '\0'; // Reset paragraph
        } else {
            strcat(paragraph, line);
        }
    }

    // Check the last paragraph (in case file doesn't end with blank line)
    if (strlen(paragraph) > 0) {
        strcpy(lowerParagraph, paragraph);
        toLowerCase(lowerParagraph);

        if (strstr(lowerParagraph, lowerWord)) {
            printf("Hey! We found your item!\n%s\n", paragraph);
            found = 1;
        }
    }

    if (!found) {
        printf("Oops, we did not find it.\n");
    }

    fclose(fp);
}

void exportToTable(char inputFile[], char outputFile[]) {
    FILE *in = fopen(inputFile, "r");
    if (in == NULL) {
        printf("Error: Cannot open input file.\n");
        return;
    }

    FILE *out = fopen(outputFile, "w");
    if (out == NULL) {
        printf("Error: Cannot create output file.\n");
        fclose(in);
        return;
    }

    fprintf(out, "Serial No,Item Name,Item Details,Found Location\n");

    char line[ML];
    char serial[50] = "", name[100] = "", details[300] = "", location[200] = "";
    int lineCount = 0;

    while (fgets(line, sizeof(line), in)) {
        if (strcmp(line, "\n") == 0 || strcmp(line, "\r\n") == 0) {
            // End of one paragraph
            if (strlen(serial) > 0) {
                fprintf(out, "%s,%s,%s,%s\n", serial, name, details, location);
                serial[0] = name[0] = details[0] = location[0] = '\0';
                lineCount = 0;
            }
        } else {
            line[strcspn(line, "\n")] = 0; // remove newline
            lineCount++;
            if (lineCount == 1)
                strcpy(serial, line);
            else if (lineCount == 2)
                strcpy(name, line);
            else if (lineCount == 3)
                strcpy(details, line);
            else if (lineCount == 4)
                strcpy(location, line);
        }
    }

    // handle last paragraph
    if (strlen(serial) > 0) {
        fprintf(out, "%s,%s,%s,%s\n", serial, name, details, location);
    }

    fclose(in);
    fclose(out);
    printf("Data exported successfully to %s\n", outputFile);
}

void ViewAll(char filename[]){
    FILE *fp = fopen(filename, "r");  // open file in read mode
    if (fp == NULL) {
        printf("Error: File Missing\n");
        return;
    }

    char ch;
    printf("\n----- All the Lost Items -----\n");

    // Read and display each character until EOF
    while ((ch = fgetc(fp)) != EOF) {
        putchar(ch);
    }

    fclose(fp);
    printf("\n----- END -----\n");
}

void retrieveItem(int retriNum) {
    FILE *fp = fopen("found_items.txt", "r");
    if (fp == NULL) {
        printf("Error: Cannot open file.\n");
        return;
    }

    char paragraphs[MP][ML];
    int paraCount = 0;
    char line[ML];
    char tempPara[ML] = "";
    int serial = 0;

    // Read file paragraph by paragraph
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] != '\n' && line[0] != '\r') {
            strcat(tempPara, line);
        } else {
            if (strlen(tempPara) > 0) {
                strcpy(paragraphs[paraCount++], tempPara);
                tempPara[0] = '\0';
            }
        }
    }
    if (strlen(tempPara) > 0)
        strcpy(paragraphs[paraCount++], tempPara);

    fclose(fp);

    // Reopen file to overwrite
    fp = fopen("found_items.txt", "w");
    if (fp == NULL) {
        printf("Error: Cannot write to file.\n");
        return;
    }

    int newSerial = 1;
    for (int i = 0; i < paraCount; i++) {
        // Extract first number from paragraph
        int num;
        sscanf(paragraphs[i], "%d", &num);
        if (num == retriNum) continue; // skip the deleted one

        // Rewrite with updated serial number
        char *rest = strchr(paragraphs[i], '\n');
        if (rest != NULL) rest++; else rest = "";
        fprintf(fp, "%d\n%s\n", newSerial++, rest);
    }

    fclose(fp);
    printf("Item Retrieved Successfully.\n");
}

int adminOption() {
    int choice,retriNum;

    while (1) {
        printf("Login successful!\n");
        printf("1. Add Lost Item\n");
        printf("2. Return Lost Item\n");
        printf("3. View All Lost Items\n");
        printf("4. Export Data\n");
        printf("5. Return to Home\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                saveFoundItem();
                break;
            case 2:
                printf("Enter the Item Serial: ");
                scanf("%d",&retriNum);
                retrieveItem(retriNum);
                break;
            case 3:
                ViewAll("found_items.txt");
                break;
            case 4:
                exportToTable("found_items.txt", "found_items.csv");
                break;
            case 5:
                return 1;
            default:
               printf("Invalid choice. Try again\n\n");
        }
    }

    return 0;
}

void admin() {

    char inputID[20], inputPass[50];
    int status;

    // Ask user for ID and password
    printf("Enter your ID: ");
    scanf("%s", inputID);
    printf("Enter your Password: ");
    scanf("%s", inputPass);

    // Check login
    status = IDCheck(inputID, inputPass);

    if (status == 1) {
       if( adminOption()) return ;

    } else {
        printf("Wrong ID or Password.\n");
    }
}

int main() {
    int choice;

    char word[20];

    while (1) {
        printf("Welcome to Unitrack - Smart Lost and Found Platform\n");
        printf("1. Admin\n");
        printf("2. Report Found Item\n");
        printf("3. Search For lost Item\n");
        printf("4. Retrieve Your Item \n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                admin();
                break;
            case 2:
                printf("Great! Submit to the Admin\n");
                break;
            case 3:
                printf("Enter Which Item You Searching for: ");
                scanf("%s", word);
                searchItem(word);
                break;
            case 4:
                printf("Please Contact Admin");
                break;
            case 5:
                printf("Goodbye!\n");
                exit(0);
            default:
               printf("Invalid choice. Try again\n\n");
        }
    }

    return 0;
}
