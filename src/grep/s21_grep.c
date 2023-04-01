#include <getopt.h>
#include <pcre.h>
#include <stdio.h>
#include <string.h>

typedef struct options {
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int o;
  int e;
  int f;
} opt;

void get_option_lin(int argc, char **argv, opt *options);
void get_option_mac(int argc, char **argv, opt *options);
void reader(int f_ind, char examples[1024], char filename[256][1024],
            opt *options);
int f_opt_examples(opt *options, char *optarg, char f_examples[1024]);
int format_line(int f_line, char examples[1024], int *strings, char line[1024],
                char formatted_line[1024], opt *options, int n,
                char filename[1024]);
int find_pat(int op, char *line, int matches[1024], char samples[1024]);
void vopt_false(int f_line, int files, opt *options, char formatted_line[1024],
                char filename[1024], char line[1024], int matches[1024]);
void vopt_true(int f_line, int files, opt *options, char formatted_line[1024],
               char filename[1024], char line[1024]);

int main(int argc, char **argv) {
  opt options = {0};
#ifdef __APPLE__
  get_option_mac(argc, argv, &options);
#endif
#ifdef __linux__
  get_option_lin(argc, argv, &options);
#endif

  return 0;
}
void read_lines(char formatted_line[1024], opt *options, int f_ind,
                char filename[1024], char line[1024], FILE *f, int *f_line,
                char examples[1024], int *strings);

void get_option_lin(int argc, char **argv, opt *options) {
  int opt;
  int prev_opt = 0, f_ind = 0, any_ex = 0;
  char examples[1024] = {0};
  char filename[256][1024] = {0};
  while (optind < argc) {
    (opt = getopt(argc, argv, "-:e:f:ivclnhso"));
    switch (opt) {
      case 'e':
        strcat(examples, optarg);
        strcat(examples, "|");
        options->e = 1;
        any_ex = 1;
        prev_opt = 0;
        break;
      case 'i':
        options->i = 1;
        prev_opt = 1;
        break;
      case 'v':
        options->v = 1;
        prev_opt = 1;
        break;
      case 'c':
        options->c = 1;
        prev_opt = 1;
        break;
      case 'l':
        options->l = 1;
        prev_opt = 1;
        break;
      case 'n':
        options->n = 1;
        prev_opt = 1;
        break;
      case 'h':
        options->h = 1;
        prev_opt = 1;
        break;
      case 's':
        options->s = 1;
        prev_opt = 1;
        break;
      case 'f':
        f_opt_examples(options, optarg, examples);
        any_ex = 1;
        options->f = 1;
        prev_opt = 0;
        break;
      case 'o':
        options->o = 1;
        prev_opt = 1;
        break;
      case 1:
        if (prev_opt && !any_ex) {
          strcat(examples, optarg);
          any_ex = 1;
          prev_opt = 0;
        } else {
          if (!any_ex) {
            strcat(examples, optarg);
            any_ex = 1;
          } else {
            strcat(filename[f_ind++], optarg);
          }
        }
        break;
    }
  }
  if (options->e || (options->f && *examples)) {
    examples[strlen(examples) - 1] = '\0';
  }
  reader(f_ind, examples, filename, options);
}
void get_option_mac(int argc, char **argv, opt *options) {
  int opt, fl = 1;
  int prev_opt = 0, f_ind = 0, any_ex = 0;
  char examples[1024] = {0};
  char filename[256][1024] = {0};
  while (fl) {
    (opt = getopt(argc, argv, ":e:f:ivclnhso"));
    switch (opt) {
      case 'e':
        strcat(examples, optarg);
        strcat(examples, "|");
        options->e = 1;
        any_ex = 1;
        prev_opt = 0;
        break;
      case 'i':
        options->i = 1;
        prev_opt = 1;
        break;
      case 'v':
        options->v = 1;
        prev_opt = 1;
        break;
      case 'c':
        options->c = 1;
        prev_opt = 1;
        break;
      case 'l':
        options->l = 1;
        prev_opt = 1;
        break;
      case 'n':
        options->n = 1;
        prev_opt = 1;
        break;
      case 'h':
        options->h = 1;
        prev_opt = 1;
        break;
      case 's':
        options->s = 1;
        prev_opt = 1;
        break;
      case 'f':
        f_opt_examples(options, optarg, examples);
        any_ex = 1;
        options->f = 1;
        prev_opt = 0;
        break;
      case 'o':
        options->o = 1;
        prev_opt = 1;
        break;
      case -1:
        for (int i = optind; i < argc; i++) {
          if (prev_opt && !any_ex) {
            strcat(examples, argv[i]);
            any_ex = 1;
            prev_opt = 0;
          } else {
            if (!any_ex) {
              strcat(examples, argv[i]);
              any_ex = 1;
            } else {
              strcat(filename[f_ind++], argv[i]);
            }
          }
        }
        fl = 0;
        break;
    }
  }
  if (options->e || (options->f && *examples)) {
    examples[strlen(examples) - 1] = '\0';
  }
  reader(f_ind, examples, filename, options);
}
int f_opt_examples(opt *options, char *optarg, char f_examples[1024]) {
  FILE *f = fopen(optarg, "r");
  char line[1024] = {0};
  if (f) {
    while (fgets(line, 1024, f)) {
      strcat(f_examples, line);
      if (*f_examples && f_examples[strlen(f_examples) - 1] == '\n') {
        f_examples[strlen(f_examples) - 1] = '|';
      } else {
        f_examples[strlen(f_examples)] = '|';
      }
    }
    fclose(f);
  } else {
    if (!options->s) {
      printf("grep: %s: %s\n", optarg, "No such file or directory");
    }
  }
  return strlen(f_examples);
}

void reader(int f_ind, char examples[1024], char filename[256][1024],
            opt *options) {
  char formatted_line[1024] = {0};
  char line[1024] = {0};
  int strings;
  FILE *f;
  int i = 0, f_line;
  while (*filename[i]) {
    f = fopen(filename[i], "r");
    if (f) {
      strings = 0;
      f_line = 0;
      read_lines(formatted_line, options, f_ind, filename[i], line, f, &f_line,
                 examples, &strings);
      if (options->c && !options->l) {
        char c_line[1024] = {0};
        sprintf(c_line, "%d", strings);
        if (*formatted_line) {
          strcat(formatted_line, c_line);
        } else {
          sprintf(formatted_line, "%s", c_line);
        }
      }
      if (options->c || options->l) {
        strcat(formatted_line, "\n");
        if (fputs(formatted_line, stdout) == EOF && !options->s) {
          perror("grep");
          break;
        }
        *formatted_line = 0;
      }
      fclose(f);
    } else {
      if (!options->s) {
        printf("grep: %s: %s\n", filename[i], "No such file or directory");
      }
    }
    i++;
  }
}

int format_line(int f_line, char examples[1024], int *strings, char line[1024],
                char formatted_line[1024], opt *options, int files,
                char filename[1024]) {
  int matches[1024] = {0};
  int op, m;
  if (options->i) {
    op = PCRE_EXTENDED | PCRE_CASELESS;
  } else {
    op = PCRE_EXTENDED;
  }
  m = find_pat(op, line, matches, examples);
  if (options->v) {
    if (!m) {
      *strings += 1;
      if (options->l) {
        sprintf(formatted_line, "%s", filename);
      } else {
        vopt_true(f_line, files, options, formatted_line, filename, line);
      }
    }
  } else {
    if (m && *examples) {
      *strings += 1;
      if (options->l) {
        sprintf(formatted_line, "%s", filename);
      } else {
        vopt_false(f_line, files, options, formatted_line, filename, line,
                   matches);
      }
    }
  }
  return m;
}
void vopt_true(int f_line, int files, opt *options, char formatted_line[1024],
               char filename[1024], char line[1024]) {
  if (files > 1 && !options->h) {
    sprintf(formatted_line, "%s:", filename);
  }
  if (!options->c) {
    if (options->n) {
      char n_line[256] = {0};
      sprintf(n_line, "%d:", f_line);
      strcat(formatted_line, n_line);
    }
    strcat(formatted_line, line);
    if (formatted_line[strlen(formatted_line) - 1] != '\n') {
      strcat(formatted_line, "\n");
    }
  }
}
void vopt_false(int f_line, int files, opt *options, char formatted_line[1024],
                char filename[1024], char line[1024], int matches[1024]) {
  if (files > 1 && !options->h) {
    sprintf(formatted_line, "%s:", filename);
  }
  if (!options->c) {
    if (options->n) {
      char n_line[256] = {0};
      sprintf(n_line, "%d:", f_line);
      strcat(formatted_line, n_line);
    }
    if (options->o) {
      int i = 0;
      char o_line[1024] = {0}, eddited_line[1024] = {0};
      strcat(eddited_line, formatted_line);
      while (matches[i] || matches[i + 1]) {
        sprintf(o_line, "%.*s\n", matches[i + 1] - matches[i],
                line + matches[i]);
        strcat(formatted_line, o_line);
        i += 2;
      }
    } else {
      strcat(formatted_line, line);
    }
    if (formatted_line[strlen(formatted_line) - 1] != '\n') {
      strcat(formatted_line, "\n");
    }
  }
}

int find_pat(int op, char *line, int matches[1024], char samples[1024]) {
  const unsigned char *tables = NULL;
  tables = pcre_maketables();
  pcre *re;
  const char *error;
  int erroffset;
  re = pcre_compile((char *)samples, op, &error, &erroffset, NULL);
  int ovector[30] = {0};
  int offset = 0;
  int rc, n = 0;
  int len = strlen(line);
  int m = 0;
  while (offset < len &&
         (rc = pcre_exec(re, 0, line, len, offset, 0, ovector,
                         sizeof(ovector))) >= 0 &&
         n <= 1022) {
    matches[n++] = ovector[0];
    matches[n++] = ovector[1];
    offset = ovector[1];
    m = 1;
  }
  pcre_free((void *)re);
  pcre_free((void *)tables);
  return m;
}
void read_lines(char formatted_line[1024], opt *options, int f_ind,
                char filename[1024], char line[1024], FILE *f, int *f_line,
                char examples[1024], int *strings) {
  while (fgets(line, 1024, f)) {
    *f_line += 1;
    format_line(*f_line, examples, strings, line, formatted_line, options,
                f_ind, filename);
    if (!options->c && !options->l) {
      if (fputs(formatted_line, stdout) == EOF && !options->s) {
        perror("grep");
        break;
      }
      *formatted_line = 0;
    }
  }
}