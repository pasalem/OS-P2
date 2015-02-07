#include <stdio.h>
#include <stdlib.h>


int main(){
	unsigned long count = 0;
	while(1){
		count++;
		if( count > 2000000000 ){
			return 0;
		}
	}
}