/**
 * Final Project Semister 2022.2 - Programming Technical EE3491
 * @file HexDisplay.c
 * Nguyen Viet Hoang 20212817
 * - Program requires:
 *  Open file
 *  Check if the file is Hex file
 *  Read each line of the file
 *  Display the file to the monitor
 *
 * - Technical
 *  The program run via Terminal with the command is HexDisplay <input file>
 *  Print maximum of 25 lines at a time, after that ask user if want to continue printing
 *  
 * - Handle file errors
 *  If file couldn't be opened, print "FILENAME could not be opened"
 *  If any line didn't matched with intel hex's format, end the program and display the error of that line
 *
 * - Bonus require
 *  Comment out the meaning of each funcion, input, output 
 */

// Required library
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define DATA                        (0x00)
#define END_FILE                    (0x01)
#define EXTEND_SEGMENT_ADDRESS      (0x02)
#define START_SEGMENT_ADDRESS	    (0x03)
#define EXTENDED_LINEAR_ADDRESS	    (0x04)
#define START_LINEAR_ADDRESS        (0X05)
#define DATA_PER_LINE               (16)
#define LINES_DISPLAY               (25)
#define BUFFER_SIZE_MAX             (65536)
#define DISPLAYABLE_CHAR_MIN        (31)




//Enum to hanlde return value
typedef enum
{
    SUCCESS = 1,
    ERROR_LINE_FORMAT,
    ERROR_CHECK_SUM,
    ERROR_OPEN_FILE, 
    NOT_HEX_FILE 
}StateCheck_e;

//Declared prototypes
void hex_display_file(int mode, int *buffer, int size); 
StateCheck_e hex_handle_file(char *fileName, int *error_line, int *buffer, int *size);
StateCheck_e hex_check_sum(char *line);
StateCheck_e hex_check_format(char *line);
StateCheck_e hex_check_file_name(char *fileName);
int hex_convert_to_dec(char *data);
void hex_handle_file_error(int state, int error_line, char *fileName, int mode, int *buffer, int size);
int error_checksum_lines[1000] = {0};

int main(int argc, char **argv)
{
    int mode;
    int buffer[BUFFER_SIZE_MAX];
    int error_line, size = 0;
    StateCheck_e result;

    result = hex_handle_file(argv[1], &error_line, buffer, &size);
    hex_handle_file_error(result, error_line, argv[1], mode, buffer,size);

    return 0;
}

//Read from buffer and print hex file 
void hex_display_file(int mode, int* buffer, int size)
{
    char user_input; //Get decide from user
    int address_line = 0;
    int state = 1;
    int count = 0;
    
    //Print intel HEX format file from buffer
    do
    {
        for(int i = count * LINES_DISPLAY; i < (count * LINES_DISPLAY + LINES_DISPLAY); i++)
        {   
            //checksum
            if(error_checksum_lines[i+1] == 1) {
                printf("Checksum error on line %d\n", i + 1);
                address_line += 0x10;
                continue;
            }
            //print data address
            printf("%08X   ", address_line);
            address_line += 0x10;

            //Print data in hexa form
            for(int j = 0; j < DATA_PER_LINE; j++)
            {
                printf("%02X ", buffer[DATA_PER_LINE* i + j]);
            }
            printf("  ");

            //print data in ASCII form
            for(int k = 0; k < DATA_PER_LINE; k++)
            {
                if(buffer[DATA_PER_LINE* i + k] >= DISPLAYABLE_CHAR_MIN)
                {
                    printf("%c", buffer[DATA_PER_LINE* i + k]);
                }
                else
                {
                    printf(".");
                }
                if(k == DATA_PER_LINE - 1)
                {
                    printf("\n");
                }
            }
        }
        //Ask user if want to print out 25 lines more
        do
        {
            printf("Continue (Y/N)?\n");
            scanf("%s", &user_input);
            if(toupper(user_input) == 'Y')
            {
                system("cls");
                state = 1;
            }
            else if(toupper(user_input) == 'N')
            {
                printf("Done");
                state = 0;
            }
        } while((toupper(user_input) != 'Y') && (toupper(user_input) != 'N'));
        count++;
    }while(state == 1);
}

//Handle errors can occur
StateCheck_e hex_handle_file(char *fileName, int *error_line, int *buffer, int *size)
{
    FILE *file_ptr;
    char word_buffer[200];      //store data readed from file
    int line_num = 0;           //represent the current line
    int byte_count = 0;  
    int address = 0;
    int record_type = 0;        //value for store record type
    int buf_size = 0;
    int end_file = 0;           //state value for end file record type

    //Handle file open
    file_ptr = fopen(fileName, "r");
    if(file_ptr == NULL)
    {
        return ERROR_OPEN_FILE;
    }

    //check file extension to define if the file is Hex file
    if(hex_check_file_name(fileName) == NOT_HEX_FILE)
    {
        return NOT_HEX_FILE;
    }
    //check file format
    while((fgets(word_buffer, 200, file_ptr) != NULL) && (end_file == 0))
    {

        line_num++;
        if(hex_check_format(word_buffer) == ERROR_LINE_FORMAT)
        {
            *error_line = line_num;
            return ERROR_LINE_FORMAT;
        }
        else if(hex_check_sum(word_buffer) == ERROR_CHECK_SUM)
        {
            error_checksum_lines[line_num] = 1;
        }
        else
        {
            //Define struct data format
            char byte_count_arr[3];
            strncpy(byte_count_arr, word_buffer + 1, 2);
            byte_count_arr[2] = '\0';
            byte_count = hex_convert_to_dec(byte_count_arr); //check byte

            char address_arr[5];
            strncpy(address_arr, word_buffer + 3, 4);
            address_arr[4] = '\0';
            address = hex_convert_to_dec(address_arr); //check address

            char record_arr[3];
            strncpy(record_arr, word_buffer + 7, 2);
            record_arr[2] = '\0';
            record_type = hex_convert_to_dec(record_arr); // check record_type

            //Test record type case
            if(record_type == START_SEGMENT_ADDRESS	)
            {
                //Do nothing
            }

            if(record_type == EXTENDED_LINEAR_ADDRESS) 
            {
                //Do nothing
            } 

            if(record_type == START_LINEAR_ADDRESS) 
            {
                //Do nothing
            } 

            if(record_type == DATA)
            {
                    int len = buf_size;
                    for(int j = 0; j < byte_count; j++)
                    {
                        char arr[2];
                        strncpy(arr, word_buffer + 9 + (j * 2), 2);
                        arr[2] = '\0';
                        buffer[len + j] = (int)hex_convert_to_dec(arr);
                        buf_size++;
                    }

            }

            if(record_type == END_FILE)
            {
                end_file = 1;
                break;
            }

            if(record_type == EXTEND_SEGMENT_ADDRESS)
            {
                //Do nothing
            }         
        }
        memset(word_buffer, '0', sizeof(word_buffer));
    }
    if(buf_size < BUFFER_SIZE_MAX)
    {
        for (int m = buf_size; m < BUFFER_SIZE_MAX; m++)
        {
            buffer[m] = 0xFF;
            buf_size++;
        } 
    }
    *size = buf_size;
    fclose(file_ptr);
    return SUCCESS;
}

StateCheck_e hex_check_sum(char *word_buffer)
{
    int sum = 0;
    int y = 0;
    for(y = 1; y < strlen(word_buffer) - 3; y += 2)
    {
        char data[3];
        strncpy(data, word_buffer + y, 2);
        data[2] = '\0';
        sum += hex_convert_to_dec(data);
    }
    char sum_byte[3];
    strncpy(sum_byte, word_buffer + strlen(word_buffer) - 3, 2);
    sum_byte[2] = '\0';
    sum = ((~sum) + 0x01) & 0xFF;
    if(sum == hex_convert_to_dec(sum_byte))
    {
        return SUCCESS;
    }
    return ERROR_CHECK_SUM;
}

StateCheck_e hex_check_file_name(char *fileName)
{
    char *name = strrchr(fileName, '.');
    if(strcmp(name, ".hex"))
    {
        return NOT_HEX_FILE;
    }
    else
        return SUCCESS;
}

StateCheck_e hex_check_format(char *word_buffer)
{
    if(word_buffer[0] != ':') {
        return ERROR_LINE_FORMAT;
    }
    for(int i = 0; i < strlen(word_buffer); i++)
    {
        if(!(((word_buffer[i] >= '0') && (word_buffer[i] <= '9')) || ((word_buffer[i] >= 'A') && (word_buffer[i] <= 'F')) || ((word_buffer[i] >= 'a') && (word_buffer[i] <= 'f')) ||(word_buffer[0] == ':') || (word_buffer[i] =='\n')))
        {
            return ERROR_LINE_FORMAT;
        }
    }
    return SUCCESS;
}

int hex_convert_to_dec(char *data)
{
    int sum = 0;
    int haftbyte;
    int leng = strlen(data);
    for(int k = 0; k < strlen(data); k++)
    {
        haftbyte = toupper(data[k]) - '0';
        if(haftbyte > 9)
        {
            haftbyte = haftbyte - 7;
        }
        sum += haftbyte * (int)pow(DATA_PER_LINE, strlen(data) - k - 1);
    }
    return sum;
}

void hex_handle_file_error(int state, int error_line, char *fileName, int mode, int *buffer, int size)
{
    switch (state)
    {
        case (NOT_HEX_FILE):
        {
            printf("%s is not Hex file", fileName);
            break;
        }    
        
        case (ERROR_OPEN_FILE):
        {
            printf("%s could not be opened", fileName);
            break;
        }
        case (ERROR_LINE_FORMAT):
        {
            printf("Format error on line %d", error_line);
            break;
        }
        default:
            hex_display_file(mode, buffer, size);
            break;
    }
}