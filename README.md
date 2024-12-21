# SFSD LAB 2CP9 :
### BELGUESMIA OUSSAMA
### BOUHOUM ADLANE
# Source Structure :
./bin : contains binary files
./include : contains c header files (.h)
./resources : contains informations files (csv) and demanded work
./result : contains files created by the program (.TOF , .TOVS , log files)\
./src : contains all source code files (.c)
./config.config : contains some configurations for program (number of lines in csv files used by showProgress function)
./makefile : make file to compile the project using the gnu make tool
# Compile :
gcc ./src/*.c -o main.exe -I ./include/
or: make (require gnu make tool)
