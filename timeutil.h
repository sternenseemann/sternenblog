enum time_format {
    RSS_TIME_FORMAT,
    ATOM_TIME_FORMAT,
    HTML_TIME_FORMAT_READABLE
};

#define MAX_TIMESTR_SIZE 32
// max HTML/Atom: 24 + NUL byte
// max RSS:       31 + NUL byte

size_t flocaltime(char *b, enum time_format type, size_t size, const time_t *time);

