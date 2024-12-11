#include <stdio.h>
#include <stdlib.h>
#include <utils.h>

void printMenu() {
    printf("Select an option:\n");
    printf("1. Create TOF file\n");
    printf("2. Create TOVS file\n");
    printf("3. Delete selected records\n");
    printf("4. Information about the files\n");
    printf("5. Status\n");
    printf("0. Exit\n");
    printf("Enter your choice: ");
}

int main() {
    int choice;
    
    while (1) {
        printMenu();
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                printf("Creating TOF file...\n");
                // Add function call for creating TOF file
                break;
            case 2:
                printf("Creating TOVS file...\n");
                // Add function call for creating TOVS file
                break;
            case 3:
                printf("Deleting selected records...\n");
                // Add function call for deleting records
                break;
            case 4:
                printf("Displaying information about the files...\n");
                // Add function call for displaying file information
                break;
            case 5:
                printf("Displaying status...\n");
                // Add function call for displaying status
                break;
            case 0:
                printf("Exiting the program...\n");
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
        
        printf("\n");
    }
    
    return 0;
}

bool CreatTOFMenu(){
    FILE *TOF_log , *csv;
    TOF_FILE TOF_file;
    printf("Reading from %s...\n",CSV1_NAME);
    csv = fopen(CSV1_NAME,"r");

    if (csv == NULL){
        printf("ERROR OPEN FILES!\ncheck files and try again...\n");  
        return false; 
    }
    printf("Creating %s and %s\n", TOF_FILE , TOF_LOG_FILE);
    TOVS_log = fopen(TOVS_LOG_FILE,"w");
    TOVS_open(TOVS_FILE_NAME , &TOF_file , 'n');

    TOF_createFile(&TOF_file , csv , TOF_log);
}
bool CreatTOVSMenu(){
    FILE *TOVS_log , *csv;
    TOF_FILE TOF_file;
    TOVS_FILE TOVS_file;

    
    printf("Reading from %s and %s\n",CSV2_NAME , TOF_FILE_NAME);
    csv = fopen(CSV2_NAME,"r");
    TOF_open(TOF_FILE_NAME , &TOF_file , 'n');

    if (TOVS_log == NULL || csv == NULL) r{
        printf("ERROR OPEN FILES!\ncheck files and try again...\n");
        return false;
    }
    
    printf("Creating %s and %s\n", TOVS_FILE_NAME , TOVS_LOG_FILE);
    TOVS_log = fopen(TOVS_LOG_FILE,"w");
    TOVS_open(TOVS_FILE_NAME , &TOVS_file , 'n');
    if (TOVS_log == NULL || TOVS_file == NULL){
        printf("ERROR CREATING FILES!\ncheck files and try again...\n");
        return false;
    }

    



}