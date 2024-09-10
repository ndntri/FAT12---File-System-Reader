/**
 * @file: main.c
 * @brief Main Program File
 * @details This program is designed to interact with the FAT file system. It includes functions to print directory lists,
 *               get user input, print error messages, and a main function that initializes the FAT file system, reads directories and files,
 *               and handles user input. The program continuously prompts the user for input to navigate directories or exit the program.
 *               It also manages memory allocation for the buffer and handles errors by calling an error callback function. Upon exiting,
 *               it deallocates any allocated memory and de-initializes the FAT file system.
 *
 * @author: Nguyen Dang Nhu Tri
 * @version: 1.0
 * @date: 2024/05/12
 *
 * @copyright: Copyright (c) 2024
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "FATfs.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 * Variables
 ******************************************************************************/

static uint8_t exit_program = 0;
/* A flag to indicate if the program should exit. 0 means the program continues, 1 means the program should exit. */

static uint16_t serial_number = 1;
/* A counter for the serial number of directory entries. */

static DirList *head_DirList = NULL;
/* A pointer to the head of the directory list. */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*
 * @brief Print a directory list.
 * @details This function prints the contents of a directory list, including file and folder names, types, modification and creation dates, and sizes. It formats the output as a table and handles empty directories and hidden files.
 * @param temp_DirList - The head of the directory list to be printed.
 * @returns None. This function outputs the directory list to the console.
 */
void print_Dir_List(DirList *temp_DirList)
{
    /* Variable to store the bit that checks if a file or directory */
    uint8_t bit_checks_file_or_directory = 0;

    printf("\n\t+-----------------------------------------------------------------------------------------------------------+\n");
    printf("\t|                                               MY FLOPPY DISK                                              |\n");
    printf("\t+-----------------------------------------------------------------------------------------------------------+\n");
    printf("\t|  Press |0| to return to the previous directory. Select the Options below to access                        |\n");
    printf("\t+--------+----------------------+------------+-----------------------+-----------------------+--------------+\n");
    printf("\t| Option |         Name         |    Type    |     Date modified     |     Date created      |     Size     | \n");
    printf("\t+--------+----------------------+------------+-----------------------+-----------------------+--------------+\n");

    /* Loop through each node in the directory list */
    while (NULL != temp_DirList)
    {
        /* Check if the directory is root directory */
        if ('.' != temp_DirList->data.File_name[0])
        {
            /* Print the serial number and the file name and extension of the directory */
            printf("\t|%4d.   |", serial_number);
            printf("%*s", 5, "");
            printf("%s %s", temp_DirList->data.File_name, temp_DirList->data.Extension);
            printf("%*s|", 5, "");

            /* Get the bit that checks is a file or directory */
            bit_checks_file_or_directory = (temp_DirList->data.Attributes >> 4) & 1;

            /* if the directory is a file */
            if (0 == bit_checks_file_or_directory)
            {
                printf("   File     |");
            }
            /* if the directory is a Folder */
            else
            {
                printf("   Folder   |");
            }

            /* Check if the last write date of the directory is not empty */
            if (0 != (temp_DirList->data.Last_Write_Date & 0x1F))
            {
                /* Print the last write time and date of the directory */
                if (12 < ((temp_DirList->data.Last_Write_Time) >> 11))
                {
                    printf(" %.2d:%.2d PM ", ((temp_DirList->data.Last_Write_Time) >> 11), (((temp_DirList->data.Last_Write_Time) >> 5) & 0x3F));
                }
                else
                {
                    printf(" %.2d:%.2d AM ", ((temp_DirList->data.Last_Write_Time) >> 11), (((temp_DirList->data.Last_Write_Time) >> 5) & 0x3F));
                }
                printf(" %d/%.2d/%.2d  |", (((temp_DirList->data.Last_Write_Date) >> 9) + 1980), (((temp_DirList->data.Last_Write_Date) >> 5) & 0x0F), (temp_DirList->data.Last_Write_Date & 0x1F));
            }
            else
            {
                /* Print an empty space if the last write date of the directory is empty */
                printf("%*s|", 23, "");
            }

            /* Check if the creation date of the directory is not empty */
            if (0 != (temp_DirList->data.Creation_Date & 0x1F))
            {
                /* Print the creation time and date of the directory */
                if (12 < ((temp_DirList->data.Creation_Time) >> 11))
                {
                    printf(" %.2d:%.2d PM ", ((temp_DirList->data.Creation_Time) >> 11), (((temp_DirList->data.Creation_Time) >> 5) & 0x3F));
                }
                else
                {
                    printf(" %.2d:%.2d AM ", ((temp_DirList->data.Creation_Time) >> 11), (((temp_DirList->data.Creation_Time) >> 5) & 0x3F));
                }
                printf(" %d/%.2d/%.2d  |", (((temp_DirList->data.Creation_Date) >> 9) + 1980), (((temp_DirList->data.Creation_Date) >> 5) & 0x0F), (temp_DirList->data.Creation_Date & 0x1F));
            }
            else
            {
                /* Print an empty space if the creation date of the directory is empty */
                printf("%*s|", 23, "");
            }

            /* Check if the directory is a file */
            if (0 == bit_checks_file_or_directory)
            {
                /* Print the size of the file */
                if (1000 > temp_DirList->data.File_Size_in_bytes)
                {
                    printf(" %5u byte   |\n", temp_DirList->data.File_Size_in_bytes);
                }
                else if (1000000 > temp_DirList->data.File_Size_in_bytes)
                {
                    printf("%8.2f KB   |\n", temp_DirList->data.File_Size_in_bytes / 1000.0);
                }
                else if (1000000000 > temp_DirList->data.File_Size_in_bytes)
                {
                    printf("%8.2f MB   |\n", temp_DirList->data.File_Size_in_bytes / 1000000.0);
                }
                else
                {
                    /* Do nothing */
                }
            }
            else
            {
                /* Print an empty space if the directory is not a file */
                printf("%*s|\n", 14, "");
            }

            /* Increment the serial number */
            serial_number++;
        }
        else
        {
            /* Do nothing */
        }

        /* Move to the next directory in the list */
        temp_DirList = temp_DirList->next;
    }

    /* Set the temporary directory list node to the head of the directory list */
    temp_DirList = head_DirList;

    /* Check if the directory list is empty and the directory is not a hidden file or directory */
    if (1 == serial_number && 1 == ((temp_DirList->data.Attributes >> 4) & 1))
    {
        /* Print a message indicating that the directory is empty */
        printf("\t|%*s|\n", 107, "");
        printf("\t|%*s", 43, "");
        printf("This folder is empty.");
        printf("%*s|\n", 43, "");
        printf("\t|%*s|\n", 107, "");
    }
    else
    {
        /* Do nothing */
    }

    /* Print the footer of the directory list */
    printf("\t+-----------------------------------------------------------------------------------------------------------+\n");
    printf("\t|  Press |e| or |E| to exit program.                                                                        |\n");
    printf("\t+-----------------------------------------------------------------------------------------------------------+\n");
    printf("\n");
}

/*
 * @brief Get user input.
 * @details This function prompts the user for an input number within a valid range and validates it. It handles empty, non-numeric, and out-of-range inputs, providing appropriate prompts and error messages.
 * @param serial_number - The maximum valid input number allowed.
 * @returns The user's valid input number or triggers program exit if 'e' or 'E' is entered.
 */
uint16_t get_input(uint16_t serial_number)
{
    char input[15];
    /* Variable to store the user's input */
    uint8_t valid;
    /* Variable to check the validity of the input */
    uint8_t i = 0;
    /* Loop counter */
    uint16_t num = 0;
    /* Variable to store the user's input as a number */
    uint32_t temp = 0;
    /* Temporary variable */
    uint8_t result = 1;
    /* Variable to store the result of the input validation */

    /* Loop until a valid input is received */
    do
    {
        /* Reset the result to 1 (valid) */
        result = 1;
        /* Prompt the user for input */
        printf("\t[OPTION] >> ");
        fgets(input, sizeof(input), stdin);

        /* Check if the user wants to exit the program */
        if ('e' == input[0] || 'E' == input[0])
        {
            exit_program = 1;
        }
        else
        {
            /* Remove the newline character from the end of the input */
            if ('\n' == input[strlen(input) - 1])
            {
                input[strlen(input) - 1] = '\0';
            }
            else
            {
                /* Clear the input buffer */
                while ('\n' != getchar())
                    ;
            }

            /* Check if the input is empty */
            if (0 == strlen(input))
            {
                printf("\n\tThe input is empty. Please enter it again !!!\n\n");
                result = 0;
            }
            /* Check if the input is too long */
            else if (10 < strlen(input))
            {
                printf("\n\tThe input is too long. Please enter it again !!!\n\n");
                result = 0;
            }
            else
            {
                /* Assume the input is valid */
                valid = 1;

                /* Check each character in the input */
                for (i = 0; i < strlen(input); i++)
                {
                    /* If the character is not a digit, the input is invalid */
                    if ('0' > input[i] || '9' < input[i])
                    {
                        valid = 0;
                    }
                    else
                    {
                        /* Do nothing*/
                    }
                }

                /* If the input is invalid, print an error message */
                if (!valid)
                {
                    printf("\n\tThe input contains special characters, not numbers. Please enter it again !!!\n\n");
                    result = 0;
                }
                else
                {
                    /* Convert the input to a number */
                    temp = atoi(input);

                    /* Check if the input is out of range */
                    if (temp > UINT16_MAX)
                    {
                        printf("\n\tInput out of range. Please enter it again !!!\n\n");
                        result = 0;
                    }
                    else
                    {
                        /* Store the input as a number */
                        num = temp;

                        /* Check if the input is greater than or equal to the serial number */
                        if (num >= serial_number)
                        {
                            printf("\n\tInput out of range. Please enter it again !!!\n\n");
                            result = 0;
                        }
                        else
                        {
                            /* Do nothing*/
                        }
                    }
                }
            }
        }

        /* Continue looping until a valid input is received or the user wants to exit the program */
    } while (0 == result && 0 == exit_program);

    /* Return the user's input */
    return num;
}

/*
 * @brief Print an error message.
 * @details This function prints a specific error message based on the provided error code. It covers various error scenarios such as file opening, boot sector reading, memory allocation, sector size updating, and directory reading errors.
 * @param err - The error code that determines the message to be printed.
 * @returns None. This function outputs error messages to the console.
 */
void print_error(ERROR_CODE err)
{
    /* Switch on the error code */
    switch (err)
    {
    /* If the error is opening the file */
    case ERROR_OPENING_FILE:
    {
        printf(" Failed to open file!\n");
        break;
    }
    /* If the error is reading the boot sector */
    case BOOT_SECTOR_READ_ERROR:
    {
        printf("Failed to read boot sector!\n");
        break;
    }
    /* If the error is memory allocation */
    case DYNAMIC_ALLOCATON_ERROR:
    {
        printf("Memory allocation failed!\n");
        break;
    }
    /* If the error is updating the sector size */
    case ERROR_UPDATING_SECTOR_SIZE:
    {
        printf("Failed to update sector size!\n");
        break;
    }
    /* If the error is reading multiple sectors */
    case MULTIPLE_SECTOR_READ_ERROR:
    {
        printf("Failed to read multiple sectors\n");
        break;
    }
    /* If the error is invalid cluster size */
    case CLUSTER_SIZE_ERROR:
    {
        printf("The cluster size is invalid !\n");
        break;
    }
    /* If the error is reading the root directory */
    case ERROR_READING_ROOT_DIRECTORY:
    {
        printf("Failed to read root directory !\n");
        break;
    }
    /* If the error is reading a subdirectory */
    case ERROR_READING_SUB_DIRECTORY:
    {
        printf("Failed to read Subdirectory !\n");
        break;
    }
    }
}

/*
 * @brief Main function to initialize the FAT file system, read directories and files, and handle user input.
 * @details This function initializes the FAT file system, reads directories and files, and handles user input.
 *               It continuously prompts the user for input to navigate directories or exit the program.
 *               It also manages memory allocation for the buffer and handles errors by calling an error callback function.
 *               Upon exiting, it deallocates any allocated memory and de-initializes the FAT file system.
 * @param None.
 * @returns Returns 0 upon successful execution.
 */
int main(void)
{
    uint32_t Cluster_size = 0;
    /* Variable to store the cluster size */
    uint8_t bit_checks_file_or_directory = 0;
    /* Variable to check if it's a file or directory */
    uint16_t First_Logical_Cluster_of_choice = 0;
    /* Variable to store the first logical cluster of choice */
    uint16_t count = 1;
    /* Variable to store the count */
    uint32_t i = 0;
    /* Loop counter */
    static ClusterList *head_cluster_list = NULL;
    /* Variable to store the head of the cluster list */
    ClusterList *temp_cluster_list = NULL;
    /* Temporary variable for a cluster list node */
    DirList *temp_DirList = NULL;
    /* Temporary variable for a directory list node */
    uint16_t choice = 0;
    /* Variable to store the user's choice */

    /* Initialize the FAT file system with the given path and error callback function */
    Cluster_size = fatfs_init("D:/floppy.img", print_error);

    /* Check if the FAT file system was initialized successfully */
    if (0 != Cluster_size)
    {
        /* Read the root directory at the first logical cluster of choice */
        head_DirList = fatfs_read_dir(First_Logical_Cluster_of_choice);
        /* Loop until the user chooses to exit the program */
        do
        {
            /* Set the temporary directory list node to the head of the directory list */
            temp_DirList = head_DirList;
            print_Dir_List(temp_DirList);
            temp_DirList = head_DirList;
            /* Get the user's choice */
            choice = get_input(serial_number);

            /* Check if the user chose to exit the program */
            if (1 != exit_program)
            {
                /* Check if the user chose the root directory and it's not a hidden file or directory */
                if (0 == choice && '.' != temp_DirList->data.File_name[0])
                {
                    /* Deallocate the directory list */
                    deallocate_Dir_List(head_DirList);
                    head_DirList = NULL;
                    /* Read the root directory */
                    head_DirList = fatfs_read_dir(0);
                }
                else
                {
                    /* Check if the user chose the root directory */
                    if (0 == choice)
                    {
                        printf("\n\tFile name: %s %s\n\n", temp_DirList->data.File_name, temp_DirList->data.Extension);
                        /* Get the first logical cluster of the chosen directory */
                        First_Logical_Cluster_of_choice = head_DirList->data.First_Logical_Cluster;
                        /* Deallocate the directory list */
                        deallocate_Dir_List(head_DirList);
                        head_DirList = NULL;
                        /* Read the directory at the first logical cluster of choice */
                        head_DirList = fatfs_read_dir(First_Logical_Cluster_of_choice);
                        /* Reset the count and serial number */
                        count = 1;
                        serial_number = 1;
                    }
                    else
                    {
                        /* Check if the chosen directory is a hidden file or directory */
                        if ('.' == temp_DirList->data.File_name[0])
                        {
                            /* Increment the user's choice */
                            choice++;
                        }
                        else
                        {
                            /* Do nothing */
                        }

                        /* Set the temporary directory list node to the head of the directory list */
                        temp_DirList = head_DirList;

                        /* Loop until the chosen directory is reached */
                        while (NULL != temp_DirList && count < choice)
                        {
                            /* Move to the next directory in the list */
                            temp_DirList = temp_DirList->next;
                            /* Increment the count */
                            count++;
                        }

                        printf("\n\tFile name: %s %s\n\n", temp_DirList->data.File_name, temp_DirList->data.Extension);
                        /* Get the bit that checks if the chosen directory is a file or directory */
                        bit_checks_file_or_directory = (temp_DirList->data.Attributes >> 4) & 1;
                        /* Get the first logical cluster of the chosen directory */
                        First_Logical_Cluster_of_choice = temp_DirList->data.First_Logical_Cluster;

                        /* Check if the chosen directory is a file */
                        if (0 == bit_checks_file_or_directory)
                        {
                            /* Read the file at the first logical cluster of choice */
                            head_cluster_list = fatfs_read_file(First_Logical_Cluster_of_choice);
                            /* Set the temporary cluster list node to the head of the cluster list */
                            temp_cluster_list = head_cluster_list;

                            printf("\t");
                            while (NULL != temp_cluster_list)
                            {
                                for (i = 0; i < Cluster_size; i++)
                                {
                                    printf("%c", temp_cluster_list->data_in_cluster[i]);
                                }

                                /* Move to the next cluster in the list */
                                temp_cluster_list = temp_cluster_list->next;
                            }

                            /* Deallocate the cluster list */
                            deallocate_Cluster_List(head_cluster_list);
                            /* Reset the head of the cluster list */
                            head_cluster_list = NULL;
                        }
                        else
                        {
                            /* Deallocate the directory list */
                            deallocate_Dir_List(head_DirList);
                            /* Reset the head of the directory list */
                            head_DirList = NULL;
                            /* Read the directory at the first logical cluster of choice */
                            head_DirList = fatfs_read_dir(First_Logical_Cluster_of_choice);
                        }

                        /* Reset the count and serial number */
                        count = 1;
                        serial_number = 1;
                    }
                }
                printf("\n");
            }
            else
            {
                /* Deallocate the directory list */
                deallocate_Dir_List(head_DirList);
                /* De-initialize the FAT file system */
                fatfs_de_init();
                /* Print a message indicating that the program has exited */
                printf("\n\tThe program has exited. Thank you for using :) \n");
            }

            /* Reset the serial number */
            serial_number = 1;

            /* Continue looping until the user chooses to exit the program */
        } while (1 != exit_program);
    }
    else
    {
        /* Print an error message if the FAT file system was not initialized successfully */
        print_error(CLUSTER_SIZE_ERROR);
    }

    return 0;
}
