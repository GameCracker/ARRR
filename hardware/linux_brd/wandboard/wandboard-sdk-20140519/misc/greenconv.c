
/*
This is a dirty-hack to reduce artifacts in HDMI bootlogos
It shifts the green-plane 0.75 pixels to the right at edges

License: This file is in the public domain.
*/

#include <stdio.h>
#include <stdlib.h>

typedef struct {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
} RGBA;

int main(int argc, char **argv) {
        FILE *f;
        int i, n;
        RGBA cur, next;        
        RGBA buf[345600];
        
        if (argc < 2) {
                printf("rgba filename argument missing\n");
                return -1;
        }
        
        f = fopen(argv[1], "rb");
        while (f != NULL && !feof(f)) {
                fread(&buf[i++], 1, sizeof(RGBA), f);                
        }
        n = i-1;
        fclose(f);

        for (i=0; i<n-1; i++) {
                cur = buf[i];
                next = buf[i+1];
                if (cur.g > next.g) 
                        cur.g = (3*(int)next.g + (int)cur.g) >> 2;
                buf[i] = cur;
        }
        
        f = fopen("out.rgba", "wb");                
        for (i=0; i<n; i++) {
                fwrite(&buf[i], 1, sizeof(RGBA), f);
        }
        fclose(f);
        
        return 0;
}
