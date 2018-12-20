#include "nrf24.h"

#define nRF24_CE_L() HAL_GPIO_WritePin(n->ce_port, n->ce_pin, GPIO_PIN_RESET)
#define nRF24_CE_H() HAL_GPIO_WritePin(n->ce_port, n->ce_pin, GPIO_PIN_SET)
#define nRF24_CSN_L() HAL_GPIO_WritePin(n->csn_port, n->csn_pin, GPIO_PIN_RESET)
#define nRF24_CSN_H() HAL_GPIO_WritePin(n->csn_port, n->csn_pin, GPIO_PIN_SET)


/* Read a register
 input:
 - reg - number of register to read
 return: value of register */
static uint8_t nRF24_ReadReg(NRF24 *n, uint8_t reg)
{
    uint8_t value;
    
    nRF24_CSN_L();
    //nRF24_LL_RW(n, reg & nRF24_MASK_REG_MAP);
    //value = nRF24_LL_RW(n, nRF24_CMD_NOP);
    uint8_t tx[2] = {reg & nRF24_MASK_REG_MAP, nRF24_CMD_NOP};
    uint8_t rx[2] = {0};
    HAL_SPI_TransmitReceive(n->spi, tx, rx, 2, 10);
    value = rx[1];
    nRF24_CSN_H();
    return value;
}

/* Write a new value to register
   input:
   - reg - number of register to write
   - value - value to write */
static void nRF24_WriteReg(NRF24 *n, uint8_t reg, uint8_t value)
{
    nRF24_CSN_L();
    if (reg < nRF24_CMD_W_REGISTER) {
        // This is a register access
        uint8_t tx[2] = {nRF24_CMD_W_REGISTER
                         | (reg & nRF24_MASK_REG_MAP),
                         value };
        HAL_SPI_Transmit(n->spi, tx, 2, 10);
    } else {
        // This is a single byte command or future command/register
        uint8_t tx[2] = {reg, 0};
        if ((reg != nRF24_CMD_FLUSH_TX) && (reg != nRF24_CMD_FLUSH_RX) && \
            (reg != nRF24_CMD_REUSE_TX_PL) && (reg != nRF24_CMD_NOP)) {
            // Send register value
            tx[1] = value;
            HAL_SPI_Transmit(n->spi, tx, 2, 10);
        }
        else {
            HAL_SPI_Transmit(n->spi, tx, 1, 10);
        }
    }
    nRF24_CSN_H();
}

/* Read a multi-byte register
   input:
   - reg - number of register to read
   - pBuf - pointer to the buffer for register data
   - count - number of bytes to read */
static void nRF24_ReadMBReg(NRF24 *n,
                            uint8_t reg, uint8_t *pBuf,
                            uint8_t count)
{
    uint8_t tbuf[128] = {0};
    uint8_t rbuf[128] = {0};
    if(count > 126)
    {
        printf("ReadMBReg small buf\n");
    }
    tbuf[0] = reg;
    nRF24_CSN_L();
    HAL_SPI_TransmitReceive(n->spi, tbuf, rbuf, count + 1, 100);
    nRF24_CSN_H();
    int i = 1;
    while (count--) {
        *pBuf++ = rbuf[i];
        i++;
    }
}

/* Write a multi-byte register
 input:
 - reg - number of register to write
 - pBuf - pointer to the buffer with data to write
 - count - number of bytes to write */
static void nRF24_WriteMBReg(NRF24 *n,
                             uint8_t reg, uint8_t *pBuf,
                             uint8_t count)
{
    nRF24_CSN_L();
    uint8_t tbuf[128] = {0};
    if(count > 126)
    {
        printf("WriteMBReg small buf\n");
    }
    tbuf[0] = reg;
    int i = 0;
    int cnt = count;
    while (count--) {
        tbuf[i + 1] = pBuf[i];
        i++;
    }
    nRF24_CSN_L();
    HAL_SPI_Transmit(n->spi, tbuf, cnt + 1, 100);
    nRF24_CSN_H();
}

// Set transceiver to it's initial state
// note: RX/TX pipe addresses remains untouched
void nRF24_Init    (NRF24 *n,
                    SPI_HandleTypeDef *spi,
                    GPIO_TypeDef *ce_port,
                    uint16_t     ce_pin,
    
                    GPIO_TypeDef *csn_port,
                    uint16_t     csn_pin,
    
                    GPIO_TypeDef *irq_port,
                    uint16_t     irq_pin      )
{
    n->spi = spi;
    n->ce_port  = ce_port;
    n->ce_pin   = ce_pin;
    n->csn_port = csn_port;
    n->csn_pin  = csn_pin;
    n->irq_port = irq_port;
    n->irq_pin  = irq_pin;

    nRF24_CE_H();
    nRF24_CSN_H();
    // Write to registers their initial values
    nRF24_WriteReg(n, nRF24_REG_CONFIG, 0x08);
    nRF24_WriteReg(n, nRF24_REG_EN_AA, 0x3F);
    nRF24_WriteReg(n, nRF24_REG_EN_RXADDR, 0x03);
    nRF24_WriteReg(n, nRF24_REG_SETUP_AW, 0x03);
    nRF24_WriteReg(n, nRF24_REG_SETUP_RETR, 0x03);
    nRF24_WriteReg(n, nRF24_REG_RF_CH, 0x02);
    nRF24_WriteReg(n, nRF24_REG_RF_SETUP, 0x0E);
    nRF24_WriteReg(n, nRF24_REG_STATUS, 0x00);
    nRF24_WriteReg(n, nRF24_REG_RX_PW_P0, 0x00);
    nRF24_WriteReg(n, nRF24_REG_RX_PW_P1, 0x00);
    nRF24_WriteReg(n, nRF24_REG_RX_PW_P2, 0x00);
    nRF24_WriteReg(n, nRF24_REG_RX_PW_P3, 0x00);
    nRF24_WriteReg(n, nRF24_REG_RX_PW_P4, 0x00);
    nRF24_WriteReg(n, nRF24_REG_RX_PW_P5, 0x00);
    nRF24_WriteReg(n, nRF24_REG_DYNPD, 0x00);
    nRF24_WriteReg(n, nRF24_REG_FEATURE, 0x00);

    // Clear the FIFO's
    nRF24_FlushRX(n);
    nRF24_FlushTX(n);

    // Clear any pending interrupt flags
    nRF24_ClearIRQFlags(n);

    // Deassert CSN pin (chip release)
    nRF24_CSN_H();
}

uint8_t nRF24_Check(NRF24 *n)
{
    uint8_t rxbuf[5];
    uint8_t i;
    uint8_t *ptr = (uint8_t *)nRF24_TEST_ADDR;

    // Write test TX address and read TX_ADDR register
    nRF24_WriteMBReg(n, nRF24_CMD_W_REGISTER | nRF24_REG_TX_ADDR, ptr, 5);
    nRF24_ReadMBReg(n, nRF24_CMD_R_REGISTER | nRF24_REG_TX_ADDR, rxbuf, 5);

    // Compare buffers, return error on first mismatch
    for (i = 0; i < 5; i++) {
        if (rxbuf[i] != *ptr++) return 0;
    }

    return 1;
}

void nRF24_SetPowerMode(NRF24 *n, uint8_t mode) {
    uint8_t reg;

    reg = nRF24_ReadReg(n, nRF24_REG_CONFIG);
    if (mode == nRF24_PWR_UP) {
        // Set the PWR_UP bit of CONFIG register to wake the transceiver
        // It goes into Stanby-I mode with consumption about 26uA
        reg |= nRF24_CONFIG_PWR_UP;
    } else {
        // Clear the PWR_UP bit of CONFIG register to put the transceiver
        // into power down mode with consumption about 900nA
        reg &= ~nRF24_CONFIG_PWR_UP;
    }
    nRF24_WriteReg(n, nRF24_REG_CONFIG, reg);
}

void nRF24_SetOperationalMode(NRF24 *n, uint8_t mode)
{
    uint8_t reg;

    // Configure PRIM_RX bit of the CONFIG register
    reg  = nRF24_ReadReg(n, nRF24_REG_CONFIG);
    reg &= ~nRF24_CONFIG_PRIM_RX;
    reg |= (mode & nRF24_CONFIG_PRIM_RX);
    nRF24_WriteReg(n, nRF24_REG_CONFIG, reg);
}

void nRF24_SetCRCScheme(NRF24 *n, uint8_t scheme)
{
    uint8_t reg;

    // Configure EN_CRC[3] and CRCO[2] bits of the CONFIG register
    reg  = nRF24_ReadReg(n, nRF24_REG_CONFIG);
    reg &= ~nRF24_MASK_CRC;
    reg |= (scheme & nRF24_MASK_CRC);
    nRF24_WriteReg(n, nRF24_REG_CONFIG, reg);
}

void nRF24_SetRFChannel(NRF24 *n, uint8_t channel)
{
    nRF24_WriteReg(n, nRF24_REG_RF_CH, channel);
}

void nRF24_SetAutoRetr(NRF24 *n, uint8_t ard, uint8_t arc)
{
    // Set auto retransmit settings (SETUP_RETR register)
    nRF24_WriteReg(n, nRF24_REG_SETUP_RETR, (uint8_t)((ard << 4) | (arc & nRF24_MASK_RETR_ARC)));
}

void nRF24_SetAddrWidth(NRF24 *n, uint8_t addr_width)
{
    nRF24_WriteReg(n, nRF24_REG_SETUP_AW, addr_width - 2);
}

void nRF24_SetAddr(NRF24 *n, uint8_t pipe, const uint8_t *addr)
{
    uint8_t addr_width;

    // RX_ADDR_Px register
    switch (pipe) {
    case nRF24_PIPETX:
    case nRF24_PIPE0:
    case nRF24_PIPE1:
        // Get address width
        addr_width = nRF24_ReadReg(n, nRF24_REG_SETUP_AW) + 1;
        // Write address in reverse order (LSByte first)
        addr += addr_width;
        nRF24_CSN_L();
        uint8_t ad[128] = {0};
        uint8_t adr_width = addr_width;
        int i = 1;
        ad[0] = nRF24_CMD_W_REGISTER | nRF24_ADDR_REGS[pipe];
        do {
            ad[i] = *addr--;
            i++;
        } while (addr_width--);
        HAL_SPI_Transmit(n->spi, ad, adr_width + 1, 10);
        nRF24_CSN_H();
        break;
    case nRF24_PIPE2:
    case nRF24_PIPE3:
    case nRF24_PIPE4:
    case nRF24_PIPE5:
        // Write address LSBbyte (only first byte from the addr buffer)
        nRF24_WriteReg(n, nRF24_ADDR_REGS[pipe], *addr);
        break;
    default:
        // Incorrect pipe number -> do nothing
        break;
    }
}

void nRF24_SetTXPower(NRF24 *n, uint8_t tx_pwr)
{
    uint8_t reg;

    // Configure RF_PWR[2:1] bits of the RF_SETUP register
    reg  = nRF24_ReadReg(n, nRF24_REG_RF_SETUP);
    reg &= ~nRF24_MASK_RF_PWR;
    reg |= tx_pwr;
    nRF24_WriteReg(n, nRF24_REG_RF_SETUP, reg);
}

void nRF24_SetDataRate(NRF24 *n, uint8_t data_rate)
{
    uint8_t reg;

    // Configure RF_DR_LOW[5] and RF_DR_HIGH[3] bits of the RF_SETUP register
    reg  = nRF24_ReadReg(n, nRF24_REG_RF_SETUP);
    reg &= ~nRF24_MASK_DATARATE;
    reg |= data_rate;
    nRF24_WriteReg(n, nRF24_REG_RF_SETUP, reg);
}

void nRF24_SetRXPipe(NRF24 *n,
                     uint8_t pipe,
                     uint8_t aa_state,
                     uint8_t payload_len)
{
    uint8_t reg;

    // Enable the specified pipe (EN_RXADDR register)
    reg = (nRF24_ReadReg(n, nRF24_REG_EN_RXADDR) | (1 << pipe)) & nRF24_MASK_EN_RX;
    nRF24_WriteReg(n, nRF24_REG_EN_RXADDR, reg);

    // Set RX payload length (RX_PW_Px register)
    nRF24_WriteReg(n, nRF24_RX_PW_PIPE[pipe], payload_len & nRF24_MASK_RX_PW);

    // Set auto acknowledgment for a specified pipe (EN_AA register)
    reg = nRF24_ReadReg(n, nRF24_REG_EN_AA);
    if (aa_state == nRF24_AA_ON) {
        reg |=  (1 << pipe);
    } else {
        reg &= ~(1 << pipe);
    }
    nRF24_WriteReg(n, nRF24_REG_EN_AA, reg);
}

void nRF24_ClosePipe(NRF24 *n, uint8_t pipe)
{
    uint8_t reg;

    reg  = nRF24_ReadReg(n, nRF24_REG_EN_RXADDR);
    reg &= ~(1 << pipe);
    reg &= nRF24_MASK_EN_RX;
    nRF24_WriteReg(n, nRF24_REG_EN_RXADDR, reg);
}

void nRF24_EnableAA(NRF24 *n, uint8_t pipe)
{
    uint8_t reg;

    // Set bit in EN_AA register
    reg  = nRF24_ReadReg(n, nRF24_REG_EN_AA);
    reg |= (1 << pipe);
    nRF24_WriteReg(n, nRF24_REG_EN_AA, reg);
}

void nRF24_DisableAA(NRF24 *n, uint8_t pipe)
{
    uint8_t reg;

    if (pipe > 5) {
        // Disable Auto-ACK for ALL pipes
        nRF24_WriteReg(n, nRF24_REG_EN_AA, 0x00);
    } else {
        // Clear bit in the EN_AA register
        reg  = nRF24_ReadReg(n, nRF24_REG_EN_AA);
        reg &= ~(1 << pipe);
        nRF24_WriteReg(n, nRF24_REG_EN_AA, reg);
    }
}

uint8_t nRF24_GetStatus(NRF24 *n)
{
    return nRF24_ReadReg(n, nRF24_REG_STATUS);
}

uint8_t nRF24_GetIRQFlags(NRF24 *n)
{
    return (nRF24_ReadReg(n, nRF24_REG_STATUS)
            & nRF24_MASK_STATUS_IRQ);
}

uint8_t nRF24_GetStatus_RXFIFO(NRF24 *n)
{
    return (nRF24_ReadReg(n, nRF24_REG_FIFO_STATUS)
            & nRF24_MASK_RXFIFO);
}

uint8_t nRF24_GetStatus_TXFIFO(NRF24 *n)
{
    return ((nRF24_ReadReg(n, nRF24_REG_FIFO_STATUS)
             & nRF24_MASK_TXFIFO) >> 4);
}

uint8_t nRF24_GetRXSource(NRF24 *n)
{
    return ((nRF24_ReadReg(n, nRF24_REG_STATUS)
             & nRF24_MASK_RX_P_NO) >> 1);
}

uint8_t nRF24_GetRetransmitCounters(NRF24 *n)
{
    return (nRF24_ReadReg(n, nRF24_REG_OBSERVE_TX));
}

void nRF24_ResetPLOS(NRF24 *n)
{
    uint8_t reg;

    // The PLOS counter is reset after write to RF_CH register
    reg = nRF24_ReadReg(n, nRF24_REG_RF_CH);
    nRF24_WriteReg(n, nRF24_REG_RF_CH, reg);
}

void nRF24_FlushTX(NRF24 *n)
{
    nRF24_WriteReg(n, nRF24_CMD_FLUSH_TX, nRF24_CMD_NOP);
}

void nRF24_FlushRX(NRF24 *n)
{
    nRF24_WriteReg(n, nRF24_CMD_FLUSH_RX, nRF24_CMD_NOP);
}

void nRF24_ClearIRQFlags(NRF24 *n)
{
    uint8_t reg;

    // Clear RX_DR, TX_DS and MAX_RT bits of the STATUS register
    reg  = nRF24_ReadReg(n, nRF24_REG_STATUS);
    reg |= nRF24_MASK_STATUS_IRQ;
    nRF24_WriteReg(n, nRF24_REG_STATUS, reg);
}

void nRF24_WritePayload(NRF24 *n, uint8_t *pBuf, uint8_t length)
{
    nRF24_WriteMBReg(n, nRF24_CMD_W_TX_PAYLOAD, pBuf, length);
}

nRF24_RXResult nRF24_ReadPayload(NRF24 *n,
                                 uint8_t *pBuf,
                                 uint8_t *length)
{
    uint8_t pipe;

    // Extract a payload pipe number from the STATUS register
    pipe = (nRF24_ReadReg(n, nRF24_REG_STATUS) & nRF24_MASK_RX_P_NO) >> 1;

    // RX FIFO empty?
    if (pipe < 6) {
        // Get payload length
        *length = nRF24_ReadReg(n, nRF24_RX_PW_PIPE[pipe]);

        // Read a payload from the RX FIFO
        if (*length) {
            nRF24_ReadMBReg(n,nRF24_CMD_R_RX_PAYLOAD, pBuf, *length);
        }

        return ((nRF24_RXResult)pipe);
    }

    // The RX FIFO is empty
    *length = 0;

    return nRF24_RX_EMPTY;
}


/*

// Print nRF24L01+ current configuration (for debug purposes)
void nRF24_DumpConfig(NRF24 *n) {
uint8_t i,j;
uint8_t aw;
uint8_t buf[5];

// Dump nRF24L01+ configuration
// CONFIG
i = nRF24_ReadReg(n, nRF24_REG_CONFIG);
USART_printf(USART1,"[0x%02X] 0x%02X MASK:%03b CRC:%02b PWR:%s MODE:P%s\r\n",
nRF24_REG_CONFIG,
i,
i >> 4,
(i & 0x0c) >> 2,
(i & 0x02) ? "ON" : "OFF",
(i & 0x01) ? "RX" : "TX"
);
// EN_AA
i = nRF24_ReadReg(n, nRF24_REG_EN_AA);
USART_printf(USART1,"[0x%02X] 0x%02X ENAA: ",nRF24_REG_EN_AA,i);
for (j = 0; j < 6; j++) {
USART_printf(USART1,"[P%1u%s]%s",j,
(i & (1 << j)) ? "+" : "-",
(j == 5) ? "\r\n" : " "
);
}
// EN_RXADDR
i = nRF24_ReadReg(n, nRF24_REG_EN_RXADDR);
USART_printf(USART1,"[0x%02X] 0x%02X EN_RXADDR: ",nRF24_REG_EN_RXADDR,i);
for (j = 0; j < 6; j++) {
USART_printf(USART1,"[P%1u%s]%s",j,
(i & (1 << j)) ? "+" : "-",
(j == 5) ? "\r\n" : " "
);
}
// SETUP_AW
i = nRF24_ReadReg(n, nRF24_REG_SETUP_AW);
aw = (i & 0x03) + 2;
USART_printf(USART1,"[0x%02X] 0x%02X EN_RXADDR=%06b (address width = %u)\r\n",nRF24_REG_SETUP_AW,i,i & 0x03,aw);
// SETUP_RETR
i = nRF24_ReadReg(n, nRF24_REG_SETUP_RETR);
USART_printf(USART1,"[0x%02X] 0x%02X ARD=%04b ARC=%04b (retr.delay=%uus, count=%u)\r\n",
nRF24_REG_SETUP_RETR,
i,
i >> 4,
i & 0x0F,
((i >> 4) * 250) + 250,
i & 0x0F
);
// RF_CH
i = nRF24_ReadReg(n, nRF24_REG_RF_CH);
USART_printf(USART1,"[0x%02X] 0x%02X (%.3uGHz)\r\n",nRF24_REG_RF_CH,i,2400 + i);
// RF_SETUP
i = nRF24_ReadReg(n, nRF24_REG_RF_SETUP);
USART_printf(USART1,"[0x%02X] 0x%02X CONT_WAVE:%s PLL_LOCK:%s DataRate=",
nRF24_REG_RF_SETUP,
i,
(i & 0x80) ? "ON" : "OFF",
(i & 0x80) ? "ON" : "OFF"
);
switch ((i & 0x28) >> 3) {
case 0x00:
USART_printf(USART1,"1M");
break;
case 0x01:
USART_printf(USART1,"2M");
break;
case 0x04:
USART_printf(USART1,"250k");
break;
default:
USART_printf(USART1,"???");
break;
}
USART_printf(USART1,"pbs RF_PWR=");
switch ((i & 0x06) >> 1) {
case 0x00:
USART_printf(USART1,"-18");
break;
case 0x01:
USART_printf(USART1,"-12");
break;
case 0x02:
USART_printf(USART1,"-6");
break;
case 0x03:
USART_printf(USART1,"0");
break;
default:
USART_printf(USART1,"???");
break;
}
USART_printf(USART1,"dBm\r\n");
// STATUS
i = nRF24_ReadReg(n, nRF24_REG_STATUS);
USART_printf(USART1,"[0x%02X] 0x%02X IRQ:%03b RX_PIPE:%u TX_FULL:%s\r\n",
nRF24_REG_STATUS,
i,
(i & 0x70) >> 4,
(i & 0x0E) >> 1,
(i & 0x01) ? "YES" : "NO"
);
// OBSERVE_TX
i = nRF24_ReadReg(n, nRF24_REG_OBSERVE_TX);
USART_printf(USART1,"[0x%02X] 0x%02X PLOS_CNT=%u ARC_CNT=%u\r\n",nRF24_REG_OBSERVE_TX,i,i >> 4,i & 0x0F);
// RPD
i = nRF24_ReadReg(n, nRF24_REG_RPD);
USART_printf(USART1,"[0x%02X] 0x%02X RPD=%s\r\n",nRF24_REG_RPD,i,(i & 0x01) ? "YES" : "NO");
// RX_ADDR_P0
nRF24_ReadMBReg(n,nRF24_REG_RX_ADDR_P0,buf,aw);
USART_printf(USART1,"[0x%02X] RX_ADDR_P0 \"",nRF24_REG_RX_ADDR_P0);
for (i = 0; i < aw; i++) USART_printf(USART1,"%c",buf[i]);
USART_printf(USART1,"\"\r\n");
// RX_ADDR_P1
nRF24_ReadMBReg(n,nRF24_REG_RX_ADDR_P1,buf,aw);
USART_printf(USART1,"[0x%02X] RX_ADDR_P1 \"",nRF24_REG_RX_ADDR_P1);
for (i = 0; i < aw; i++) USART_printf(USART1,"%c",buf[i]);
USART_printf(USART1,"\"\r\n");
// RX_ADDR_P2
USART_printf(USART1,"[0x%02X] RX_ADDR_P2 \"",nRF24_REG_RX_ADDR_P2);
for (i = 0; i < aw - 1; i++) USART_printf(USART1,"%c",buf[i]);
i = nRF24_ReadReg(n, nRF24_REG_RX_ADDR_P2);
USART_printf(USART1,"%c\"\r\n",i);
// RX_ADDR_P3
USART_printf(USART1,"[0x%02X] RX_ADDR_P3 \"",nRF24_REG_RX_ADDR_P3);
for (i = 0; i < aw - 1; i++) USART_printf(USART1,"%c",buf[i]);
i = nRF24_ReadReg(n, nRF24_REG_RX_ADDR_P3);
USART_printf(USART1,"%c\"\r\n",i);
// RX_ADDR_P4
USART_printf(USART1,"[0x%02X] RX_ADDR_P4 \"",nRF24_REG_RX_ADDR_P4);
for (i = 0; i < aw - 1; i++) USART_printf(USART1,"%c",buf[i]);
i = nRF24_ReadReg(n, nRF24_REG_RX_ADDR_P4);
USART_printf(USART1,"%c\"\r\n",i);
// RX_ADDR_P5
USART_printf(USART1,"[0x%02X] RX_ADDR_P5 \"",nRF24_REG_RX_ADDR_P5);
for (i = 0; i < aw - 1; i++) USART_printf(USART1,"%c",buf[i]);
i = nRF24_ReadReg(n, nRF24_REG_RX_ADDR_P5);
USART_printf(USART1,"%c\"\r\n",i);
// TX_ADDR
nRF24_ReadMBReg(n,nRF24_REG_TX_ADDR,buf,aw);
USART_printf(USART1,"[0x%02X] TX_ADDR \"",nRF24_REG_TX_ADDR);
for (i = 0; i < aw; i++) USART_printf(USART1,"%c",buf[i]);
USART_printf(USART1,"\"\r\n");
// RX_PW_P0
i = nRF24_ReadReg(n, nRF24_REG_RX_PW_P0);
USART_printf(USART1,"[0x%02X] RX_PW_P0=%u\r\n",nRF24_REG_RX_PW_P0,i);
// RX_PW_P1
i = nRF24_ReadReg(n, nRF24_REG_RX_PW_P1);
USART_printf(USART1,"[0x%02X] RX_PW_P1=%u\r\n",nRF24_REG_RX_PW_P1,i);
// RX_PW_P2
i = nRF24_ReadReg(n, nRF24_REG_RX_PW_P2);
USART_printf(USART1,"[0x%02X] RX_PW_P2=%u\r\n",nRF24_REG_RX_PW_P2,i);
// RX_PW_P3
i = nRF24_ReadReg(n, nRF24_REG_RX_PW_P3);
USART_printf(USART1,"[0x%02X] RX_PW_P3=%u\r\n",nRF24_REG_RX_PW_P3,i);
// RX_PW_P4
i = nRF24_ReadReg(n, nRF24_REG_RX_PW_P4);
USART_printf(USART1,"[0x%02X] RX_PW_P4=%u\r\n",nRF24_REG_RX_PW_P4,i);
// RX_PW_P5
i = nRF24_ReadReg(n, nRF24_REG_RX_PW_P5);
USART_printf(USART1,"[0x%02X] RX_PW_P5=%u\r\n",nRF24_REG_RX_PW_P5,i);
}

*/
