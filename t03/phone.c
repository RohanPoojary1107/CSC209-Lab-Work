#include <stdio.h>
#include <stdlib.h>

int main(){
	char str[11];
	int index;
	while(scanf("%10s %d", str, &index)!= EOF){
		if (index == 0){
			printf("%s\n", str);
		}else if(index >= 1 && index <= 9){
			printf("%c\n", str[index]);
		}else{
			printf("%c\n", str[9]);
		}
	}
	return 0;
}
