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


uint8_t rev(uint8_t b)
{
	return  BIT(b,0)<<7 | BIT(b,1)<<6 | BIT (b,2)<<5 | BIT(b,3)<<4 |
		BIT(b,4)<<3 | BIT(b,5)<<2 | BIT (b,6)<<1 | BIT(b,7);
}

int main(int argc, char *argv[])
{
	if (argc!=2) {
		printf("Usage: %s <output-directory>\n", argv[0]);
		return 1;
	}
	char *output_directory=argv[1];

	//We have (up to) 8 multiplexed magazines, in order to handle this we have multiple output file handles
	FILE *of[8]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

	uint8_t packet[42];
	while (fread(packet, 1,sizeof(packet), stdin)>0) {
		//Read the packet and get the magazine and row number
		int mpag=de_hamm(packet[1])<<4 | de_hamm(packet[0]);
		int magazine=mpag&0x7;
		int row=mpag>>3;
		if (row==0) { //If row==0 => new page
			int page=de_hamm(packet[3])<<4 | de_hamm(packet[2]);
			int sub=(de_hamm(packet[4])) | (de_hamm(packet[6])<<4) | (de_hamm(packet[6])<<8) | (de_hamm(packet[7])<<12);
			int subpage=sub&0x3f7f;
			char time[9];
			memset(time, 0, sizeof(time));
			int n;
			for (n=0; n<8; n++) {
				char c=packet[34+n]&0x7f;
				if (c<'0') c='-'; else
				if (c>'9') c='-';
				time[n]=c;
			}
			if (of[magazine]!=NULL) {
				fclose(of[magazine]);
				of[magazine]=NULL;
			}

			if (page!=0xff) {
				char filename[strlen(output_directory)+32];
				memset(filename, 0, sizeof(filename));
				int hundreds=magazine;
				if (magazine==0) hundreds=8;
				snprintf(filename, sizeof(filename), "%s/%d%02x-%04x-%s.t42", output_directory, hundreds, page, subpage, time);
				printf("New file %s\n", filename);
				of[magazine]=fopen(filename,"w");
			}
		}
		//If there is an output file for this magazine, write packet into it
		if ((of[magazine]!=NULL) ) {
			fwrite(packet, sizeof(packet), 1, of[magazine]);
		}
	}
}
