/**
 * @file: FATfs.h
 * @brief Header File for FAT File System
 * @details This header file contains the function prototypes, data structures, and type definitions used in the FAT file system program.
 *               It includes function prototypes for initializing the FAT file system, reading directories and files, deallocating directory and cluster lists,
 *               and de-initializing the FAT file system. The data structures defined in this file are used to manage directories, files, and clusters in the FAT file system.
 *               It also includes an enumeration of error codes for different error scenarios and a typedef for an error callback function.
 *
 * @author: Nguyen Dang Nhu Tri
 * @version: 1.0
 * @date: 2024/05/12
 *
 * @copyright: Copyright (c) 2024
 */

#ifndef FATFS_H
#define FATFS_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "HAL.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*
 * @brief Structure representing the boot sector of a FAT file system.
 * @details This structure contains various parameters of the boot sector such as bytes per sector, sectors per cluster, number of FATs,
 *                maximum number of root directory entries, total sector count, and sectors per FAT.
 */
typedef struct fatfs_bootsector_struct_t
{
    uint16_t bytes_per_sector;                         /* The number of bytes in each sector. */
    uint16_t sectors_per_cluster;                      /* The number of sectors in each cluster. */
    uint8_t Number_of_FATs;                            /* The number of File Allocation Tables (FATs). */
    uint16_t Maximum_number_of_root_directory_entries; /* The maximum number of entries in the root directory. */
    uint16_t Total_sector_count;                       /* The total number of sectors in the file system. */
    uint16_t Sectors_per_FAT;                          /* The number of sectors per File Allocation Table (FAT). */
} fatfs_bootsector_struct_t;

/*
 * @brief Structure representing a directory entry in a FAT file system.
 * @details This structure contains various parameters of a directory entry such as file name, extension, attributes,
 *                creation time and date, last write time and date, first logical cluster, and file size in bytes.
 */
typedef struct fatfs_directory_entry_list_struct_t
{
    char File_name[9];              /* The name of the file. */
    char Extension[5];              /* The file extension. */
    uint8_t Attributes;             /* The file attributes. */
    uint16_t Creation_Time;         /* The time the file was created. */
    uint16_t Creation_Date;         /* The date the file was created. */
    uint16_t Last_Write_Time;       /* The last time the file was written to. */
    uint16_t Last_Write_Date;       /* The last date the file was written to. */
    uint16_t First_Logical_Cluster; /* The first logical cluster of the file. */
    uint64_t File_Size_in_bytes;    /* The size of the file in bytes. */
} fatfs_directory_entry_list_struct_t;

/*
 * @brief Structure representing a node in a directory list.
 * @details This structure contains a directory entry data and a pointer to the next node in the directory list.
 */
typedef struct DirList
{
    fatfs_directory_entry_list_struct_t data; /* The directory entry data. */
    struct DirList *next;                     /* Pointer to the next node in the directory list. */
} DirList;

/*
 * @brief Structure representing a node in a cluster list.
 * @details This structure contains a pointer to the data in a cluster and a pointer to the next node in the cluster list.
 */
typedef struct ClusterList
{
    uint8_t *data_in_cluster; /* Pointer to the data in a cluster. */
    struct ClusterList *next; /* Pointer to the next node in the cluster list. */
} ClusterList;

/*
 * @brief Enumeration of error codes.
 * @details This enumeration defines various error codes for different error scenarios such as file opening,
                  boot sector reading, memory allocation, sector size updating, and directory reading errors. */
typedef enum ERROR_CODE
{
    ERROR_OPENING_FILE,
    BOOT_SECTOR_READ_ERROR,
    DYNAMIC_ALLOCATON_ERROR,
    ERROR_UPDATING_SECTOR_SIZE,
    MULTIPLE_SECTOR_READ_ERROR,
    CLUSTER_SIZE_ERROR,
    ERROR_READING_ROOT_DIRECTORY,
    ERROR_READING_SUB_DIRECTORY,
} ERROR_CODE;

/*
 * @brief Typedef for an error callback function.
 * @details This typedef defines a function pointer type for an error callback function that takes an error code as a parameter.
 */

typedef void (*ErrorCallback)(ERROR_CODE);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*
 * @brief Initialize the FAT file system.
 * @details This function initializes the FAT file system. It sets an error callback, initializes the KMC with the provided path,
 *               reads and parses the boot sector, updates the sector size, retrieves the FAT table, and calculates various parameters.
 *               It handles errors by calling the error callback.
 * @param path - The path to the file system to be initialized.
 * @param callback - The callback function for error handling.
 * @returns Returns the size of the cluster.
 */
uint32_t fatfs_init(const char *path, ErrorCallback callback);

/*
 * @brief Read a directory from the FAT file system.
 * @details This function reads a directory from a FAT file system. It handles both root and subdirectories,
 *               reading directory entries into a buffer, validating them, and adding them to a directory list.
 *               It also manages memory allocation for the buffer and handles errors by calling an error callback function.
 * @param First_Logical_Directory_of_current - The first logical directory of the current directory.
 * @returns Returns a pointer to the head of the directory list.
 */
DirList *fatfs_read_dir(uint16_t First_Logical_Cluster_of_choice);

/*
 * @brief Read a file from the FAT file system.
 * @details This function reads a file's data into a linked list of clusters. It allocates memory for reading file data,
 *               iterates over the file's clusters, and constructs a linked list with the data until the end of the file is reached.
 * @param First_Logical_Cluster_of_current - The starting cluster of the file.
 * @returns A pointer to the first node in the linked list of file data clusters.
 */
ClusterList *fatfs_read_file(uint16_t First_Logical_Cluster_of_choice);

/*
 * @brief Deallocate a directory list.
 * @details This function traverses a linked list of directory entries and deallocates each node to free memory.
 * @param head - The starting node of the directory list.
 * @returns None. This function performs memory deallocation.
 */
void deallocate_Dir_List(DirList *head);

/*
 * @brief Deallocate a cluster list.
 * @details This function frees the memory allocated for a linked list of clusters, including the data within each cluster.
 * @param head - The starting node of the cluster list to be freed.
 * @returns None. This function is used for memory cleanup.
 */
void deallocate_Cluster_List(ClusterList *head);

/*
 * @brief De-initialize the FAT file system.
 * @details This function performs cleanup for the FAT file system by deallocating the FAT table and de-initializing the KMC.
 * @param None.
 * @returns None. This function is used for cleanup and does not return a value.
 */
void fatfs_de_init(void);

#endif /* FATFS_H */
