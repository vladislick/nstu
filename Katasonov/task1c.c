#include <stdio.h>
#include <stdlib.h>

int coder(int arr[], int sms);
int decoder(int arr[], int sms);

int main(int argv, char* argc[]) {
	int n = 4, d = 1, k, D, sms, arr[256], i = 0, c;
	if (argv == 1) {
		printf("Coding(0) or decoding(1)?\n");
		scanf("%d", &c);
        //res = atoi(argc[1]);
    } else {
		c = atoi(argc[1]);
		}

    printf("Enter the number: ");
	if (scanf("%d", &sms) != 1) {
		printf("n/a");
		//exit;
	}
	
	switch(c) {
		case 0:		
			coder(arr, sms);
			break;
		case 1:
			decoder(arr, sms);
			break;
	}
	
	
	//printf ("\nM = 10\nn = %d\nd = 1\n", n);
}

int coder(int arr[], int sms) {
	int count;
	int sms1 = sms;
	while (sms1 !=0) {
		sms1 = sms1 / 10;
		count++;
	}
	
	for (int i = count-1; i >= 0; i--) {
		int temp = sms % 10;
		arr[i] = temp;
		sms = sms/10;
	}
	
	for (int i = 0; i < count; i++) {
	    int temp = 0;
	    while (arr[i] != 0) {
			if ((arr[i] - 5) >= 0) { 
			temp = temp + 1000;
			arr[i] = arr[i] - 5;
			}
			if ((arr[i] - 3) >= 0) { 
			temp = temp + 100;
			arr[i] = arr[i] - 3;
			} 
			if ((arr[i] - 2) >= 0) { 
			temp = temp + 10;
			arr[i] = arr[i] - 2;
			} 
			if ((arr[i] - 1) >= 0) { 
			temp = temp + 1;
			arr[i] = arr[i] - 1;
			}
	    }
		arr[i] = temp;
	}
	for (int i = 0; i < count; i++){
if (arr[i] < 10) {
printf("000%d ", arr[i]);
} else if (arr[i] < 100) {
printf("00%d ", arr[i]);
} else if (arr[i] < 1000) {
printf("0%d ", arr[i]);
} else {
printf("%d ", arr[i]);
}
}
}

int decoder(int arr[], int sms) {
	int temp = 0, count = 0;
	int sms1 = sms;
	while (sms1 !=0) {
		sms1 = sms1 / 10;
		count++;
	}
	
	for (int i = 0; i < count; i++) {
		int temp = sms % 10;
		arr[i] = temp;
		//с++;
		sms = sms/10;
	}
	for (int i = 0; i < count; i++) {
		if (arr[i] == 1) {
			if (i == 0) {
			temp = temp + 1; 
			}
			if (i == 1) {
			temp = temp + 2; 
			}
			if (i  == 2) {
			temp = temp + 3; 
			}
			if (i == 3) {
			temp = temp + 5; 
			}
		}
	}
	arr[0] = temp;
	printf("\n%d ", arr[0]);
}
//int decoder(int arr, count) {}

	