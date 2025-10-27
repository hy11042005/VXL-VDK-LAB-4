#include "main.h"
#include "scheduler.h"

TIM_HandleTypeDef htim2;

// LED task prototypes
void Task1(void);
void Task2(void);
void Task3(void);
void Task4(void);
void Task5(void);

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_TIM2_Init();
  SCH_Init();

  // Start timer interrupt
  HAL_TIM_Base_Start_IT(&htim2);

  // Add tasks with different periods (tick = 10ms)
  SCH_Add_Task(Task1, 0, 50);   // 0.5s
  SCH_Add_Task(Task2, 0, 100);  // 1.0s
  SCH_Add_Task(Task3, 0, 150);  // 1.5s
  SCH_Add_Task(Task4, 0, 200);  // 2.0s
  SCH_Add_Task(Task5, 0, 250);  // 2.5s

  while (1)
  {
    SCH_Dispatch_Tasks();
   // __WFI();
  }
}

// === Timer interrupt callback ===
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  //if (htim->Instance == TIM2)
  //{
    SCH_Update();
  //}
}

// === LED Tasks ===
void Task1(void) { HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0); }
void Task2(void) { HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1); }
void Task3(void) { HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2); }
void Task4(void) { HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3); }
void Task5(void) { HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4); }

// === Timer config 10ms ===
static void MX_TIM2_Init(void)
{
  __HAL_RCC_TIM2_CLK_ENABLE();

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;     // if APB1 timer clock = 72MHz -> 72MHz/7200 = 10kHz
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;         // 10kHz / 100 = 100Hz -> 10ms tick
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }

  // Enable NVIC for TIM2
  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);

  // Start timer in interrupt mode
  if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
}


// === GPIO config ===
static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4, GPIO_PIN_SET);
}
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Use HSI (internal 8 MHz) and PLL to 72 MHz
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2; // HSI/2 = 4MHz
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;              // 4MHz *9 = 36MHz -> WAIT
    // Note: On some HAL for F1, PLLSOURCE HSI_DIV2 *9 -> 36MHz. If your device expects 72MHz use HSE.
    // Better to set to 8MHz *9 using HSE; but Proteus often runs OK with HSI-based clocks.
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    // Configure clocks: use PLL as SYSCLK if PLL started
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                                  RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    // APB1 max is 36MHz on some devices. set DIV2 if PLL output=72MHz
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

// -------------------- HÀM XỬ LÝ LỖI --------------------
void Error_Handler(void)
{
    __disable_irq();
    while (1) {
        // Có thể nháy LED báo lỗi ở đây
    }
}
