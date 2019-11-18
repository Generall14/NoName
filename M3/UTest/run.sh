rm a.out
gcc -I../Utils -I../Inc *.c fake/*.c ../Utils/SProt/sprot_l.c && ./a.out
