#include <stdio.h>
#include <string.h>

void bitStuffing(const char* data, char* stuffedData) {
    int j = 0;
    int count = 0;
    for (int i = 0; data[i] != '\0'; i++) {
        stuffedData[j++] = data[i];
        if (data[i] == '1') {
            count++;
            if (count == 5) {
                stuffedData[j++] = '0';
                count = 0;
            }
        } else {
            count = 0;
        }
    }
    stuffedData[j] = '\0';
}

int main() {
    char data[100];
    char stuffedData[100];
    printf("Enter the Bit String");
    scanf("%s",data);
    bitStuffing(data, stuffedData);
    printf("Original Data: %s\n", data);
    printf("Stuffed Data: %s\n", stuffedData);
   
    return 0;
}
