#include <serial.h>
#include <serial_types.h>

static struct {
    volatile struct h8_3069f_sci *sci;
} regs[SERIAL_SCI_NUM] = {
    {H8_3069F_SCI0},
    {H8_3069F_SCI1},
    {H8_3069F_SCI2},
};

int serial_init(int index){
    volatile struct h8_3069f_sci *sci = regs[index].sci;
    sci->scr = 0;
    sci->smr = 0;    
    sci->brr = 64; //generate 9600bps from 20Mhz. 9600 = 64[bitrate] * 300 / 20[MHz]
    volatile union H8_3069F_SCI_SCR scr;
    scr.u8all = sci->scr;
    scr.bits.RE = 1;
    scr.bits.TE = 1;
    sci->scr = scr.u8all;
    sci->ssr = 0;
    return 0;
}

int serial_is_send_enable(int index)
{
    volatile struct h8_3069f_sci *sci = regs[index].sci;
    volatile union H8_3069F_SCI_SSR ssr;
    ssr.u8all = sci->ssr;
    return ssr.bits.TDRE;
}

int serial_send_byte(int index, uint8_t data)
{
    volatile struct h8_3069f_sci *sci = regs[index].sci;
    //wait for send enable
    while (!serial_is_send_enable(index));
    sci->tdr = data;
    volatile union H8_3069F_SCI_SSR ssr;
    ssr.u8all = sci->ssr;
    ssr.bits.TDRE = 0; //transmit start
    sci->ssr = ssr.u8all;

    return 0;
}

int serial_is_recv_enable(int index){
    volatile struct h8_3069f_sci *sci = regs[index].sci;
    volatile union H8_3069F_SCI_SSR ssr;
    ssr.u8all = sci->ssr;
    return ssr.bits.RDRF;
}

uint8_t serial_recv_byte(int index){
    volatile struct h8_3069f_sci *sci = regs[index].sci;
    uint8_t c;
    //wait by the receive character
    while(!serial_is_recv_enable(index));
    //data receive
    c = sci->rdr;
    //status to receive complete
    volatile union H8_3069F_SCI_SSR ssr;
    ssr.u8all = sci->ssr;
    ssr.bits.RDRF = 0;
    sci->ssr = ssr.u8all;
    return c;
}

int serial_intr_is_send_enable(int index){
    volatile struct h8_3069f_sci *sci = regs[index].sci;
    volatile union H8_3069F_SCI_SCR scr;
    scr.u8all = sci->scr;
    return scr.bits.TIE;
}

void serial_intr_send_enable(int index){
    volatile struct h8_3069f_sci *sci = regs[index].sci;
    volatile union H8_3069F_SCI_SCR scr;
    scr.u8all = sci->scr;
    scr.bits.TIE = 1;
    sci->scr = scr.u8all;
}

void serial_intr_send_disable(int index){
    volatile struct h8_3069f_sci *sci = regs[index].sci;
    volatile union H8_3069F_SCI_SCR scr;
    scr.u8all = sci->scr;
    scr.bits.TIE = 0;
    sci->scr = scr.u8all;
}

int serial_intr_is_recv_enable(int index){
    volatile struct h8_3069f_sci *sci = regs[index].sci;
    volatile union H8_3069F_SCI_SCR scr;
    scr.u8all = sci->scr;
    return scr.bits.RIE;
}

void serial_intr_recv_enable(int index){
    volatile struct h8_3069f_sci *sci = regs[index].sci;
    volatile union H8_3069F_SCI_SCR scr;
    scr.u8all = sci->scr;
    scr.bits.RIE = 1;
    sci->scr = scr.u8all;
}

void serial_intr_recv_disable(int index){
    volatile struct h8_3069f_sci *sci = regs[index].sci;
    volatile union H8_3069F_SCI_SCR scr;
    scr.u8all = sci->scr;
    scr.bits.RIE = 0;
    sci->scr = scr.u8all;
}



