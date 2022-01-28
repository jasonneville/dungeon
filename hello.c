#include <stdio.h> 


int main(int argc, char *argv[]) {


	int i = 0;



	for (i = 0; i < 100; i++) {

		printf("%2d", i);
		if (i % 9 == 0 && i != 0) {
			printf("\n");
		}
	}




	return 0;

}