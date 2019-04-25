#ifndef _SERIAL_TYPES_H_
#define _SERIAL_TYPES_H_
#include <typedefs.h>

#define SERIAL_SCI_NUM 3

#define H8_3069F_SCI0 ((volatile struct h8_3069f_sci *)0xffffb0)
#define H8_3069F_SCI1 ((volatile struct h8_3069f_sci *)0xffffb8)
#define H8_3069F_SCI2 ((volatile struct h8_3069f_sci *)0xffffc0)


//H8_3069F is big endian
#define H8_3069F_SCI_SMR_CKS_PER1  (0<<0)
#define H8_3069F_SCI_SMR_CKS_PER4  (1<<0)
#define H8_3069F_SCI_SMR_CKS_PER16 (2<<0)
#define H8_3069F_SCI_SMR_CKS_PER64 (3<<0)

union H8_3069F_SCI_SMR {
    struct {
        uint8_t CA:1;        
        uint8_t CHR:1;
        uint8_t PE:1;
        uint8_t OE:1;
        uint8_t STOP:1;
        uint8_t MP:1;
        uint8_t CKS:2;
    } bits;
    uint8_t u8all;
};

union H8_3069F_SCI_SCR {
    struct {
        uint8_t TIE:1;  //Transmit Interrupt Enable
        uint8_t RIE:1;  //Receive Interrupt Enable
        uint8_t TE:1;   //Transmit Enable
        uint8_t RE:1;   //Receive Enable
        uint8_t MPIE:1;
        uint8_t TEIE:1;
        uint8_t CKE1:1;
        uint8_t CKE0:1;        
    } bits;
    uint8_t u8all;
};

union H8_3069F_SCI_SSR {
    struct {
        uint8_t TDRE:1; //transmit complete
        uint8_t RDRF:1; //receive complete
        uint8_t ORER:1;
        uint8_t FERERS:1;
        uint8_t PER:1;
        uint8_t TEND:1;
        uint8_t MPB:1;
        uint8_t MPBT:1;
    } bits;
    uint8_t u8all;
};

struct h8_3069f_sci {
    volatile uint8_t smr;
    volatile uint8_t brr;
    volatile uint8_t scr;
    volatile uint8_t tdr;
    volatile uint8_t ssr;
    volatile uint8_t rdr;
    volatile uint8_t scmr;
};
#endif//_SERIAL_TYPES_H_
