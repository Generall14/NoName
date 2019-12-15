rm a.out
gcc -DUTEST -I../Utils -I../Inc *.c fake/*.c ../Utils/SProt/sprot_l.c ../Utils/SLog/slog.c && ./a.out
