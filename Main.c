#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


/*


____________________________________________________________________________________________________
______________________________________________COMMENTS______________________________________________

1)  fflush() is a system call that "flushes" any pending buffer on a specific file. 
    So, in the case of the file stdin (standard input), it flushes out any pending 
    characters in the input buffer. So basically it throws away anything I haven't read yet.

2)  Function memset() is a library function of "string. h" – it is used to fill a block of memory 
    with given/particular value. It is used when you want to fill all or some of the blocks of the 
    memory with a particular value.

____________________________________________________________________________________________________
*/


typedef struct{
    char VaccineName[15];
    char VaccineCode[5];
    char VaccineCountry[15];
    float VaccineQuantity;
    float population;
    int dose;
}Master_Record_STD; 


    void inventory_creation();
    void update_vaccine(); 
    int search_vaccine();
    void list_vaccine();
    void list_distribution(); 
    void list_received();
    bool compare_two_arrays(char * array1,char *array2,int size);
    bool assert_Vcode(char*inputVcode);
    void display_master_table_header();
    void display_master_record(Master_Record_STD* MasterRecord);
    void save_master_record(Master_Record_STD* MasterRecord);
    char get_update_vac_operation();
    float get_distributed_quant(float current_Qn);
    float get_received_quant();
    void log_received_operation(float Qn ,Master_Record_STD record);
    void log_distributed_operation(float Qn,Master_Record_STD record);
    int get_records_in_rec_file();
    int get_records_in_dist_file();
    
    


int main()
{

    int choice;
    do{
        
        printf("\n\t\t  ~Vaccine Inventory Managment System~  ");
        printf("\n\t\t________________________________________");
        printf("\n\t\t|      1. Create Inventory File        |");
        printf("\n\t\t|      2. Update Vaccine Quantities    |");
        printf("\n\t\t|      3. Search Vaccine By Code       |");
        printf("\n\t\t|      4. List Vaccines                |");
        printf("\n\t\t|      5. List Distribution Logs       |");
        printf("\n\t\t|      6. List Received Logs           |");
        printf("\n\t\t|      7. Exit                         |");
        printf("\n\t\t|______________________________________|");
        printf("\n\t\t Enter Choice: ");
        
        scanf("%d", &choice); 
        switch(choice)
        {
        case 1:
            inventory_creation(); //Function call
            break;
        case 2:
            update_vaccine(); //Function call
            break;
        case 3:
            search_vaccine();//Function call
            break;
        case 4: 
            list_vaccine(); //Function call
            break;
        case 5:
            list_distribution();//Function call
            break;

        case 6:
            list_received();//Function call
            break;

        case 7:
            printf("\nGoodbye!"); 
            printf("\nExiting..");
            exit(1); // Exits 
               
        default:
            printf("\nInvalid choice! Try again.");
        }
    }while(choice!= 7); // Loops until choice is 7
}

//=================================== MAIN FUNCTIONS ===================================//

void update_vaccine() // To update vaccine quantities 
 {
    Master_Record_STD Master_File_Records[5];
    int index =0;
    char Vcode[4];
    FILE *vfile;
    bool vcode_assert_result=false;
    char operation;
    float Qn=0.0;
    do{
        printf("Enter a Vaccine Code: ");
        scanf("%s",Vcode);
        fflush(stdin);
        vcode_assert_result = assert_Vcode(Vcode); // Checks that the vcode is within the defined list.
       }while(!vcode_assert_result); // Do while breaks only if the vcode is defined in the list | When function is true  
    /*
     double check that the vcode is correct.
    */
    vfile = fopen("vaccine.txt", "r"); 

    if(vfile == NULL) // Checks if file exist 
    {
        printf("\nFile not found."); 
    }
    
    memset(Master_File_Records,0,sizeof(Master_File_Records));// Initialize Master_File_Records with zeros to avoid printing junk values
    while(fscanf(vfile,"\t%s\t%s\t%s\t%d\t%f\t%f\n", 
                 Master_File_Records[index].VaccineName,
                 Master_File_Records[index].VaccineCode,
                 Master_File_Records[index].VaccineCountry,
                 &Master_File_Records[index].dose,
                 &Master_File_Records[index].population,
                 &Master_File_Records[index].VaccineQuantity) !=EOF &&++index<5); // Save the master record file into Master_File_Records struct
    fclose(vfile);
    
    display_master_record(Master_File_Records); //Prints Master_File_Records
    //Check which record has the same Vcode as the Vcode was entered by the user;
    for(index=0;index<5;index++)
    {
        if(compare_two_arrays(Vcode,Master_File_Records[index].VaccineCode,2))// If the result of the comparison is true
          {
              // Printed the vaccine quantity for information
              printf("\nCurrent Quantity of Vaccine %s is %15f",
                     Master_File_Records[index].VaccineName,
                     Master_File_Records[index].VaccineQuantity);
              operation = get_update_vac_operation();  // Forwards input to the function that receives user input for operation
              switch(operation)
              {
            case 'D':
                    Qn = get_distributed_quant(Master_File_Records[index].VaccineQuantity);
                    Master_File_Records[index].VaccineQuantity -= Qn; // Distribution operation (subtraction)
                    log_distributed_operation(Qn,Master_File_Records[index]); // Logs in dist.txt file
                break;
            case 'R':
                    Qn = get_received_quant();
                    Master_File_Records[index].VaccineQuantity += Qn; // Receiving operation (addition) 
                    log_received_operation(Qn,Master_File_Records[index]);
                break;
            case 'C':
            default:
                return;

              }
            save_master_record(Master_File_Records); // Saves master record to file
           }
    }

 }

 int search_vaccine() // Prints vaccine per code
{   
    char VaccineName[15];
    char VaccineCode[5];
    char VaccineCountry[15];
    
    float VaccineQuantity;
    int dose;
    float population;

    FILE *vfile;
    char vcode[5];
    char temp[5];

    vfile = fopen("vaccine.txt", "r"); // Opens vaccine.txt in read mode

    if(vfile == NULL)
    {
        printf("\nFile not found."); // Checks if file exists
    }
    
    printf("Enter Vaccine Code to Search : "); 
    scanf("%s",vcode); while (getchar() != '\n'); // User input

    strcpy(temp,vcode);
    while(fscanf(vfile,"\t%s\t%s\t%s\t%d\t%f\t%f\n",VaccineName,VaccineCode,VaccineCountry,&dose,&population,&VaccineQuantity) !=EOF)
    {
        if(VaccineCode[0] == temp[0] && VaccineCode[1] == temp[1])
        {
            printf("------------------------------------------------------------------------------------------------------------\n");
            printf("   |    NAME     |     CODE     |     COUNTRY     |     DOSAGE     |     POPULATION     |     QUANTITY\n");
            printf("------------------------------------------------------------------------------------------------------------\n");
            printf("%15s\t\t%s\t%13s\t%11d\t%18f\t%15f\n",VaccineName,VaccineCode,VaccineCountry,dose,population,VaccineQuantity);
        }
    }
    fclose(vfile);

return 0;
}

void list_vaccine() // Prints all vaccines
{
    char VaccineName[15];
    char VaccineCode[5];
    char VaccineCountry[15];

    float VaccineQuantity;
    int dose;
    float population;
    
    FILE *vfile;
    vfile = fopen("vaccine.txt","r"); // Opens vaccine.txt in read mode

    if(vfile == NULL) // Checks if file exists or not
    {
        printf("\nFile not found\n.");
        exit(1);
    }
    
        printf("\n------------------------------------------------------------------------------------------------------------\n");
        printf("   |    NAME     |     CODE     |     COUNTRY     |     DOSAGE     |     POPULATION     |     QUANTITY\n");
        printf("------------------------------------------------------------------------------------------------------------\n");

  
    while(fscanf(vfile,"\t%s\t%s\t%s\t%d\t%f\t%f\n",VaccineName,VaccineCode,VaccineCountry,&dose,&population,&VaccineQuantity) !=EOF)
    {
    
        printf("%15s\t\t%s\t%13s\t%11d\t%18f\t%15f\n",VaccineName,VaccineCode,VaccineCountry,dose,population,VaccineQuantity);

    }
    fclose(vfile); 
}

void list_distribution() // Prints distributed quantities
{
    int number_of_records=get_records_in_dist_file();
    Master_Record_STD record[number_of_records];
    Master_Record_STD tmp_record;
    int index0 = 0;
    int index1 = 0;
    FILE *vfile;
    vfile = fopen("dist.txt", "r");

    if(vfile == NULL) // Checks if file exist
    {
        printf("\nFile not found.\n");
        exit(1);
    }

    while(fscanf(vfile,"\t%s\t distributed %15f\n",record[index0].VaccineName,
                 &record[index0].VaccineQuantity)!=EOF)index0++; // Reads all enteries
     // Display sorted quantities
    for(index0 =0;index0<number_of_records;index0++)
    {
     for(index1 = index0+1;index1<number_of_records;index1++)  // Compares and swaps quantities (Sorting)
     {
         if(record[index0].VaccineQuantity<record[index1].VaccineQuantity) // Compares and swaps quantities (Sorting)
         {
             //Swap
             tmp_record = record[index0];
             record[index0]=record[index1];
             record[index1]=tmp_record;
         }
     }
    }
    for(index0 =0;index0<number_of_records;index0++)
    {
        printf("\t%s\t distributed %15f\n",record[index0].VaccineName,record[index0].VaccineQuantity);
    }

    fclose(vfile);
}

void inventory_creation() // Function to create to inventory file and input respective data
{
    int choice = 1;
    
    char VaccineName[15];
    char VaccineCode[5];
    char VaccineCountry[15];

    float VaccineQuantity;
    int dose;
    float population;

    FILE *vfile;
    vfile = fopen("vaccine.txt", "w");  // Opens vaccine.txt in write mode.

    if(vfile == NULL) // Checks if file exist  
    {
        printf("\nFile not found.\n");
        exit(1);
    }


    while(choice != 0) // Starts a while loop until the user types 0, stop adding more data in vaccine.txt.
    {
        printf("Enter the Vaccine Name: ");
        scanf("%s",VaccineName); while (getchar() != '\n');
        fflush(stdin);

        printf("Enter the Vaccine Code: ");
        scanf("%s",VaccineCode); while (getchar() != '\n');
        fflush(stdin);

        printf("Enter the Country of Origin: ");
        scanf("%s",VaccineCountry); while (getchar() != '\n');
        fflush(stdin);

        printf("Enter the Dosage Required: ");
        scanf("%d",&dose); while (getchar() != '\n');
        fflush(stdin);

        printf("Enter the Population Covered: ");
        scanf("%f",&population); while (getchar() != '\n');
        fflush(stdin);

        printf("Enter Vaccine Quantitity (in millions): ");
        scanf("%f",&VaccineQuantity); while (getchar() != '\n');
        fflush(stdin);

        fprintf(vfile,"\t%s\t%s\t%s\t%d\t%f\t%f\n",VaccineName,VaccineCode,VaccineCountry,dose,population,VaccineQuantity);
    
        printf("\nDo you want to add more? Anything | Yes & 0 | No : "); // Option to add more data into the file until input 0
        scanf("%d",&choice);

        if(choice == 0) // An if case where if the user input is 0 the file saves and closes
            fclose(vfile);  
    }
}

void list_received() // Prints received quantities
{
    int number_of_records=get_records_in_rec_file();
    Master_Record_STD record[number_of_records];
    Master_Record_STD tmp_record;
    int index0 = 0;
    int index1 = 0;
    FILE *vfile;
    vfile = fopen("rec.txt", "r");

    if(vfile == NULL) // Checks if file exist
    {
        printf("\nFile not found.\n");
        exit(1);
    }

    while(fscanf(vfile,"\t%s\t received %15f\n",record[index0].VaccineName,
                 &record[index0].VaccineQuantity)!=EOF)index0++; //Reads all enteries
     // display sorted quantities
    for(index0 =0;index0<number_of_records;index0++)
    {
     for(index1 = index0+1;index1<number_of_records;index1++) // Compares and swaps quantities (Sorting)
     {
         if(record[index0].VaccineQuantity<record[index1].VaccineQuantity) // Compares and swaps quantities (Sorting)
         {
             //Swap
             tmp_record = record[index0];
             record[index0]=record[index1];
             record[index1]=tmp_record;
         }
     }
    }
    for(index0 =0;index0<number_of_records;index0++)
    {
        printf("\t%s\t received %15f\n",record[index0].VaccineName,record[index0].VaccineQuantity);
    }

    fclose(vfile);

}


//=================================== SUB FUCNTIONS ===================================//

bool compare_two_arrays(char * array1,char *array2,int size) // Function compares input with listed codes
{
    int index =0;
    for(index=0;index < size; index++)
    {
        if(array1[index]!=array2[index]) return false; // Returns false if input and listed codes don't match
    }
    return true;
}

bool assert_Vcode(char*inputVcode)
{
    char vcode_list[5][3]={"PF","SV","AZ","SP","CS"}; // Registered vaccine codes to search before updating quantities 
    int index =0; 
    for(index=0;index<5;index++) 
    {
        if(compare_two_arrays(vcode_list[index],inputVcode,2))return true; // Returns true if the user input matches the given list
    }
return false;
}

//___________________________________________________________________//

void display_master_table_header() // Function to display header format before displaying data
{
            printf("------------------------------------------------------------------------------------------------------------\n");
            printf("   |    NAME     |     CODE     |     COUNTRY     |     DOSAGE     |     POPULATION     |     QUANTITY IN M\n");
            printf("------------------------------------------------------------------------------------------------------------\n");

}

void display_master_record(Master_Record_STD* MasterRecord) 
{
  int index=0;
  display_master_table_header(); // Function to display header format before displaying data
  for(;index<5 && MasterRecord[index].VaccineName[0]!= 0;index++) // To prevent junk values
    printf("%15s\t\t%s\t%13s\t%11d\t%18f\t%15f\n",MasterRecord[index].VaccineName,
           MasterRecord[index].VaccineCode,
           MasterRecord[index].VaccineCountry,
           MasterRecord[index].dose,
           MasterRecord[index].population,
           MasterRecord[index].VaccineQuantity);
}

void save_master_record(Master_Record_STD* MasterRecord) 
{
 int index=0;
 FILE *vfile;
 vfile = fopen("vaccine.txt", "w");
 for(;index<5 && MasterRecord[index].VaccineName[0]!= 0;index++)
    fprintf(vfile,"\t%s\t%s\t%s\t%d\t%f\t%f\n",MasterRecord[index].VaccineName,
           MasterRecord[index].VaccineCode,
           MasterRecord[index].VaccineCountry,
           MasterRecord[index].dose,
           MasterRecord[index].population,
           MasterRecord[index].VaccineQuantity);
fclose(vfile);
}

//___________________________________________________________________//

char get_update_vac_operation() // Function that receives user input before receive or distribute 
{
    char input;
    fflush(stdin);
    do{
        printf("\nDo you want to Receive (R) , Distribute (D) or Cancel (C): ");
        scanf("%c",&input);
        fflush(stdin);
    }while(input!= 'C' &&input!= 'R'&&input!= 'D');
    return input;
}

float get_distributed_quant(float current_Qn) // Function to receive user input for distribution and initial quantity
{
    float input;
    fflush(stdin);
    do{
        printf("\nEnter Distribution Quantity (less than %15f):",current_Qn);
        scanf("%f",&input);
        fflush(stdin);
    }while(input>current_Qn); // Do while to prevent input greater than initial quantity
    return input;
}

float get_received_quant() // Function to receive user input for receiving quantities
{
    float input;
    fflush(stdin);
    do{
        printf("\nEnter Quantity Received: ");
        scanf("%f",&input);
    }while(input<=0.0); // Do while to avoid negative input
    return input;
}

//___________________________________________________________________//

int get_records_in_dist_file()
{
    int cnt =0;
    Master_Record_STD record;

    FILE *vfile;
    vfile = fopen("dist.txt", "r");

    if(vfile == NULL) // Checks if file exist
    {
        printf("\nFile not found.\n");
        int main();
    }  
    
    // Counts the numbers of vaccine distributions to save before sorting
    while(fscanf(vfile,"\t%s\t distributed %15f\n",record.VaccineName,&record.VaccineQuantity)!=EOF) cnt++;  
    fclose(vfile);
    return cnt;
}

int get_records_in_rec_file()
{
    int cnt =0;
    Master_Record_STD record;

    FILE *vfile;
    vfile = fopen("rec.txt", "r");

    if(vfile == NULL) // Checks if file exist
    {
        printf("\nFile not found.\n");
        int main();
    }  
    
    // Counts the numbers of vaccine distributions to save before sorting
    while(fscanf(vfile,"\t%s\t received %15f\n",record.VaccineName,&record.VaccineQuantity)!=EOF) cnt++;  
    fclose(vfile);
    return cnt;
}

//___________________________________________________________________//


void log_received_operation(float Qn ,Master_Record_STD record) // Logs received quantities (in rec.txt)
{
 FILE *vfile;
 vfile = fopen("rec.txt", "a");
 fprintf(vfile,"\t%s\t received %15f\n",record.VaccineName,
           Qn);
fclose(vfile);
}

void log_distributed_operation(float Qn,Master_Record_STD record) // Logs distributed quantities (in dist.txt)
{
 FILE *vfile;
 vfile = fopen("dist.txt", "a");
 fprintf(vfile,"\t%s\t distributed %15f\n",record.VaccineName,
           Qn);
fclose(vfile);

}

//___________________________________________________________________//