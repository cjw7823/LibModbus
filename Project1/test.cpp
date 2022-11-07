// modbus_slave_example.c
// c. 2018 Technologic Systems
// written by Michael Peters

// This example sets up a modbus slave node that listens for modbus commands.

// The modbus_map structure is documented literally nowhere.  I dug it out of the source
//  code in modbus.h:
/*   THIS STRUCT IS HERE FOR REFERENCE.  IT IS DEFINED IN modbus.h.
typedef struct {
    int nb_bits;
    int nb_input_bits;
    int nb_input_registers;
    int nb_registers;
    uint8_t *tab_bits;
    uint8_t *tab_input_bits;
    uint16_t *tab_input_registers;
    uint16_t *tab_registers;
} modbus_mapping_t;
*/

// Compile instructions:
//  Install package pkg-config.
//  gcc `pkg-config --cflags --libs libmodbus` <filename.c> -o <binaryname> -Wall
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <modbus.h>
#include <errno.h>

#define BAUD_RATE 9600
#define MESSAGE 0X0
#define NEW_SEQUENCE 0X1
#define CURRENT_SEQUENCE 0X2
#define DA_REGISTER 0x14
#define REBOOT_ORDER 0XFFFF

#define SERIAL_PORT "COM4"
#define CONFIG_FILE "slaveid.conf"
#define TARGET_SLAVE 1
#define BROADCAST   0

modbus_t* comm_setup()
{
    modbus_t* mb;
    mb = modbus_new_rtu(SERIAL_PORT, BAUD_RATE, 'N', 8, 1);
    if (!mb) {
        fprintf(stderr, "unable to open bus at port %s.\n", SERIAL_PORT);
    }
    else
        modbus_connect(mb);
    return mb;
}

// Get the value of a specific register, populate ret_valule with it.
// returns 1 on read error.
int get_specific_register(modbus_t* mb, uint8_t target_id, uint16_t the_register, uint16_t* ret_value)
{
    int error;
    modbus_set_slave(mb, target_id);
    error = modbus_read_registers(mb, the_register, 1, ret_value);
    if (error < 0) return 1;
    else return 0;
}

// sends a number to the target device.
//  remember ID 0 is broadcast.
//  returns 1 if something went wrong, else returns 0.
int set_sequence_register(modbus_t* mb, uint8_t target_id, uint16_t sequence)
{
    modbus_set_slave(mb, target_id);
    if (modbus_write_register(mb, NEW_SEQUENCE, sequence) == -1)
        return 1;
    else
        return 0;
}

// Sets an arbitrary register on the target device.
int set_specific_register(modbus_t* mb, uint8_t target_id, uint8_t the_register, uint16_t the_message)
{
    modbus_set_slave(mb, target_id);
    if (modbus_write_register(mb, MESSAGE, the_message) == -1)
        return 1;
    else
        return 0;
}

// ******************************ENTRY POINT HERE***************************************
int main1(int argc, char** argv)
{
    int error;
    uint16_t return_value;

    modbus_t* mb = comm_setup();

    if (mb == NULL) {
        printf("%s\n", modbus_strerror(errno));
        return 1;
    }

    else {
        // Test get_register.
        error = get_specific_register(mb, TARGET_SLAVE, DA_REGISTER, &return_value);
        if (error > 0) printf("%s\n", modbus_strerror(errno));
        else printf("Requested 0xDA register, received 0x%2X.\n", return_value);

        // Test set sequence register.
        error = set_sequence_register(mb, BROADCAST, 10); // broadcast new sequence is 10.
        if (error > 0)
            printf("Unable to set sequence number.  %s\n", modbus_strerror(errno));
        else
            printf("Set new sequence number to 10.\n");

        // Read back the sequence number to make sure it got set properly.
        error = get_specific_register(mb, TARGET_SLAVE, CURRENT_SEQUENCE, &return_value);
        if (error > 0)
            printf("Unable to read back sequence number. %s\n", modbus_strerror(errno));
        else
            printf("Target's current sequence number is %d.\n", return_value);

        // Broadcast the reboot command.
        error = set_specific_register(mb, BROADCAST, MESSAGE, REBOOT_ORDER);
        if (error > 0)
            printf("Unable to send broadcast reset.  %s\n", modbus_strerror(errno));
        else
            printf("Reset message has been broadcast.\n");
    }

    modbus_close(mb);
    modbus_free(mb);
    return error;
}