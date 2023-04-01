GO = s21_grep
GC = s21_grep.c
s21_grep: $(GC)
	gcc -Wall -Werror -Wextra -o $(GO) $(GC) -lpcre
	chmod +x $(GO)
clean:
	rm -rf log.txt
	rm -rf $(GO) s21_grep.txt grep.txt
test: t_grep.sh
	chmod +x t_grep.sh
	./t_grep.sh
leak: t_grep_leaks.sh
	chmod +x t_grep_leaks.sh
	./t_grep_leaks.sh
all: clean s21_grep