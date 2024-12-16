#include <utils.h>
#include <lib.h>
int main() {
    ConfigProgram();
    int choice;
    
    while (1) {
        printMenu();
        readINT(&choice);
        
        switch (choice) {
            case 1:
                printf("Creating TOF file...\n");
                // Add function call for creating TOF file
                CreatTOFMenu();
                break;
            case 2:
                printf("Creating TOVS file...\n");
                // Add function call for creating TOVS file
                CreatTOVSMenu();
                break;
            case 3:
                printf("Deleting selected records...\n");
                // Add function call for deleting records
                break;
            case 4:
                printf("Displaying information about the files...\n");
                // Add function call for displaying file information
                TOF_FILE toff;
                TOF_open(TOF_FILE_NAME,&toff,'r');
                TOF_printFile(&toff);
                break;
            case 5:
                printf("Displaying status...\n");
                // Add function call for displaying status
                printStudentInfosMenu();
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

