


#define SLOGNAME SHOULD_BE_DEFINED_IN_EACH_FILE

#define MAIN_FILE_20 777
#define MAIN_FILE_21 0x13

#define CAT(A, C) A##_##C
#define GEN_LABEL(name, line) CAT(name, line)
#define LOG_ERROR(TXT, ...) slog_log(GEN_LABEL(SLOGNAME, __LINE__), __VA_ARGS__)
