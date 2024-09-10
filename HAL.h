/**
 * @file: HAL.h
 * @brief Header File for KMC System
 * @details This header file contains the function prototypes and type definitions used in the KMC system program.
 *               It includes function prototypes for initializing the KMC system with a specific file path, updating the sector size for KMC,
 *               reading a sector from KMC, reading multiple sectors from KMC, and de-initializing the KMC system.
 *               The file also defines the default sector size for the KMC system.
 *
 * @author: Nguyen Dang Nhu Tri
 * @version: 1.0
 * @date: 2024/05/12
 *
 * @copyright: Copyright (c) 2024
 */

#ifndef HAL_H
#define HAL_H

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdio.h>
#include <stdint.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define KMC_DEFAULT_SECTOR_SIZE 512 /* Default size of sector */

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*
 * @brief Initialize KMC with a specific file path.
 * @details This function attempts to open a file in binary read mode using the given file path.
 *               It sets a default sector size and checks if the file was opened successfully.
 *               If the file cannot be opened, it sets the result to 0 indicating failure.
 *               Otherwise, it retains the default result value of 1, indicating success, and returns it.
 * @param path - A pointer to a character array that contains the path to the file to be opened.
 * @returns Returns 1 if the file opens successfully, indicating success. Returns 0 if the file cannot be opened, indicating failure.
 */
uint8_t kmc_init(const char *path);

/*
 * @brief Update the sector size for KMC.
 * @details This function is designed to update the sector size used by the KMC system.
 *               It checks whether the new sector size provided is a multiple of the default sector size.
 *               If it is, the sector size is updated to the new value.
 *               If not, the function fails to update the sector size and indicates failure by returning 0.
 * @param sector_size - The new sector size to be set. It must be a multiple of the KMC_DEFAULT_SECTOR_SIZE.
 * @returns Returns 1 if the sector size is updated successfully, indicating success. Returns 0 if the update fails, indicating failure.
 */
uint8_t kmc_update_sector_size(uint16_t sector_size);

/*
 * @brief Read a sector from KMC.
 * @details This function is responsible for reading data from a specific sector of the KMC system.
 *               It takes an index representing the sector number and a buffer to store the read data.
 *               The function sets the file pointer to the start of the specified sector and reads data up to the size of the sector into the buffer.
 *               The number of bytes successfully read is then returned.
 * @param index - The index of the sector to be read. It determines the starting point for the read operation within the file.
 * @param buff - A pointer to a buffer where the read data will be stored. It must be large enough to hold the data from one sector.
 * @returns Returns the number of bytes read from the sector. This value can be used to determine if the read operation was successful and how much data was retrieved.
 */
int32_t kmc_read_sector(uint32_t index, uint8_t *buff);

/*
 * @brief Read multiple sectors from KMC.
 * @details This function reads a specified number of sectors starting from a given index in the KMC system.
 *               It sets the file pointer to the beginning of the first sector to be read and then reads data sequentially
 *               from the file into the provided buffer. The total number of bytes read across all sectors is calculated
 *               and returned, which reflects the amount of data successfully retrieved from the file.
 * @param index - The index of the first sector to be read. This is the starting point for the read operation.
 * @param num - The number of sectors to be read. This determines how many sectors' worth of data will be read into the buffer.
 * @param buff - A pointer to a buffer where the read data will be stored. The buffer should be large enough to hold the data from the specified number of sectors.
 * @returns Returns the number of bytes read, which indicates the total amount of data retrieved from the specified sectors.
 */
int32_t kmc_read_multi_sector(uint32_t index, uint32_t num, uint8_t *buff);

/*
 * @brief De-initialize KMC.
 * @details This function is responsible for de-initializing the KMC system by closing the file associated with it.
 *               It ensures that any open file is properly closed to prevent data loss or corruption.
 *               This is typically called as a cleanup procedure when the KMC operations are no longer needed.
 * @param None.
 * @returns No return value. This function performs a cleanup task and does not return any value.
 */
void kmc_de_init(void);
#endif /*HAL_H*/
