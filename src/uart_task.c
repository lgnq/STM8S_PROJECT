/*----------------------------------------------------------------------------*-

  uart_task.c (Release 2017-02-22a)

  ----------------------------------------------------------------------------

  Also offers unbuffered ('flush whole buffer') option: use this with care!

  Allows use of USB port on board as UART2 interface.

  See 'ERES2' (Chapter 3) for further information.

-*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*-

  This code is copyright (c) 2014-2017 SafeTTy Systems Ltd.

  This code forms part of a Time-Triggered Reference Design (TTRD) that is
  documented in the following book:

   Pont, M.J. (2016)
   "The Engineering of Reliable Embedded Systems (Second Edition)",
   Published by SafeTTy Systems Ltd. ISBN: 978-0-9930355-3-1.

  Both the TTRDs and the above book ("ERES2") describe patented
  technology and are subject to copyright and other restrictions.

  This code may be used without charge: [i] by universities and colleges on
  courses for which a degree up to and including 'MSc' level (or equivalent)
  is awarded; [ii] for non-commercial projects carried out by individuals
  and hobbyists.

  Any and all other use of this code and / or the patented technology
  described in ERES2 requires purchase of a ReliabiliTTy Technology Licence:
  http://www.safetty.net/technology/reliabilitty-technology-licences

  Please contact SafeTTy Systems Ltd if you require clarification of these
  licensing arrangements: http://www.safetty.net/contact

  CorrelaTTor, DecomposiTTor, DuplicaTTor, MoniTTor, PredicTTor, ReliabiliTTy,
  SafeTTy, SafeTTy Systems, TriplicaTTor and WarranTTor are registered
  trademarks or trademarks of SafeTTy Systems Ltd in the UK & other countries.

-*----------------------------------------------------------------------------*/

#include "main.h"

// ------ Private constants --------------------------------------------------

// Value returned by PC_LINK_Get_Char if no character is
// available in buffer
#define PC_LINK_NO_CHAR 127
#define XON 0x11
#define XOFF 0x13

// The transmit buffer length
#define TX_BUFFER_SIZE 200

// The receive buffer length
#define RX_BUFFER_SIZE 100

// ------ Private variables --------------------------------------------------

static char tx_buffer[TX_BUFFER_SIZE];
static char rx_buffer[RX_BUFFER_SIZE];

static uint32_t tx_buffer_out_idx;  // Data in buffer that has been written
static uint32_t tx_buffer_in_idx;  // Data in buffer not yet written

static uint32_t rx_buffer_out_idx;      // Data in buffer that has been received
static uint32_t rx_buffer_in_idx;   // Data in buffer not yet received

// ------ Private function prototypes ----------------------------------------

void UART2_BUF_O_Send_Char(const char);

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Init()

  Set up UART1.

  PARAMETER:
     Required baud rate.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     UART1.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void UART2_BUF_O_Init(uint32_t BAUD_RATE)
{
    tx_buffer_out_idx = 0;
    tx_buffer_in_idx  = 0;
    rx_buffer_out_idx = 0;
    rx_buffer_in_idx  = 0;

    UART1_DeInit();

    UART1_Init(BAUD_RATE, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);

    UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
    UART1_Cmd(ENABLE);
}

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Update()

  Sends next character from the software transmit buffer

  NOTE: Output-only library (Cannot receive chars)

  Uses on-chip UART hardware.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     UART1.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  WCET:
     Not yet determined.

  BCET:
     Not yet determined.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void UART2_BUF_O_Update(void)
{
    // Deal with transmit bytes here

    // Is there any data ready to send?
    if (tx_buffer_out_idx < tx_buffer_in_idx)
    {
        UART2_BUF_O_Send_Char(tx_buffer[tx_buffer_out_idx]);

        tx_buffer_out_idx++;
    }
    else
    {
        // No data to send - just reset the buffer index
        tx_buffer_in_idx = 0;
        tx_buffer_out_idx = 0;
    }

#if 0
    if (UART1_GetFlagStatus(UART1_FLAG_RXNE) == 1)
    {
        // Flag only set when a valid stop bit is received,
        // -> data ready to be read into the received buffer
        // Want to read into index 0, if old data have been read
        // (simple ~circular buffer)
        if (rx_buffer_in_idx == rx_buffer_out_idx)
        {
            rx_buffer_in_idx = 0;
            rx_buffer_out_idx = 0;
        }

        // Read the data from USART buffer
        rx_buffer[rx_buffer_in_idx] = UART1_ReceiveData8();
        UART2_BUF_O_Write_Char_To_Buffer(rx_buffer[rx_buffer_in_idx]);

        if (rx_buffer_in_idx < RX_BUFFER_SIZE)
        {
            rx_buffer_in_idx++;
        }
    }
#endif
}

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Send_All_Data()

  Sends all data from the software transmit buffer.

  NOTES:
  * May have very long execution time!
  * Intended for use when the scheduler is NOT running.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     UART1.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void UART2_BUF_O_Send_All_Data(void)
{
    while (tx_buffer_out_idx < tx_buffer_in_idx)
    {
        UART2_BUF_O_Send_Char(tx_buffer[tx_buffer_out_idx]);

        tx_buffer_out_idx++;
    }

    // Reset the buffer indices
    tx_buffer_in_idx = 0;
    tx_buffer_out_idx = 0;
}

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Write_String_To_Buffer()

  Copies a (null terminated) string to the module Tx buffer.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void UART2_BUF_O_Write_String_To_Buffer(const char* const STR_PTR)
{
    uint32_t i = 0;

    while (STR_PTR[i] != '\0')
    {
        UART2_BUF_O_Write_Char_To_Buffer(STR_PTR[i]);
        i++;
    }
}

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Write_Char_To_Buffer()

  Stores a single character in the Tx buffer.

  PARAMETERS:
     CHARACTER is character to be stored.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void UART2_BUF_O_Write_Char_To_Buffer(const char CHARACTER)
{
    // Write to the buffer *only* if there is space
    if (tx_buffer_in_idx < TX_BUFFER_SIZE)
    {
        tx_buffer[tx_buffer_in_idx] = CHARACTER;
        tx_buffer_in_idx++;
    }
    else
    {
        // Write buffer is full
        // No error handling / reporting here (characters may be lost)
        // Adapt as required to meet the needs of your application
    }
}

uint8_t uart_read_char_from_buffer(void)
{
    uint8_t ch = PC_LINK_NO_CHAR;

    UART1_ITConfig(UART1_IT_RXNE_OR, DISABLE);

    // If there is new data in the buffer
    if (rx_buffer_out_idx < rx_buffer_in_idx)
    {
        ch = rx_buffer[rx_buffer_out_idx];
        if (rx_buffer_out_idx < RX_BUFFER_SIZE)
        {
            rx_buffer_out_idx++;
        }
    }

    UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);

    return ch;
}

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Write_Number10_To_Buffer()

  Writes 10-digit (decimal, unsigned) number to Tx buffer as a string.

  Supported values: 0 - 9,999,999,999.

  (Can be used with 32-bit unsigned integer values.)

  PARAMETERS:
     DATA : The number to be stored.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void UART2_BUF_O_Write_Number10_To_Buffer(const uint32_t DATA)
{
    char Digit[11];

    int_disable();
    Digit[10] = '\0';  // Null terminator
    Digit[9]  = 48 + (DATA % 10);
    Digit[8]  = 48 + ((DATA/10) % 10);
    Digit[7]  = 48 + ((DATA/100) % 10);
    Digit[6]  = 48 + ((DATA/1000) % 10);
    Digit[5]  = 48 + ((DATA/10000) % 10);
    Digit[4]  = 48 + ((DATA/100000) % 10);
    Digit[3]  = 48 + ((DATA/1000000) % 10);
    Digit[2]  = 48 + ((DATA/10000000) % 10);
    Digit[1]  = 48 + ((DATA/100000000) % 10);
    Digit[0]  = 48 + ((DATA/1000000000) % 10);
    int_enable();

    UART2_BUF_O_Write_String_To_Buffer(Digit);
}

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Write_Number03_To_Buffer()

  Writes 3-digit (decimal, unsigned) number to Tx buffer as a string.

  Supported values: 0 - 999.

  PARAMETERS:
     DATA : The number to be stored.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     A simple range check is performed.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     The function simply returns if the data are out of range.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void UART2_BUF_O_Write_Number03_To_Buffer(const uint32_t DATA)
{
    char Digit[4];

    if (DATA <= 999)
    {
        int_disable();
        Digit[3] = '\0';  // Null terminator
        Digit[2] = 48 + (DATA % 10);
        Digit[1] = 48 + ((DATA/10) % 10);
        Digit[0] = 48 + ((DATA/100) % 10);
        int_enable();

        UART2_BUF_O_Write_String_To_Buffer(Digit);
    }
}

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Write_Number02_To_Buffer()

  Writes 2-digit (decimal, unsigned) number to Tx buffer as a string.

  Supported values: 0 - 99.

  PARAMETERS:
     DATA : The number to be stored.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     A simple range check is performed.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     The function simply returns if the data are out of range.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void UART2_BUF_O_Write_Number02_To_Buffer(const uint32_t DATA)
{
    char Digit[3];

    if (DATA <= 99)
    {
        int_disable();
        Digit[2] = '\0';  // Null terminator
        Digit[1] = 48 + (DATA % 10);
        Digit[0] = 48 + ((DATA/10) % 10);
        int_enable();
    }

    UART2_BUF_O_Write_String_To_Buffer(Digit);
}

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Send_Char()

  Uses on-chip UART0 hardware to send a single character.

  PARAMETERS:
     CHARACTER : The data to be sent.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     UART1.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void UART2_BUF_O_Send_Char(const char CHARACTER)
{
    UART1_SendData8((unsigned char)CHARACTER);

    /* Loop until the end of transmission */
    while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET)
      ;
}

uint8_t cmd = CMD_NONE;

uint8_t idx = 0;

uint8_t buff[50];

uint8_t state = WAIT_FOR_SOF1;

#if 0
void get_data_by_idx(uint8_t *msg, uint8_t idx)
{
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t n = 0;
    uint8_t m = 0;
    uint8_t tmp[14];

    while (1)
    {
        switch (msg[i])
        {
        case ',':
        case '#':
            i++;

            if (n == idx)
            {
                for (m = 0; m < j; m++)
                    UART2_BUF_O_Write_Char_To_Buffer(tmp[m]);

                return;
            }
            else
            {
                n++;
                j = 0;
                for (m = 0; m < 14; m++)
                    tmp[m] = 0;
            }
            break;
        default:
            tmp[j++] = msg[i++];
            break;
        }
    }
}
#endif // 0

int a2i(const char *str)  
{  
    int s = 0;  
    bool falg = 0;  
      
    while(*str==' ')  
    {  
        str++;  
    }  
  
    if (*str=='-' || *str=='+')  
    {  
        if (*str=='-')  
            falg = 1;  
        str++;  
    }  
  
    while (*str>='0' && *str<='9')  
    {  
        s = s*10 + *str - '0';  
        str++;
  
        if (s < 0)  
        {  
            s = 2147483647;  
            break;  
        }  
    }  

    return s*(falg?-1:1);  
}  

void parse_btfrt_data(uint8_t *msg)
{
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t n = 0;
    //uint8_t m = 0;
    uint8_t tmp[6];

    uint16_t pgn;
    uint16_t src;
    uint16_t dst;
    uint8_t pri;
    uint8_t message_identifier;
    uint16_t battery_voltage;

    while (1)
    {
        if (msg[i] == '#')
        {
            tmp [j] = '\0';

            return;
        }
        else if (msg[i] == ',')
        {
            tmp[j] = '\0';

            //for (m = 0; m < j; m++)
            //{
            //    UART2_BUF_O_Write_Char_To_Buffer(tmp[m]);
            //}

            switch (n++)
            {
            case 0: //PGN
                pgn = a2i(tmp);
                break;
            case 1: //Source Address
                src = a2i(tmp);
                UART2_BUF_O_Write_Number03_To_Buffer(src);
                break;
            case 2: //Destination Address
                dst = a2i(tmp);
                break;
            case 3: //Priority
                pri = a2i(tmp);
                break;
            case 4: //Message Identifier
                message_identifier = a2i(tmp);
                break;
            case 5: //Battery Voltage
                battery_voltage = a2i(tmp);
                break;
            case 6: //Machine Utilization Hour Meter
                break;
            case 7: //DC Controller Enable Hour Meter Trigger
                break;
            case 8: //IC Engine Hour Meter Trigger
                break;
            case 9: //Boom/Platform Stowed
                break;
            case 10: //Over Load
                break;
            case 11: //Key Switch State
                break;
            case 12: //Footswitch
                break;
            case 13: //Machine Off Level
                break;
            default:
                break;
            }

            j = 0;
            i++;
        }
        else
        {
            tmp[j++] = msg[i++];
        }
    }
}

void process_msg(uint8_t cmd, uint8_t *msg, uint8_t size)
{
    uint8_t i;

    for (i = 0; i < size; i++)
        UART2_BUF_O_Write_Char_To_Buffer(msg[i]);

    switch (cmd)
    {
    case CMD_BTCAR:
        UART2_BUF_O_Write_String_To_Buffer("CMD_BTCAR\r\n");
        break;
    case CMD_BTFRT:
        parse_btfrt_data(msg);
        break;
    case CMD_BTSEC:
        UART2_BUF_O_Write_String_To_Buffer("CMD_BTSEC\r\n");
        break;
    case CMD_NONE:
    default:
        UART2_BUF_O_Write_String_To_Buffer("CMD_NONE\r\n");
        break;
    }
}

/*
  AT+BTFRT=61184,1,6,6,11,4500,1,1,1,1,1,1,1#
  AT+BTSEC=61184,1,6,6,12,4500,1,1,1,1#
  AT+BTCAR=61184,1,6,6,20100320064500,1,1,1,1#

  AT+BTACK=1,1,1,4500#
 */
void protocol_update(void)
{
    uint8_t c;

    c = uart_read_char_from_buffer();

    if (c != PC_LINK_NO_CHAR)
    {
        switch (state)
        {
        case WAIT_FOR_SOF1:
            if (c == 'A')
                state = WAIT_FOR_SOF2;
            break;
        case WAIT_FOR_SOF2:
            if (c == 'T')
                state = WAIT_FOR_SOF3;
            else
                state = WAIT_FOR_SOF1;
            break;
        case WAIT_FOR_SOF3:
            if (c == '+')
                state = WAIT_FOR_SOF4;
            else
                state = WAIT_FOR_SOF1;
            break;
        case WAIT_FOR_SOF4:
            if (c == 'B')
                state = WAIT_FOR_SOF5;
            else
                state = WAIT_FOR_SOF1;
            break;
        case WAIT_FOR_SOF5:
            if (c == 'T')
                state = WAIT_FOR_SOF6;
            else
                state = WAIT_FOR_SOF1;
            break;
        case WAIT_FOR_SOF6:
            if (c == 'C')
            {
                cmd = CMD_BTCAR;
                state = WAIT_FOR_SOF7;
            }
            else if (c == 'F')
            {
                cmd = CMD_BTFRT;
                state = WAIT_FOR_SOF7;
            }
            else if (c == 'S')
            {
                cmd = CMD_BTSEC;
                state = WAIT_FOR_SOF7;
            }
            else
            {
                cmd = CMD_NONE;
                state = WAIT_FOR_SOF1;
            }
            break;
        case WAIT_FOR_SOF7:
            if (cmd == CMD_BTCAR)
            {
                if (c == 'A')
                    state = WAIT_FOR_SOF8;
                else
                {
                    cmd = CMD_NONE;
                    state = WAIT_FOR_SOF1;
                }
            }
            else if(cmd == CMD_BTFRT)
            {
                if (c == 'R')
                    state = WAIT_FOR_SOF8;
                else
                {
                    cmd = CMD_NONE;
                    state = WAIT_FOR_SOF1;
                }
            }
            else if(cmd == CMD_BTSEC)
            {
                if (c == 'E')
                    state = WAIT_FOR_SOF8;
                else
                {
                    cmd = CMD_NONE;
                    state = WAIT_FOR_SOF1;
                }
            }
            break;
        case WAIT_FOR_SOF8:
            if (cmd == CMD_BTCAR)
            {
                if (c == 'R')
                    state = WAIT_FOR_SOF9;
                else
                {
                    cmd = CMD_NONE;
                    state = WAIT_FOR_SOF1;
                }
            }
            else if(cmd == CMD_BTFRT)
            {
                if (c == 'T')
                    state = WAIT_FOR_SOF9;
                else
                {
                    cmd = CMD_NONE;
                    state = WAIT_FOR_SOF1;
                }
            }
            else if(cmd == CMD_BTSEC)
            {
                if (c == 'C')
                    state = WAIT_FOR_SOF9;
                else
                {
                    cmd = CMD_NONE;
                    state = WAIT_FOR_SOF1;
                }
            }
            break;
        case WAIT_FOR_SOF9:
            if (c == '=')
            {
                idx = 0;
                state = WAIT_FOR_DATA;
            }
            else
                state = WAIT_FOR_SOF1;
            break;
        case WAIT_FOR_DATA:
            buff[idx++] = c;

            if (c == '#')
            {
                state = WAIT_FOR_SOF1;

                process_msg(cmd, buff, idx);
            }

            //if (timeout)
            //    state = WAIT_FOR_SOF1;
            break;
        default:
            break;
        }
    }
}

INTERRUPT_HANDLER(UART1_RX_IRQHandler, 18)
{
    if (rx_buffer_in_idx == rx_buffer_out_idx)
    {
        rx_buffer_in_idx = 0;
        rx_buffer_out_idx = 0;
    }

    // Read the data from USART buffer
    rx_buffer[rx_buffer_in_idx] = UART1_ReceiveData8();

    if (rx_buffer_in_idx < RX_BUFFER_SIZE)
    {
        rx_buffer_in_idx++;
    }
}

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/

