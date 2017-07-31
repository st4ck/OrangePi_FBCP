/*
Develop By Roberto Tacconelli 2017
*/

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

inline static unsigned short int make16color(unsigned char r, unsigned char g, unsigned char b)
{
    return (
               (((r >> 3) & 31) << 11) |
               (((g >> 2) & 63) << 5)  |
               ((b >> 3) & 31)        );
}

typedef struct {
    int fbfd;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize;
} FBINFO;

int tryToFillFBInfoForDevice(FBINFO *fbi, const char * fbDev) {
    fbi->fbfd = open(fbDev, O_RDWR);
    if (!fbi->fbfd) {
        perror("Error: cannot open framebuffer device");
        return 0;
    }
    printf("%s opened successfully.\n", fbDev);
    // Get fixed screen information
    if (ioctl(fbi->fbfd, FBIOGET_FSCREENINFO, &fbi->finfo)) {
        perror("Error reading fixed information");
        return 0;
    }
    // Get variable screen information
    if (ioctl(fbi->fbfd, FBIOGET_VSCREENINFO, &fbi->vinfo)) {
        perror("Error reading variable information");
        return 0;
    }
    printf("%dx%d, %dbpp\n", fbi->vinfo.xres, fbi->vinfo.yres, fbi->vinfo.bits_per_pixel);
    printf("xoffset:%d, yoffset:%d, line_length: %d\n", fbi->vinfo.xoffset, fbi->vinfo.yoffset, fbi->finfo.line_length );

    fbi->screensize = fbi->vinfo.xres * fbi->vinfo.yres * fbi->vinfo.bits_per_pixel / 8;
    printf("screensize = %ld\n", fbi->screensize);

    printf("The %s framebuffer device was inited successfully.\n", fbDev);
    return 1 ;
}

void releaseFBINFO(FBINFO * fbi) {
    if ((int)fbi->fbfd != -1)
        close(fbi->fbfd);
}



int main(int argc, char ** argv) {

    char devF1[20], devF2[20];

    if (argc != 3) {
        printf("NO INPUT!!\n\nUsage : \"PiFBCP fb_from fb_to\" \nFor Example: PiFBCP 0 1\n\n");
        return 0;
    }
    //simple format mapping
    sprintf(devF1, "/dev/fb%s", argv[1]);
    sprintf(devF2, "/dev/fb%s", argv[2]);

    FBINFO FB0, FB_TFT;
    //perror("sizeof(unsigned short) = %ld\n", sizeof(unsigned short));
    if (tryToFillFBInfoForDevice(&FB0, devF1) == 1) {
        if (tryToFillFBInfoForDevice(&FB_TFT, devF2) == 1) {

            if (FB0.vinfo.xres == FB_TFT.vinfo.xres && FB0.vinfo.xres == FB_TFT.vinfo.xres) {

                if ((FB0.vinfo.bits_per_pixel ==24 || FB0.vinfo.bits_per_pixel ==32)&&
                    FB_TFT.vinfo.bits_per_pixel==16) {
                    int i,step;
                    unsigned short c;

                    step = FB0.vinfo.bits_per_pixel/8;

		    char *img0 = (char *)malloc(FB0.screensize * sizeof(char));
		    char *imgTFT = (char *)malloc(FB_TFT.screensize * sizeof(char));

                    while (1) {
			lseek(FB0.fbfd, 0, SEEK_SET);
			read(FB0.fbfd, img0, FB0.screensize);

                        for (i = 0; i < FB0.screensize; i += step) {
                            c = make16color(img0[i], img0[i + 1], img0[i + 2]);
			    imgTFT[i / step * 2] = c & 0xff;
			    imgTFT[i / step * 2 + 1] = c >> 8;
                        }

			lseek(FB_TFT.fbfd, 0, SEEK_SET);
			write(FB_TFT.fbfd, imgTFT, FB_TFT.screensize);
                        usleep(1000 * 50); //about 20fps
                    }
                }else{
                    perror("FB Device ColorFormat unmatch!! FBCP terminal!");
                }
            } else {
                perror("FB Device Resolution unmatch!! FBCP terminal!");
            }
	    releaseFBINFO(&FB_TFT);
        } else {
            perror("FB_TO Init Error!! FBCP terminal!");
        }
	releaseFBINFO(&FB0);
    } else {
        perror("FB_FROM Init Error!! FBCP terminal!");
    }
    return 0;
}
