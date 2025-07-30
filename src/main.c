#include "Actividad_Eval2.h"
#include "stm32f103xb.h"
#define s1 1 //abre la barrera
#define s2 2 //cierra la barrera
int grados;
int main(void){
    mPap_init();
    Actividad_Eval2_init();

    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    AFIO->EXTICR[0] &= ~(0XF << (s1 * 4));
    AFIO->EXTICR[1] &= ~(0XF << (s2 * 4));
    
    EXTI->IMR |= (1 << s1);
    EXTI->FTSR |= (1 << s1);
    EXTI->RTSR &= (1 << s1);

    EXTI->IMR |= (1 << s2);
    EXTI->FTSR |= (1 << s2); //se fija en el flanco de bajada porque los sensores son pull-up
    EXTI->RTSR &= ~(1 << s2);
    
    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_SetPriority(EXTI0_IRQn, 0); //ambos sensores tienen la misma prioridad
    NVIC_SetPriority(EXTI1_IRQn, 0);

    while(1){
        if(grados != 0){
            mPap_Steps(grados);
            grados = 0;
        }
    }
    return 0;
}