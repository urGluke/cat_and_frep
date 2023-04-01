#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct options {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
  int E;
  int T;
} opt;

void parser(int argc, char **argv, opt *options);
void reader(char **argv, opt *options);
int format_line(int line_count, FILE *f, opt *options, int *nf);

int main(int argc, char **argv) {
  if (argc > 1) {
    opt options = {0};
    parser(argc, argv, &options);
  }
}

void parser(int argc, char **argv, opt *options) {
  int opt, err = 0;
  int option_index = 0;
#ifdef __linux__
  static struct option long_options[] = {{"number", 0, NULL, 'n'},
                                         {"squeeze", 0, NULL, 's'},
                                         {"number-nonblank", 0, NULL, 'b'},
                                         {NULL, 0, NULL, 0}};
  char *optns = "+benstvTE";
#else
  static struct option long_options[] = {NULL, 0, NULL, 0};
  char *optns = "+benstv";
#endif
  while ((opt = getopt_long(argc, argv, optns, long_options, &option_index)) !=
         -1) {
    switch (opt) {
      case 'b':
        options->b = 1;
        break;
      case 'e':
        options->e = 1;
        break;
      case 'n':
        options->n = 1;
        break;
      case 's':
        options->s = 1;
        break;
      case 't':
        options->t = 1;
        break;
      case 'E':
        options->E = 1;
        break;
      case 'T':
        options->T = 1;
        break;
      case 'v':
        options->v = 1;
        break;
      default:
        err = 1;
        break;
    }
  }
  if (!err) reader(argv, options);
}

void reader(char **argv, opt *options) {
  FILE *f;
  int i = optind, num = 1, nf = 0;
  while (argv[i]) {
    f = fopen(argv[i], "r");
    if (f)
      num = format_line(num, f, options, &nf);
    else
      printf("cat: %s: No such file ot directory\n", argv[i]);
    i++;
    fclose(f);
    nf = 1;
  }
}
int format_line(int line_count, FILE *f, opt *options, int *nf) {
  char c = getc(f), c_n = 0;
#ifdef __APPLE__
  line_count = 1;
#endif
  if (options->b && c != '\n') printf("%6d\t", line_count), line_count++;
#ifdef __linux__
  if (options->n && !options->b && (c == '\n' && !*nf))
    printf("%6d\t", line_count), line_count++;
#endif
#ifdef __APPLE__
  if (options->n && !options->b) printf("%6d\t", line_count), line_count++;
#endif
  fseek(f, -1, 1);
  while (c_n != EOF) {
    c = getc(f);
    if ((options->e || options->E) && c == '\n') printf("$");
    if ((options->T || options->t) && c == '\t') {
      printf("^I");
      c_n = getc(f);
      fseek(f, -1, 1);
      continue;
    }
    if (options->t || options->e || options->v) {
      if ((c >= 0 && c < 9) || (c > 10 && c < 32))
        printf("^%c", c + 64);
      else if (c == 127)
        printf("^?");
      else
        printf("%c", c);
    } else {
      printf("%c", c);
    }
    c_n = getc(f);
    if (options->s && c == '\n' && c_n == '\n') {
      while ((c_n = getc(f)) == '\n') {
      }
      if (options->n && !options->b) printf("%6d\t", line_count), line_count++;
      if (options->e || options->E) printf("$");
      printf("%c", c);
    }
    if (c == '\n' && options->n && c_n != EOF && !options->b)
      printf("%6d\t", line_count), line_count++;
    if ((c == '\n' && (c_n != '\n' && c_n != EOF)) && options->b)
      printf("%6d\t", line_count), line_count++;
    fseek(f, -1, 1);
  }
  return line_count;
}