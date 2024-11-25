#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

float float_decoder(uint16_t value);

int main(void)
{
    char *filename = "tg";
    FILE *fp = fopen(filename, "r");
    char buffer[28];

    while((fgets(buffer, 28, fp))!=NULL){

        char buf_value[5];
        buf_value[0] = '0';
        buf_value[1] = '0';
        buf_value[2] = '0';
        buf_value[3] = buffer[0];
        buf_value[4] = '\0';
        uint16_t value = 0;

        sscanf(buf_value, "%x", &value);
        value = value & 0x2;

        if (value == 2){
            printf("Resending is required");
        }
        else{
            printf("Resending is not required");
        }

        buf_value[3] = buffer[1];
        sscanf(buf_value, "%x", &value);
        value = value & 0xC;
        if (value == 0xC){
            printf(", low operational priority ");
        }
        else if(value == 0x8){
            printf(", alarm ");
        }
        else if(value == 0x0){
            printf(", highest priority ");
        }
        else if(value == 0x4){
            printf(", normal ");
        }

        buf_value[0] = buffer[2];
        buf_value[1] = buffer[3];
        buf_value[2] = buffer[4];
        buf_value[3] = buffer[5];
        sscanf(buf_value, "%x", &value);
        uint16_t area = (value >> 12) & 0xF;
        uint16_t line = (value & 0xF00) >> 8;
        uint16_t bus = value & 0x0FF;
        printf("\nAddress: %d.%d.%d  ", area, line, bus);

        buf_value[0] = buffer[6];
        buf_value[1] = buffer[7];
        buf_value[2] = buffer[8];
        buf_value[3] = buffer[9];
        sscanf(buf_value, "%x", &value);
        uint16_t target_addr_m = (value >> 11) & 0x1F;
        uint16_t target_addr_mi = (value & 0x700) >> 8;
        uint16_t target_addr_s = value & 0x0FF;
        printf("Target address: %d.%d.%d  ", target_addr_m, target_addr_mi, target_addr_s);

        buf_value[0] = '0';
        buf_value[1] = '0';
        buf_value[2] = buffer[10];
        buf_value[3] = buffer[11];
        sscanf(buf_value, "%x", &value);
        uint16_t target_individual = (value >> 7) & 0x01;

        if (target_individual){
            printf("All bus devices are addressed\n");
        }
        else{
            printf("Only one bus device is addressed\n");
        }

        uint16_t routing_couter = (value >> 4) & 0b111;
        uint16_t lenght = value & 0b1111;
        printf("Routing: %d, ", routing_couter);
        printf("Lenght: %d\n", lenght);

        for(uint16_t i = 16; i<16+4*(lenght-2); i+=4){
            buf_value[0] = buffer[i];
            buf_value[1] = buffer[i+1];
            buf_value[2] = buffer[i+2];
            buf_value[3] = buffer[i+3];
            sscanf(buf_value, "%x", &value);
            float result = float_decoder(value);
            char result_str[10];
            sprintf(result_str, "%.2f", result);
            printf("Payload: %s\n", result_str);
        }
        printf("\n");
;   }
    return 0;
}

float float_decoder(uint16_t value){

    uint16_t sign = (value >> 15) & 0x1;
    uint16_t exp = (value >> 11) & 0xF;
    uint16_t mantissa = value & 0x7FF;

    if (sign){
        mantissa = mantissa | 0xF000;
    }

    float result = (mantissa * 0.01) * pow(2, exp);
    return result;
}



