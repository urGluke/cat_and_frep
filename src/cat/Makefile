CO = s21_cat
CC = s21_cat.c
s21_cat: $(CC)
	gcc -o $(CO) $(CC) 
	chmod +x $(CO)
clean:
	rm -rf log.txt
	rm -rf $(CO) s21_cat.txt cat.txt
test: t_cat.sh
	chmod +x t_cat.sh
	./t_cat.sh
leak: t_cat_leaks.sh
	chmod +x t_cat_leaks.sh
	./t_cat_leaks.sh
all: clean s21_cat