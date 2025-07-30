#include "Actividad_Eval2.h"
#include "delay.h"
#include "stm32f103xb.h"
#define s1 1 //abre la barrera
#define s2 2 //cierra la barrera
int sec[4][4] = { 
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1},
    //es una secuencia de paso completo
};
int pins[4];
void Actividad_Eval1_init(){
    RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;
    for(int i = 0; i < 3; i++){
        //seteamos los pines del PA en 0 para la inicializacion
        GPIOA -> CRL &= ~(0xF << ((s1 * 4) + i)); 
        GPIOA -> CRL &= ~(0xF << ((s2 * 4) + i));
    }
    GPIOA -> CRL |= (0b1000 << (s1 * 4)); //los sensores son entradas con pull-up por consigna 
    GPIOA -> CRL |= (0b1000 << (s2 * 4));
}
void mPaP_Init(int IN0, int IN1, int IN2, int IN3){
    int pins[4] = {IN0, IN1, IN2, IN3};
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    for(int i = 0; i < 4; i++){
        if(pins[i] < 8){
            for(int j = 0; j < 4; j++){
                GPIOA->CRL &= ~(0XF << ((pins[i]*4) + j));
            }
            GPIOA->CRL |= (1 << (pins[i] * 4));
        }else{
            for(int j = 0; j < 4; j++){
                GPIOA->CRH &= ~(0XF << ((pins[i]*4) + j));
            }
            GPIOA->CRH |= (1 << (pins[i] * 4));
        }
    }
    
}
int mPap_Steps(int grados){
    int sentido = 0; //tambien se puede hacer con variable de tipo bool
    float pasos = grados/1.8;
    
    for(int i = 0; i < pasos; i++){ //porque debo prender y apagar las bobinas (cantidad de pasos) veces
        if(grados > 0){ //si el angulo de inclinacion de la barrera es mayor a 0, la barrera debe subir
            sentido = 1;
        }else{ //si no, debe bajar
            sentido = -1;
            pasos = -pasos; 
        }
        if(grados == -60){
            delay_ms(10000); //se esperan 10 segundos antes de cerrar la barrera
        }
        for(int i = 0; i < 4; i++){
            int orden;
            if (sentido == 1){
                for(int j = 0; j < 4; j++){
                    if(sec[i%4][j]){
                        GPIOA->BSRR |= (1 << pins[j]);
                    }else{
                        GPIOA->BSRR |= (1 << (pins[j] + 16));
                    }
                }
                //las bobinas se prenden de la 0 a la 3
            }else if(sentido == -1){
                for(int j = 0; j < 4; j++){
                    if(sec[3-(i%4)][j]){
                        GPIOA->BSRR |= (1 << pins[j]);
                    }else{
                        GPIOA->BSRR |= (1 << (pins[j] + 16));
                    }
                }
                //las bobinas del motor se prender en orden inverso (de la 3 a la 0)
            }
        }
    }
    return pasos;
}
void EXTI0_IRQHandler(void){
    if(EXTI->PR & (1 << s1)){
        mPap_Steps(60);
        EXTI->PR |= (1 << s1);
    }
}
void EXTI1_IRQHandler(void){
    if(EXTI->PR & (1 << s2)){
        mPap_Steps(-60);
        EXTI->PR |= (1 << s2);
    }
}