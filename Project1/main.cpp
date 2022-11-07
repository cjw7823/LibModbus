#include <stdio.h>
#include <modbus.h>


int main(void) {
    modbus_t* ctx;
    //modbus_mapping_t* mb_mapping;
    int rc = 0;
    ctx = modbus_new_rtu("COM4", 9600, 'N', 8, 1);

    if (modbus_set_slave(ctx, 1) == -1) {
        fprintf(stderr, "modbus_set_slave failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }
    printf("\n setted slave!!!!\n");
    modbus_set_socket(ctx, 1);

    struct timeval response;

    response.tv_sec = 0;

    response.tv_usec = 1000;

    if (modbus_set_response_timeout(ctx, response.tv_sec, response.tv_usec) == -1) {
        fprintf(stderr, "modbus_set_response_timeout failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    modbus_set_debug(ctx, TRUE);

    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    while (1)
    {
        printf("\n START \n");
        0xffff;
        uint16_t test = 0;
        int count = modbus_read_input_registers(ctx, 0x000A, 1, &test);
        printf(" %d ", test);

        Sleep(100);
        system("cls");
    }

    uint8_t raw_req[] = { 1, MODBUS_FC_READ_INPUT_REGISTERS, 0x08, 0x01};
    uint8_t raw_receive[MODBUS_RTU_MAX_ADU_LENGTH] = { 0, };


    //while(1)
    //{
    //    //req_length = modbus_send_raw_request(ctx, raw_req, 6 * sizeof(uint8_t));
    //    //req_length = modbus_receive(ctx, raw_receive);
    //    modbus_send_raw_request(ctx, raw_req, 4 * sizeof(uint8_t));
    //    int req_length = 0;
    //    req_length = modbus_receive(ctx, raw_receive);
    //    printf(" %d ", req_length);
    //   if (req_length != -1)
    //        break;
    //}
    /*while (1)
    {
        uint16_t test;
        int a = modbus_read_input_registers(ctx, 0, 1, &test);
        printf(" %d ", a);
    }*/
   

    printf("\n END \n");
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}