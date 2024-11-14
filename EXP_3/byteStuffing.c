#include <stdio.h>
#include <string.h>

void byteStuffing(const char* data, char* stuffedData) {
    char ESC = 0x1B;
    char FLAG = 0x7E; 
    int j = 0;
   
    for (int i = 0; data[i] != '\0'; i++) {
        if (data[i] == FLAG || data[i] == ESC) {
            stuffedData[j++] = ESC;
            stuffedData[j++] = data[i] ^ 0x20; 
        } else {
            stuffedData[j++] = data[i];
        }
    }
    stuffedData[j] = '\0';
}

int main() {
    char data[] = "This is a test frame with a special character: ~";
    char stuffedData[100];
   
    byteStuffing(data, stuffedData);
    printf("Original Data: %s\n", data);
    printf("Stuffed Data: %s\n", stuffedData);
   
    return 0;
}





