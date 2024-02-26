/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */
#include "tim.h"
/* USER CODE END 0 */

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 16;
  hcan.Init.Mode = CAN_MODE_SILENT_LOOPBACK;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_3TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_5TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = ENABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */
	CAN_FilterTypeDef sFilterConfig;
	sFilterConfig.FilterBank = 0; // 使用过滤器0
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000; // 接收所有ID
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0; // 分配到FIFO 0
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14; // 如果是双CAN，则需要配置

	if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK) {
			// 错误处理
			Error_Handler();
	}

	if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
    // 错误处理
    Error_Handler();
	}
	//	CAN_Filter_Configuration();
	if (HAL_CAN_Start(&hcan) != HAL_OK) {
    // 错误处理
	}

  /* USER CODE END CAN_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

#include "stdlib.h"
#include "string.h"

#define CAN_QUEUE_SIZE 10 // 根据需要调整队列大小
CAN_MessageTypeDef canQueue[CAN_QUEUE_SIZE];
int queueStart = 0; // 队列开始位置
int queueEnd = 0;   // 队列结束位置
bool queueFull = false;

bool EnqueueCANMessage(const CAN_TxHeaderTypeDef *TxHeader, const uint8_t *TxData) {
    if (queueFull) {
        // 队列已满，无法添加新消息
        return false;
    }

    canQueue[queueEnd].TxHeader = *TxHeader;
    memcpy(canQueue[queueEnd].TxData, TxData, 8);

    queueEnd = (queueEnd + 1) % CAN_QUEUE_SIZE;

    if (queueEnd == queueStart) {
        queueFull = true; // 更新队列满的状态
    }

    return true;
}


void ProcessCANQueue(void) {
    uint32_t TxMailbox;
    while (queueStart != queueEnd || queueFull) { // 修改条件以处理队列满的情况
        if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan) > 0) {
            CAN_MessageTypeDef *msg = &canQueue[queueStart];
            if (HAL_CAN_AddTxMessage(&hcan, &msg->TxHeader, msg->TxData, &TxMailbox) == HAL_OK) {
                queueStart = (queueStart + 1) % CAN_QUEUE_SIZE;
                if (queueFull) {
                    queueFull = false; // 成功发送，队列现在肯定不满
                }
            } else {
                // 发送失败，可能需要处理
                break;
            }
        } else {
            // 没有空闲邮箱，稍后再试
            break;
        }
    }
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
       // A(); // 调用函数A
    }
		
		
}

uint8_t TxData[8];
void A(void) {
    CAN_TxHeaderTypeDef TxHeader;
//    uint8_t TxData[8];
    uint32_t TxMailbox;

    TxHeader.DLC = 8; // 数据长度为8
    TxHeader.IDE = CAN_ID_STD; // 使用标准标识符
    TxHeader.RTR = CAN_RTR_DATA; // 数据帧
    TxHeader.StdId = 0x123; // 设置一个标准标识符，示例为0x123

    // 将字符串"12345678"复制到数据数组
//    char *message = "12345678";
//    for (int i = 0; i < 8; i++) {
//        TxData[i] = message[i];
//    }

    if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK) {
        // 错误处理
        Error_Handler();
    }
}

uint8_t RxData[8];

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    CAN_RxHeaderTypeDef RxHeader;
//    uint8_t RxData[8];

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK) {
        // 错误处理
        Error_Handler();
    }

    // 处理接收到的消息
    // 例如，检查RxData中的数据或RxHeader中的ID
}

void HandleCANRxMessage(CAN_HandleTypeDef *hcan, uint32_t RxFifo) {
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[8];

    // 检查是否有消息待读取
    while (HAL_CAN_GetRxFifoFillLevel(hcan, RxFifo) > 0) {
        // 获取消息
        if (HAL_CAN_GetRxMessage(hcan, RxFifo, &RxHeader, RxData) == HAL_OK) {
            // 在这里进行消息的路由解析
            // 例如，根据RxHeader.StdId或RxHeader.ExtId路由到不同的处理函数
            if (RxHeader.IDE == CAN_ID_STD) {
                // 处理标准ID消息
            } else if (RxHeader.IDE == CAN_ID_EXT) {
                // 处理扩展ID消息
            }

            // 更新接收消息计数器或执行其他状态统计
        }
    }
}

// 对应于FIFO 0的中断服务例程
void CAN_RX0_IRQHandler(void) {
    HandleCANRxMessage(&hcan, CAN_RX_FIFO0);
}

// 对应于FIFO 1的中断服务例程
void CAN_RX1_IRQHandler(void) {
    HandleCANRxMessage(&hcan, CAN_RX_FIFO1);
}

int error1,error2,error3;
void CAN_SCE_IRQHandler(void) {
    // 检查具体的错误或警告状态
    if (__HAL_CAN_GET_FLAG(&hcan, CAN_FLAG_EWG)) {
        // 错误警告状态
			error1++;
    }
    if (__HAL_CAN_GET_FLAG(&hcan, CAN_FLAG_EPV)) {
        // 错误被动状态
			error2++;
    }
    if (__HAL_CAN_GET_FLAG(&hcan, CAN_FLAG_BOF)) {
        // 总线关闭状态
			error3++;
    }
    // 清除中断标志
    __HAL_CAN_CLEAR_FLAG(&hcan, CAN_FLAG_EWG);
    __HAL_CAN_CLEAR_FLAG(&hcan, CAN_FLAG_EPV);
    __HAL_CAN_CLEAR_FLAG(&hcan, CAN_FLAG_BOF);

    // 更新状态统计，如错误计数
}

void CAN_Filter_Configuration(void) {
    CAN_FilterTypeDef sFilterConfig;

    // 过滤器0配置，用于FIFO0
    sFilterConfig.FilterBank = 0;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = 0x100 << 5; // 对于标准ID，需要左移5位
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0xFFFF << 5; // 接收特定标准ID
    sFilterConfig.FilterMaskIdLow = 0x0000; // 不关心扩展ID位
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.FilterActivation = ENABLE;

    if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK) {
        Error_Handler();
    }

    // 过滤器1配置，用于FIFO1
    sFilterConfig.FilterBank = 1; // 使用下一个过滤器组
    sFilterConfig.FilterIdHigh = (0x1FFFFFFF >> 13) & 0xFFFF; // 对于扩展ID，高16位
    sFilterConfig.FilterIdLow = (0x1FFFFFFF << 3) & 0xFFFF; // 对于扩展ID，低16位需要左移3位
    sFilterConfig.FilterMaskIdHigh = 0xFFFF; // 接收特定扩展ID
    sFilterConfig.FilterMaskIdLow = 0xFFF8; // 屏蔽位设置，只关心特定扩展ID
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO1;

    if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK) {
        Error_Handler();
    }
}


/* USER CODE END 1 */
