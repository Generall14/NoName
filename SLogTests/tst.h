#define SLOGNAME TST

#define ICAT(A, B) #A#B
#define CAT(A, B) ICAT(A, B)
// #define SLABEL CAT(SLOGFNAME, STR(__LINE__))
#define SLABEL ICAT(SLOGNAME, SLOGNAME)

#define ISTR(x) #x
#define STR(x) ISTR(x)
// #define AT __FILE__ ":" TOSTRING(__LINE__)

#define LOG_ERROR(TXT) SLABEL
#define AT __FILE__

#define LOG_TEST(CNT, ...) slog_log(CNT, __VA_ARGS__)
