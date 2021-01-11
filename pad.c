#include <stdint.h>

#define SMPC_REG_IREG(i)        *((volatile uint8_t *)0x20100001+((i) * 2))
#define SMPC_REG_COMREG         *((volatile uint8_t *)0x2010001F)
#define SMPC_REG_OREG(o)        *((volatile uint8_t *)0x20100021+((o) * 2))
#define SMPC_REG_SF             *((volatile uint8_t *)0x20100063)

#define SMPC_CMD_SSHOFF             0x03
#define SMPC_CMD_INTBACK			0x10

void stop_slave_sh2(void) {
    while (SMPC_REG_SF & 0x1)
        ;

    SMPC_REG_SF = 1;
    SMPC_REG_COMREG = SMPC_CMD_SSHOFF;

    while (SMPC_REG_SF & 0x1)
        ;
}

int pad_read(void) {
    while (SMPC_REG_SF & 0x1)
        ;

    SMPC_REG_IREG(0) = 0x00;
    SMPC_REG_IREG(1) = 0x0a;
    SMPC_REG_IREG(2) = 0xF0;

    SMPC_REG_SF = 1;
    SMPC_REG_COMREG = SMPC_CMD_INTBACK;

    while (SMPC_REG_SF & 0x1)
        ;

    int buttons = 0;
    int at = 0;
    uint16_t data = 0;
    for (int root_port=0; root_port<2; root_port++) {
        uint8_t tap_ports = SMPC_REG_OREG(at++) & 0xf;
        for (int port=0; port<tap_ports; port++) {
            uint8_t port_header = SMPC_REG_OREG(at++);
            uint8_t data_size = port_header & 0xf;
            for (int byte=0; byte<data_size; byte++) {
                data <<= 8;
                data |= SMPC_REG_OREG(at++);
            }
            if (!(port_header & 0xf0))    // it's a pad!
                buttons |= ~data;
        }
    }
    return buttons;
}
