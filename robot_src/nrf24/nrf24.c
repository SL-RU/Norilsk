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

static uint8_t nRF24_ToggleFeatures(NRF24 *n)
{
    uint8_t value;
    
    nRF24_CSN_L();
    //nRF24_LL_RW(n, reg & nRF24_MASK_REG_MAP);
    //value = nRF24_LL_RW(n, nRF24_CMD_NOP);
    uint8_t tx[2] = {nRF24_ACTIVATE, 0x73};
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

    n->payload_size = 10;
    n->addr_width   = 5;
    
    uint8_t t = 0;
    HAL_SPI_Transmit(spi, &t, 1, 10);
    nRF24_CSN_H();
    nRF24_CE_L();
    HAL_Delay(5);
    // Write to registers their initial values
    nRF24_WriteReg(n, nRF24_REG_CONFIG, 0x0C);
    nRF24_SetAutoRetr(n, nRF24_ARD_1500us, 15);
    nRF24_SetDataRate(n, nRF24_DR_1Mbps);
    nRF24_SetCRCScheme(n, nRF24_CRC_2byte);
    //nRF24_SetAddrWidth(n, 5);
    
    nRF24_ToggleFeatures(n);
    nRF24_WriteReg(n, nRF24_REG_FEATURE, 0x00);
    nRF24_WriteReg(n, nRF24_REG_DYNPD, 0x00);

    nRF24_WriteReg(n, nRF24_REG_STATUS,
                   (nRF24_FLAG_TX_DS | nRF24_FLAG_MAX_RT | nRF24_FLAG_RX_DR));
    
    nRF24_SetRFChannel(n, 76);
    
    // Clear any pending interrupt flags
    //nRF24_ClearIRQFlags(n);

    // Clear the FIFO's
    nRF24_FlushRX(n);
    nRF24_FlushTX(n);

    nRF24_SetPowerMode(n, nRF24_PWR_UP);

    nRF24_WriteReg(n, nRF24_REG_CONFIG,
                   nRF24_ReadReg(n, nRF24_REG_CONFIG) & ~nRF24_CONFIG_PRIM_RX);
    
    // Deassert CSN pin (chip release)
    nRF24_CSN_H();
    nRF24_CE_L();
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
    HAL_Delay(5);
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
    if( addr_width < 3 || addr_width > 5)
        return;
    nRF24_WriteReg(n, nRF24_REG_SETUP_AW, addr_width - 2);
    n->addr_width = addr_width;
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
    uint8_t pp[32] = {0};
    for(int i = 0; i < 32; i++)
    {
        if(length) {
            pp[i] = pBuf[i];
            length --;
        }
        else
            pp[i] = 0;
    }
    nRF24_WriteMBReg(n, nRF24_CMD_W_TX_PAYLOAD, pp, 32);
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

void nRF24_CE_ON(NRF24 *n)
{
    nRF24_CE_H();
}
void nRF24_CE_OFF(NRF24 *n)
{
    nRF24_CE_L();
}

void nRF24_print(NRF24 *n)
{
    printf("config 0x%x\n", nRF24_ReadReg(n, nRF24_REG_CONFIG));
    printf("config 0x%x\n", nRF24_ReadReg(n, nRF24_REG_CONFIG));
}

void nRF24_openWritingPipe(NRF24 *n, uint8_t *addr, uint8_t length)
{
    if(length < n->addr_width)
        return;
    
    nRF24_WriteMBReg(n, nRF24_CMD_W_REGISTER | nRF24_REG_RX_ADDR_P0, addr, n->addr_width);
    nRF24_WriteMBReg(n, nRF24_CMD_W_REGISTER | nRF24_REG_TX_ADDR, addr, n->addr_width);

    nRF24_WriteReg(n, nRF24_REG_RX_PW_P0, n->payload_size);
}
void nRF24_stopListening(NRF24 *n)
{
    nRF24_CE_L();
    HAL_Delay(1);

    if(nRF24_ReadReg(n, nRF24_REG_FEATURE) & 0x01)
    {
        HAL_Delay(1);
        nRF24_FlushRX(n);
    }
    nRF24_WriteReg(n, nRF24_REG_CONFIG,
                   nRF24_ReadReg(n, nRF24_REG_CONFIG) & ~nRF24_CONFIG_PRIM_RX);

    nRF24_WriteReg(n, nRF24_REG_EN_RXADDR,
                   nRF24_ReadReg(n, nRF24_REG_EN_RXADDR) | 1);
}
void nRF24_write(NRF24 *n, void* buf, uint8_t len)
{
    nRF24_WritePayload(n, buf, len);
    nRF24_CE_H();

    uint32_t tries = 20;

    while(! (nRF24_GetStatus(n) & ( nRF24_FLAG_TX_DS | nRF24_FLAG_MAX_RT) ))
    {
        if(tries == 0) {
            printf("write too much tries\n");
            return;
        }
        HAL_Delay(1);
        tries--;
    }
    nRF24_CE_L();
    
    uint8_t status = (nRF24_GetStatus(n)
                      & (nRF24_FLAG_TX_DS | nRF24_FLAG_MAX_RT | nRF24_FLAG_RX_DR));

    if(status & nRF24_FLAG_MAX_RT)
    {
        nRF24_FlushTX(n);
        printf("write MAX_RT\n");
        return;
    }
}
