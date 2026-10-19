// gcc zeeyaa.c -o zeeyaa -lm
// ./zeeyaa

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#define SLEEP_MS(x) Sleep(x)
#else
#include <sys/ioctl.h>
#include <unistd.h>
#define SLEEP_MS(x) usleep((x) * 1000)
#endif

#define MAXW 140
#define MAXH 60
#define NP 22
#ifndef T0
#define T0 0.0
#endif

typedef struct { int r, g, b; } Rgb;
typedef struct { char ch; Rgb fg, bg; } Cell;

static const char *GL[4][5] = {
    {"#####", "   # ", "  #  ", " #   ", "#####"}, 
    {"#####", "#    ", "#### ", "#    ", "#####"}, 
    {"#   #", " # # ", "  #  ", "  #  ", "  #  "}, 
    {" ### ", "#   #", "#####", "#   #", "#   #"}, 
};
static const int SEQ[6] = {0, 1, 1, 2, 3, 3};      

static Cell g[MAXH][MAXW];
static char out[MAXW * MAXH * 48 + 2048];

static int cl(double v) { return v < 0 ? 0 : (v > 255 ? 255 : (int)v); }

static void bye(int s) {
    (void)s;
    fputs("\033[0m\033[?25h\033[2J\033[H", stdout);
    fflush(stdout);
    exit(0);
}

static void term_size(int *cols, int *rows) {
    int c = 0, r = 0;
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO i;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &i)) {
        c = i.srWindow.Right - i.srWindow.Left + 1;
        r = i.srWindow.Bottom - i.srWindow.Top + 1;
    }
#else
    struct winsize ws;
    if (ioctl(1, TIOCGWINSZ, &ws) == 0 && ws.ws_col) { c = ws.ws_col; r = ws.ws_row; }
#endif
    if (!c && getenv("COLUMNS")) c = atoi(getenv("COLUMNS"));
    if (!r && getenv("LINES"))   r = atoi(getenv("LINES"));
    *cols = c ? c : 80;
    *rows = r ? r : 24;
}

int main(void) {
#ifdef _WIN32
    HANDLE ho = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(ho, &mode)) SetConsoleMode(ho, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
    signal(SIGINT, bye);
    signal(SIGTERM, bye);
    srand((unsigned)time(NULL));

    int w = 0, h = 0, frame = 0;
    double px[NP], py[NP], pv[NP];
    for (int i = 0; i < NP; i++) pv[i] = 0.12 + (rand() % 100) / 400.0;
    printf("\033[2J\033[?25l");

    for (double t = T0;; t += 0.055, frame++) {
        if (frame % 20 == 0) {                         
            int cols, rows; term_size(&cols, &rows);
            int nw = cols - 1, nh = rows - 2;
            if (nw > MAXW) nw = MAXW;
            if (nh > MAXH) nh = MAXH;
            if (nw != w || nh != h) {
                w = nw; h = nh;
                for (int i = 0; i < NP; i++) { px[i] = rand() % (w > 0 ? w : 1); py[i] = rand() % (h > 0 ? h : 1); }
                printf("\033[0m\033[2J");
            }
        }
        if (w < 45 || h < 20) {
            printf("\033[H\033[0mTerminal kekecilan - perbesar jendela (min 46x22)   ");
            fflush(stdout); SLEEP_MS(200); continue;
        }

        double ry = fmin(h / 2.0, w / 4.0) / 1.4;      
        double beat  = 1.0 + 0.09 * pow(fmax(0, sin(t * 4.0)), 3)
                           + 0.045 * pow(fmax(0, sin(t * 4.0 + 1.0)), 3);
        double grow  = 1.0 - exp(-t * 2.5);
        double sweep = fmod(t * 0.9, 5.0) - 2.5;

        for (int r = 0; r < h; r++) {
            for (int c = 0; c < w; c++) {
                double X = (c - w / 2.0) / (2.0 * ry);
                double Y = (h / 2.0 - r) / ry;
                double glow = exp(-(X * X + Y * Y) * 0.9) * grow * (0.7 + (beat - 1.0) * 3.0);
                Rgb bg = { cl(10 + r * 90.0 / h * 0.4 + 90 * glow), cl(2 + 10 * glow), cl(22 + r * 32.0 / h + 45 * glow) };
                Cell *k = &g[r][c];
                k->ch = ' '; k->fg = (Rgb){255, 255, 255};

                int hs = (c * 73 + r * 151) % 131;               
                if (hs == 0 && glow < 0.35) {
                    double tw = 0.5 + 0.5 * sin(t * 2.5 + c * r);
                    int v = cl(90 + 165 * tw);
                    k->ch = tw > 0.85 ? '+' : '.';
                    k->fg = (Rgb){v, v, cl(v + 20)};
                }

                double s = beat * grow;
                if (s > 0.02) {
                    double x = X / s, y = Y / s;
                    double a = x * x + y * y - 1;
                    double f = a * a * a - x * x * y * y * y;
                    if (f <= 0) {
                        double hx = x + 0.45, hy = y - 0.55;     
                        double L = exp(-(hx * hx + hy * hy) * 1.3);
                        double d = fmin(1.0, -f * 5.0);
                        double m = 0.62 + 0.38 * d;
                        double sh = exp(-pow((x + y * 0.5 - sweep) * 2.2, 2)) * 90;
                        bg.r = cl((170 + 85 * L) * m + sh);
                        bg.g = cl((10 + 120 * L * L) * m + sh);
                        bg.b = cl((50 + 110 * L) * m + sh);
                        if (f > -0.03) bg = (Rgb){255, 160, 200}; 
                        k->ch = ' ';
                    }
                }
                k->bg = bg;
            }
        }

        int tr0 = h / 2 - 4, tc0 = w / 2 - 17;
        for (int i = 0; i < 6; i++) {
            if (t < 1.0 + i * 0.25) continue;
            for (int rr = 0; rr < 5; rr++)
                for (int cc = 0; cc < 5; cc++)
                    if (GL[SEQ[i]][rr][cc] == '#') {
                        Cell *k = &g[tr0 + rr][tc0 + i * 6 + cc];
                        k->ch = ' ';
                        k->bg = (Rgb){255, cl(228 + 27 * sin(t * 3 + i)), 242};
                    }
        }

        for (int i = 0; i < NP; i++) {
            py[i] -= pv[i];
            px[i] += 0.15 * sin(t * 2 + i);
            if (py[i] < 0) { py[i] = h - 1; px[i] = rand() % w; }
            int ix = (int)px[i], iy = (int)py[i];
            if (grow > 0.9 && ix >= 0 && ix < w && iy >= 0 && iy < h && g[iy][ix].ch == ' ') {
                g[iy][ix].ch = (i % 2) ? '*' : 'o';
                g[iy][ix].fg = (Rgb){255, cl(150 + 60 * sin(t + i)), 190};
            }
        }

        char *p = out;
        p += sprintf(p, "\033[H");
        for (int r = 0; r < h; r++) {
            Rgb lf = {-1, -1, -1}, lb = {-1, -1, -1};
            for (int c = 0; c < w; c++) {
                Cell *k = &g[r][c];
                if (k->bg.r != lb.r || k->bg.g != lb.g || k->bg.b != lb.b) {
                    p += sprintf(p, "\033[48;2;%d;%d;%dm", k->bg.r, k->bg.g, k->bg.b);
                    lb = k->bg;
                }
                if (k->ch != ' ' && (k->fg.r != lf.r || k->fg.g != lf.g || k->fg.b != lf.b)) {
                    p += sprintf(p, "\033[38;2;%d;%d;%dm", k->fg.r, k->fg.g, k->fg.b);
                    lf = k->fg;
                }
                *p++ = k->ch;
            }
            p += sprintf(p, "\033[0m\n");
        }
        p += sprintf(p, "\033[38;2;255;130;190m%*s* * *  Z e e y a a  * * *\033[0m\033[K", (w - 25) / 2 > 0 ? (w - 25) / 2 : 0, "");
        fwrite(out, 1, (size_t)(p - out), stdout);
        fflush(stdout);
        SLEEP_MS(45);
    }
    return 0;
}