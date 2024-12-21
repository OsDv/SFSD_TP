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
                printf("Deleting selected records TOF...\n");
                TOFdeleteSelected();
                // Add function call for deleting records
                break;
                break;
            case 4:
                printf("Deleting selected records TOVS...\n");
                // Add function call for deleting records
                TOVSDeleteFromFile();
                break;
            case 5:
                printf("Creating/Update indexes for TOF (primary/BirthDate)...\n");
                creatTOF_primaryIndex();
                creatTOF_SIBirthDate();
                break;
            case 6:
                TOF_BirthDateQueryMenu();
                break;
            case 7:
                printf("Displaying information about the files...\n");
                PrintFilesInfos();
                // Add function call for displaying file information
                break;
            case 8:printStudentInfosMenu();
                break;
            case 9:
                TOF_loadIndexes();
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

