/**
 * @file: HAL.c
 * @brief Main Program File
 * @Description: This program is designed to interact with the KMC system. It includes functions to initialize the KMC system with a specific file path,
 *               update the sector size for KMC, read a sector from KMC, read multiple sectors from KMC, and de-initialize the KMC system. The program handles errors
 *               by checking the success of file opening and sector size updating operations. It manages memory allocation for buffers and ensures that any open file
 *               is properly closed to prevent data loss or corruption.
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
#include "HAL.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 * Variables
 ******************************************************************************/

static FILE *s_fptr = NULL;
/*File pointer points to file img*/

static uint16_t s_sectorSize = 0;
/* Contains sector size*/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*******************************************************************************
 * Code
 ******************************************************************************/

/*
 *@brief Initialize KMC with a specific file path.
 *@param path - Path to the file to be opened.
 *@returns Returns 1 if the file opens successfully, 0 otherwise.
 */
uint8_t kmc_init(const char *path)
{
    uint8_t result = 1;
    /* Default result is 1 (success) */

    s_fptr = fopen(path, "rb");
    /* Open the file in binary read mode */
    /* Set the default sector size */
    s_sectorSize = KMC_DEFAULT_SECTOR_SIZE;

    /* Check if the file was opened successfully */
    if (NULL == s_fptr)
    {
        /* If the file could not be opened, set the result to 0 (failure) */
        result = 0;
    }
    else
    {
        /* If the file was opened successfully, do nothing */
    }

    /* Return the result */
    return result;
}

/*
 *@brief Update the sector size for KMC.
 *@param sector_size - The new sector size to be set.
 *@returns Returns 1 if the sector size is updated successfully, 0 otherwise.
 */
uint8_t kmc_update_sector_size(uint16_t sector_size)
{

    uint8_t result = 1;
    /* Default result is 1 (success) */

    /* Check if the new sector size is a multiple of the default sector size */
    if (0 == (sector_size % KMC_DEFAULT_SECTOR_SIZE))
    {
        /* If it is, update the sector size */
        s_sectorSize = sector_size;
    }
    else
    {
        /* If it's not, set the result to 0 (failure) */
        result = 0;
    }

    return result;
}

/*
 *@brief Read a sector from KMC.
 *@param index - The index of the sector to be read.
 *@param buff - The buffer where the read data will be stored.
 *@returns Returns the number of bytes read.
 */
int32_t kmc_read_sector(uint32_t index, uint8_t *buff)
{
    int32_t number_of_bytes_read = 0;
    /* Variable to store the number of bytes read */

    /* Set the file position to the specified index */
    fseek(s_fptr, index, SEEK_SET);
    /* Read data from the file into the buffer */
    number_of_bytes_read = fread(buff, sizeof(uint8_t), s_sectorSize, s_fptr);

    return number_of_bytes_read;
}

/*
 *@brief Read multiple sectors from KMC.
 *@param index - The index of the first sector to be read.
 *@param num - The number of sectors to be read.
 *@param buff - The buffer where the read data will be stored.
 *@returns Returns the number of bytes read.
 */
int32_t kmc_read_multi_sector(uint32_t index, uint32_t num, uint8_t *buff)
{
    int32_t number_of_bytes_read = 0;
    /* Variable to store the number of bytes read */

    /* Set the file position to the specified index */
    fseek(s_fptr, index, SEEK_SET);
    /* Read data from the file into the buffer */
    number_of_bytes_read = fread(buff, sizeof(uint8_t), s_sectorSize * num, s_fptr);

    return number_of_bytes_read;
}

/*
 *@brief De-initialize KMC.
 *@param None.
 *@returns No return value.
 */
void kmc_de_init(void)
{
    /* Close the file */
    fclose(s_fptr);
}
