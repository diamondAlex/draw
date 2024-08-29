you need sdl2 \
you need sdl2_ttf\
if you pass one argument, you go into tagging mode \
in tagging mode, you select a number, draw that number, then click S\
it will create a tagged image in the tagged_img folder\
tagged images have the number in the name (ex: test0_4.pbm would be the image 0 of a 4)\
you need sdl2_ttf\

make sure to mkdir tagged_img\

gcc draw.c -Wall -lSDL2 -lSDL2_ttf -lm && ./a.out\
