/**
 * @file: FATfs.c
 * @brief Main Program File
 * @Description: This program is designed to interact with the FAT file system. It includes functions to initialize the FAT file system,
 *               retrieve the FAT table, retrieve the next entry in the FAT table, create new node entries for a directory list and a cluster list,
 *               parse the boot sector of a FAT12 file system, read directories and files, deallocate directory and cluster lists,
 *               and de-initialize the FAT file system. The program handles errors by calling an error callback function and manages memory allocation
 *               for buffers and lists. Upon exiting, it deallocates any allocated memory and de-initializes the FAT file system.
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

static fatfs_bootsector_struct_t s_FAT12Infor;
/*This variable is used to store information about the boot sector FAT12*/

static fatfs_directory_entry_list_struct_t s_dirList;
/*This variable is used to store information about the entry lists read from the img file and then returned to the application layer*/

static uint8_t *s_fat_table = NULL;
/*This variable is used to store the FAT table*/

static void (*error_callback)(ERROR_CODE);
/* A function pointer to an error callback function that handles errors based on the provided error code. */

static uint16_t num_cluster_in_root_directory = 0;
/* The number of clusters in the root directory. */

static uint16_t cluster_started_in_physical_of_rootdirectory = 0;
/* The physical start of the root directory in clusters. */

static uint16_t Cluster_starts_in_physical_of_the_data_area = 0;
/* The physical start of the data area in clusters. */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*******************************************************************************
 * Code
 ******************************************************************************/

/*
 *@brief Retrieve the FAT (File Allocation Table).
 *@param None.
 *@returns No return value.
 */
static void get_fat_table(void)
{

    uint32_t number_of_bytes_read_in_FAT = 0;
    /* Variable to store the number of bytes read in FAT */

    s_fat_table = (uint8_t *)calloc((s_FAT12Infor.bytes_per_sector * s_FAT12Infor.Sectors_per_FAT), sizeof(uint8_t));
    /* Allocate memory for the FAT table */

    /* Check if memory allocation was successful */
    if (NULL != s_fat_table)
    {
        /* Read multiple sectors from the FAT into the allocated memory */
        number_of_bytes_read_in_FAT = kmc_read_multi_sector(s_FAT12Infor.bytes_per_sector * s_FAT12Infor.sectors_per_cluster, s_FAT12Infor.Sectors_per_FAT, s_fat_table);

        /* Check if the correct number of bytes were read */
        if (number_of_bytes_read_in_FAT != (s_FAT12Infor.Sectors_per_FAT) * (s_FAT12Infor.bytes_per_sector))
        {
            /* If the incorrect number of bytes were read, call the error callback with the appropriate error code */
            error_callback(MULTIPLE_SECTOR_READ_ERROR);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* If memory allocation failed, call the error callback with the appropriate error code */
        error_callback(DYNAMIC_ALLOCATON_ERROR);
    }
}

/*
 *@brief Retrieve the next entry in the FAT (File Allocation Table).
 *@param n - The index of the current FAT entry.
 *@returns Returns the next FAT entry.
 */
static uint16_t get_fat_entry_next(uint16_t n)
{

    uint16_t entry;
    /* Variable to store the next FAT entry */

    /* Check if the current index is even */
    if (0 == n % 2)
    {
        /* If it is, calculate the next FAT entry using the even index formula */
        entry = (s_fat_table[(3 * n) / 2 + 1] & 0x0F) << 8 | s_fat_table[(3 * n) / 2];
    }
    else
    {
        /* If it's not, calculate the next FAT entry using the odd index formula */
        entry = s_fat_table[(3 * n) / 2 + 1] << 4 | (s_fat_table[(3 * n) / 2] & 0xF0) >> 4;
    }

    return entry;
}

/*
 *@brief Create a new node entry for a directory list.
 *@param None.
 *@returns Returns a pointer to the newly created node.
 */
static DirList *createNodeEntry(void)
{
    /* Allocate memory for the new node */
    DirList *newNode = (DirList *)malloc(sizeof(DirList));

    /* Check if memory allocation was successful */
    if (NULL == newNode)
    {
        /* If memory allocation failed, call the error callback with the appropriate error code */
        error_callback(DYNAMIC_ALLOCATON_ERROR);
    }
    else
    {
        /* If memory allocation was successful, initialize the next pointer of the new node to NULL */
        newNode->next = NULL;
    }

    return newNode;
}

/*
 *@brief Create a new node for a cluster list.
 *@param None.
 *@returns Returns a pointer to the newly created node.
 */
static ClusterList *createNodeCluster(void)
{
    /* Allocate memory for the new node */
    ClusterList *newNode = (ClusterList *)malloc(sizeof(ClusterList));

    /* Check if memory allocation was successful */
    if (NULL == newNode)
    {
        /* If memory allocation failed, call the error callback with the appropriate error code */
        error_callback(DYNAMIC_ALLOCATON_ERROR);
    }
    else
    {
        /* If memory allocation was successful, initialize the next pointer of the new node to NULL */
        newNode->next = NULL;
    }

    return newNode;
}

/*
 *@brief Parse the boot sector of a FAT12 file system.
 *@param buff - The buffer containing the boot sector data.
 *@returns No return value.
 */
static void parse_bootsector(uint8_t *buff)
{
    /* Copy the bytes per sector,sectors per cluster,  number of FATs, maximum number of root directory entries,
       total sector count, sectors per FAT value from the buffer to the FAT12 info structure */
    memcpy(&s_FAT12Infor.bytes_per_sector, &buff[11], 2);
    memcpy(&s_FAT12Infor.sectors_per_cluster, &buff[13], 1);
    memcpy(&s_FAT12Infor.Number_of_FATs, &buff[16], 1);
    memcpy(&s_FAT12Infor.Maximum_number_of_root_directory_entries, &buff[17], 2);
    memcpy(&s_FAT12Infor.Total_sector_count, &buff[19], 2);
    memcpy(&s_FAT12Infor.Sectors_per_FAT, &buff[22], 2);
}

/*
 *@brief Initialize the FAT file system.
 *@param path - The path to the file system.
 *@param callback - The callback function for error handling.
 *@returns Returns the size of the cluster.
 */
uint32_t fatfs_init(const char *path, ErrorCallback callback)
{
    uint32_t Cluster_size = 0;
    /* Variable to store the size of the cluster */
    uint32_t number_of_bytes_read = 0;
    /* Variable to store the number of bytes read */
    uint8_t *buff;
    /* Buffer to store the data read from the file system */

    /* Set the error callback function */
    error_callback = callback;
    /* Initialize the KMC with the given path */
    if (0 != kmc_init(path))
    {
        /* Allocate memory for the buffer */
        buff = (uint8_t *)calloc(KMC_DEFAULT_SECTOR_SIZE, sizeof(uint8_t));

        /* Check if memory allocation was successful */
        if (NULL != buff)
        {
            /* Read the sector at index 0 into the buffer */
            number_of_bytes_read = kmc_read_sector(0, buff);

            /* Check if the correct number of bytes were read */
            if (KMC_DEFAULT_SECTOR_SIZE != number_of_bytes_read)
            {
                /* If reading the boot sector failed, call the error callback with the appropriate error code */
                error_callback(BOOT_SECTOR_READ_ERROR);
            }
            else
            {
                /* Parse the boot sector data from the buffer */
                parse_bootsector(buff);

                /* Free the memory allocated for the buffer */
                free(buff);

                /* Update the sector size for the KMC */
                if (0 != kmc_update_sector_size(s_FAT12Infor.bytes_per_sector))
                {
                    /* Retrieve the FAT table */
                    get_fat_table();

                    /* Calculate the size of the cluster */
                    Cluster_size = s_FAT12Infor.sectors_per_cluster * s_FAT12Infor.bytes_per_sector;
                    /* Calculate the number of clusters in the root directory */
                    num_cluster_in_root_directory = (s_FAT12Infor.Maximum_number_of_root_directory_entries * 32) / s_FAT12Infor.bytes_per_sector;

                    /* Calculate the physical start of the root directory in clusters */
                    cluster_started_in_physical_of_rootdirectory = (s_FAT12Infor.Number_of_FATs * s_FAT12Infor.Sectors_per_FAT) + 1;

                    /* Calculate the physical start of the data area in clusters */
                    Cluster_starts_in_physical_of_the_data_area = s_FAT12Infor.Number_of_FATs * s_FAT12Infor.Sectors_per_FAT + num_cluster_in_root_directory + 1;
                }
                else
                {
                    /* If updating the sector size failed, call the error callback with the appropriate error code */
                    error_callback(ERROR_UPDATING_SECTOR_SIZE);
                }
            }
        }
        else
        {
            /* If memory allocation for the buffer failed, call the error callback with the appropriate error code */
            error_callback(DYNAMIC_ALLOCATON_ERROR);
        }
    }
    else
    {
        /* If initializing the KMC failed, call the error callback with the appropriate error code */
        error_callback(ERROR_OPENING_FILE);
    }

    return Cluster_size;
}

/*
 *@brief Read a directory from the FAT file system.
 *@param First_Logical_Directory_of_current - The first logical directory of the current directory.
 *@returns Returns a pointer to the head of the directory list.
 */
DirList *fatfs_read_dir(uint16_t First_Logical_Directory_of_current)
{
    uint8_t *buff = NULL;
    /* Buffer to store the data read from the file system */
    uint32_t i = 0;
    /* Variables to store the loop counter */
    uint32_t number_of_bytes_read = 0;
    /* Variable to store the number of bytes read */
    DirList *head = NULL;
    /* the head of the cluster list*/
    DirList *tail = NULL;
    /* the tail of the cluster list */
    DirList *newNode = NULL;
    /* the new node of the cluster list */

    /* Check if the first logical directory is the root directory */
    if (0 == First_Logical_Directory_of_current)
    {
        /* Allocate memory for the buffer */
        buff = (uint8_t *)calloc((s_FAT12Infor.bytes_per_sector * s_FAT12Infor.sectors_per_cluster * num_cluster_in_root_directory), sizeof(uint8_t));

        /* Check if memory allocation was successful */
        if (NULL != buff)
        {
            /* Read multiple sectors from the root directory into the buffer */
            number_of_bytes_read = kmc_read_multi_sector((cluster_started_in_physical_of_rootdirectory * s_FAT12Infor.bytes_per_sector), num_cluster_in_root_directory, buff);

            /* Check if the correct number of bytes were read */
            if (number_of_bytes_read == (s_FAT12Infor.bytes_per_sector * num_cluster_in_root_directory))
            {
                /* Loop through each directory entry in the buffer */
                for (i = 0; i < number_of_bytes_read; i += 32)
                {
                    /* Check if the directory entry is valid
                    If the Attributes byte is 0x0F, then this directory entry is part of a long file name and can be ignored for purposes of this assignment.
                    If the first byte of the Filename field is 0x00, then this directory entry is free and all the remaining directory entries in this directory are also free. */
                    if (0 != buff[i] && 0x0F != buff[i + 11])
                    {
                        /* Copy the file name, extension, attributes, creation time and date, last write time and date, first logical cluster, and file size in bytes from the buffer to the directory list structure */
                        memcpy(&s_dirList.File_name, &buff[i], 8);
                        s_dirList.File_name[8] = '\0';
                        memcpy(&s_dirList.Extension, &buff[i + 8], 3);
                        s_dirList.Extension[4] = '\0';
                        memcpy(&s_dirList.Attributes, &buff[i + 11], 1);
                        memcpy(&s_dirList.Creation_Time, &buff[i + 14], 2);
                        memcpy(&s_dirList.Creation_Date, &buff[i + 16], 2);
                        memcpy(&s_dirList.Last_Write_Time, &buff[i + 22], 2);
                        memcpy(&s_dirList.Last_Write_Date, &buff[i + 24], 2);
                        memcpy(&s_dirList.First_Logical_Cluster, &buff[i + 26], 2);
                        memcpy(&s_dirList.File_Size_in_bytes, &buff[i + 28], 4);

                        /* Create a new directory entry node */
                        newNode = createNodeEntry();
                        /* Copy the directory list data to the new node */
                        memcpy(&newNode->data, &s_dirList, sizeof(fatfs_directory_entry_list_struct_t));

                        /* If the directory list is empty, set the head and tail to the new node */
                        if (NULL == head)
                        {
                            head = newNode;
                            tail = newNode;
                        }
                        else
                        {
                            /* If the directory list is not empty, add the new node to the end of the list */
                            tail->next = newNode;
                            tail = newNode;
                        }
                    }
                    /* If the first byte of the Filename field is 0x00, then this directory entry is free and all the remaining directory entries in this directory are also free. */
                    else if (0 == buff[i])
                    {
                        i = number_of_bytes_read;
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }

                /* Free the memory allocated for the buffer */
                free(buff);
                buff = NULL;
            }
            else
            {
                /* If reading the root directory failed, call the error callback with the appropriate error code */
                error_callback(ERROR_READING_ROOT_DIRECTORY);
            }
        }
        else
        {
            /* If memory allocation for the buffer failed, call the error callback with the appropriate error code */
            error_callback(DYNAMIC_ALLOCATON_ERROR);
        }
    }
    else
    {
        /* Allocate memory for the buffer */
        buff = (uint8_t *)calloc(s_FAT12Infor.sectors_per_cluster * s_FAT12Infor.bytes_per_sector, sizeof(uint8_t));

        /* Check if memory allocation was successful */
        if (NULL != buff)
        {
            /* Loop until the end of the FAT is reached */
            do
            {
                /* Read multiple sectors from the subdirectory into the buffer */
                number_of_bytes_read = kmc_read_multi_sector(((Cluster_starts_in_physical_of_the_data_area - 2 + First_Logical_Directory_of_current) * s_FAT12Infor.bytes_per_sector), s_FAT12Infor.sectors_per_cluster, buff);

                /* Check if the correct number of bytes were read */
                if (number_of_bytes_read == (s_FAT12Infor.sectors_per_cluster * s_FAT12Infor.bytes_per_sector))
                {
                    /* Loop through each directory entry in the buffer */
                    for (i = 0; i < number_of_bytes_read; i += 32)
                    {
                        /* Check if the directory entry is valid
                        If the Attributes byte is 0x0F, then this directory entry is part of a long file name and can be ignored for purposes of this assignment.
                        If the first byte of the Filename field is 0x00, then this directory entry is free and all the remaining directory entries in this directory are also free. */
                        if (0 != buff[i] && 0x0F != buff[i + 11])
                        {
                            /* Copy the file name, extension, attributes, creation time and date, last write time and date, first logical cluster,
                             and file size in bytes from the buffer to the directory list structure */
                            memcpy(&s_dirList.File_name, &buff[i], 8);
                            s_dirList.File_name[8] = '\0';
                            memcpy(&s_dirList.Extension, &buff[i + 8], 3);
                            s_dirList.Extension[4] = '\0';
                            memcpy(&s_dirList.Attributes, &buff[i + 11], 1);
                            memcpy(&s_dirList.Creation_Time, &buff[i + 14], 2);
                            memcpy(&s_dirList.Creation_Date, &buff[i + 16], 2);
                            memcpy(&s_dirList.Last_Write_Time, &buff[i + 22], 2);
                            memcpy(&s_dirList.Last_Write_Date, &buff[i + 24], 2);
                            memcpy(&s_dirList.First_Logical_Cluster, &buff[i + 26], 2);
                            memcpy(&s_dirList.File_Size_in_bytes, &buff[i + 28], 4);

                            /* Ignore if entry points to current directory */
                            if (First_Logical_Directory_of_current != s_dirList.First_Logical_Cluster)
                            {
                                /* Create a new directory entry node */
                                newNode = createNodeEntry();
                                /* Copy the directory list data to the new node */
                                memcpy(&newNode->data, &s_dirList, sizeof(fatfs_directory_entry_list_struct_t));

                                /* If the directory list is empty, set the head and tail to the new node */
                                if (NULL == head)
                                {
                                    head = newNode;
                                    tail = newNode;
                                }
                                else
                                {
                                    /* If the directory list is not empty, add the new node to the end of the list */
                                    tail->next = newNode;
                                    tail = newNode;
                                }
                            }
                            else
                            {
                                /* Do nothing*/
                            }
                        }
                        /* If the first byte of the Filename field is 0x00, then this directory entry is free and all the remaining directory entries in this directory are also free. */
                        else if (0 == buff[i])
                        {
                            i = number_of_bytes_read;
                        }
                        else
                        {
                            /* Do nothing */
                        }
                    }

                    /* Get the next FAT entry */
                    First_Logical_Directory_of_current = get_fat_entry_next(First_Logical_Directory_of_current);
                    /* Reset the buffer */
                    buff = NULL;
                }
                else
                {
                    /* If reading the subdirectory failed, call the error callback with the appropriate error code */
                    error_callback(ERROR_READING_SUB_DIRECTORY);
                }

                /* Continue looping until the end of the FAT is reached */
            } while (0xFF7 > First_Logical_Directory_of_current);

            /* Free the memory allocated for the buffer */
            free(buff);
        }
        else
        {
            /* If memory allocation for the buffer failed, call the error callback with the appropriate error code */
            error_callback(DYNAMIC_ALLOCATON_ERROR);
        }
    }

    /* Return the head of the directory list */
    return head;
}

/*
 *@brief Read a file from the FAT file system.
 *@param First_Logical_Cluster_of_current - The first logical cluster of the current file.
 *@returns Returns a pointer to the head of the cluster list.
 */
ClusterList *fatfs_read_file(uint16_t First_Logical_Cluster_of_current)
{
    uint16_t fat_entry_next = 0;
    /* Variables to store the next FAT entry */
    uint8_t *buff = NULL;
    /* Buffer to store the data read from the file system */
    uint32_t number_of_bytes_read = 0;
    /* Variable to store the number of bytes read */
    ClusterList *head_cluster_list = NULL;
    /* the head of the cluster list*/
    ClusterList *tail_cluster_list = NULL;
    /* the tail of the cluster list */
    ClusterList *newNode = NULL;
    /* the new node of the cluster list */

    /* Loop until the end of the FAT is reached */
    do
    {
        /* Allocate memory for the buffer, buff will be freed when the linked list is deallocated,
        specifically, it will be freed in the deallocate_Cluster_List function  */
        buff = (uint8_t *)calloc(s_FAT12Infor.sectors_per_cluster * s_FAT12Infor.bytes_per_sector, sizeof(uint8_t));

        /* Check if memory allocation was successful */
        if (NULL != buff)
        {
            /* Read multiple sectors from the file into the buffer */
            number_of_bytes_read = kmc_read_multi_sector(((Cluster_starts_in_physical_of_the_data_area - 2 + First_Logical_Cluster_of_current) * s_FAT12Infor.bytes_per_sector), s_FAT12Infor.sectors_per_cluster, buff);

            /* Check if the correct number of bytes were read */
            if (number_of_bytes_read == (s_FAT12Infor.bytes_per_sector * s_FAT12Infor.sectors_per_cluster))
            {
                /* Create a new cluster node */
                newNode = createNodeCluster();

                /* Assign the data in the buffer to the new node */
                newNode->data_in_cluster = buff;

                /* If the cluster list is empty, set the head and tail to the new node */
                if (NULL == head_cluster_list)
                {
                    head_cluster_list = newNode;
                    tail_cluster_list = newNode;
                }
                else
                {
                    /* If the cluster list is not empty, add the new node to the end of the list */
                    tail_cluster_list->next = newNode;
                    tail_cluster_list = newNode;
                }

                /* Get the next FAT entry */
                First_Logical_Cluster_of_current = get_fat_entry_next(First_Logical_Cluster_of_current);
            }
            else
            {
                /* If the incorrect number of bytes were read, do nothing */
            }
        }
        else
        {
            /* If memory allocation for the buffer failed, call the error callback with the appropriate error code */
            error_callback(DYNAMIC_ALLOCATON_ERROR);
        }

        /* Continue looping until the end of the FAT is reached */
    } while (0xFF7 > First_Logical_Cluster_of_current);

    return head_cluster_list;
}

/*
 *@brief Deallocate a directory list.
 *@param head - The head of the directory list to be deallocated.
 *@returns No return value.
 */
void deallocate_Dir_List(DirList *head)
{
    /* Temporary variable to store the node to be deallocated */
    DirList *tmp;

    /* Loop through each node in the list */
    while (NULL != head)
    {
        /* Store the current node in the temporary variable */
        tmp = head;
        /* Move the head to the next node */
        head = head->next;
        /* Deallocate the current node */
        free(tmp);
    }
}

/*
 *@brief Deallocate a cluster list.
 *@param head - The head of the cluster list to be deallocated.
 *@returns No return value.
 */
void deallocate_Cluster_List(ClusterList *head)
{
    /* Temporary variable to store the node to be deallocated */
    ClusterList *tmp;

    /* Loop through each node in the list */
    while (NULL != head)
    {
        /* Store the current node in the temporary variable */
        tmp = head;
        /* Move the head to the next node */
        head = head->next;
        /* Deallocate the data in the cluster */
        free(tmp->data_in_cluster);
        /* Deallocate the current node */
        free(tmp);
    }
}

/*
 *@brief De-initialize the FAT file system.
 *@param None.
 *@returns No return value.
 */
void fatfs_de_init(void)
{
    /* Deallocate the FAT table */
    free(s_fat_table);
    /* De-initialize the KMC */
    kmc_de_init();
}
