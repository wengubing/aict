/**************************************************************************//**
 * @file     cmsis_version.h
 * @brief    CMSIS Core(M) Version definitions
 * @version  V5.0.5
 * @date     02. February 2022
 ******************************************************************************/
#ifndef __CMSIS_VERSION_H
#define __CMSIS_VERSION_H

#define __CM_CMSIS_VERSION_MAIN  ( 5U)  /*!< [31:16] CMSIS Core(M) main version   */
#define __CM_CMSIS_VERSION_SUB   ( 6U)  /*!< [15:0]  CMSIS Core(M) sub version    */
#define __CM_CMSIS_VERSION       ((__CM_CMSIS_VERSION_MAIN << 16U) | \
                                   __CM_CMSIS_VERSION_SUB           )

#define __CORTEX_M                (4U)   /*!< Cortex-M Core                        */

#endif /* __CMSIS_VERSION_H */
