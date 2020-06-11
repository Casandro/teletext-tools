#include <stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<strings.h>


#define BIT(x, y) ((x>>y)&0x1)
int de_hamm(uint8_t x)
{
	return BIT(x,1) | (BIT(x,3)<<1) | (BIT(x,5)<<2) | (BIT(x,7)<<3);
}


int compare_pages(const char *fn1, const char *fn2)
{
	FILE *f1=fopen(fn1, "r");
	if (f1==NULL) return -1;
	FILE *f2=fopen(fn2, "r");
	if (f2==NULL) return -1;

	uint8_t packet1[42];
	uint8_t packet2[42];

	while( (fread(packet1, 1, sizeof(packet1), f1)>0) &&
		(fread(packet2, 1, sizeof(packet2), f2)>0) ) {

		int mpag1=de_hamm(packet2[1])<<4 | de_hamm(packet2[0]);
		int magazine1=mpag1&0x7;
		int row1=mpag1>>3;
		int mpag2=de_hamm(packet2[1])<<4 | de_hamm(packet2[0]);
		int magazine2=mpag2&0x7;
		int row2=mpag2>>3;


		if ( (row1==0) && (row2==0) ) continue; //Ignore header row as time can change
		int n;
		for (n=0; n<sizeof(packet1); n++) {
			if (packet1[n]!=packet2[n]) {
				fclose(f1);
				fclose(f2);
				return 1; //We found a difference
			}
		}
	}
	fclose(f1);
	fclose(f2);
	return 0; // We found no difference

}

int main(int argc, char *argv[])
{
	if (argc<2) {
		printf("This program deletes duplicate copies of a page keeping the first one.\nUsage: %s <file1> <file2> ... <filen>\n", argv[0]);
		return 1;
	}
	if (argc<4) return 1;
	int n;
	for (n=1; n<argc-1; n++) {
		int m;
		for (m=n+1; m<argc-1; m++)  {
//			printf("Comparing %p with %p... ", argv[n], argv[m]);
//			printf("Comparing %s with %s... ", argv[n], argv[m]);
			int cres=compare_pages(argv[n], argv[m]);
			if (cres==0) {
				printf("deleting %s\n",argv[m]);
				unlink(argv[m]);
			}
//			if (cres<0) printf("error\n");
//			if (cres>0) printf("different\n");
		}
	}
}
