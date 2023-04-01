#!/bin/bash
COUNTER_SUCCESS=0
COUNTER_FAIL=0
DIFF_RES=""
LEAKS="valgrind --quiet"
TEST_FILE="bytes.txt test.txt"
ENG="Files s21_cat.txt and cat.txt are identical"
RU="Файлы s21_cat.txt and cat.txt идентичны"
echo "$TEST_FILE"
rm -rf log.txt

for var in -e -s -t -v -b -n -E -T #--number-nonblank --number --squeeze-blank
do
          TEST1="$var $TEST_FILE"          
          $LEAKS ./s21_cat $TEST1 > s21_cat.txt
          cat $TEST1 > cat.txt
          DIFF_RES="$(diff -s s21_cat.txt cat.txt)"
          if [ "$DIFF_RES" == "$ENG" ]
            then
              (( COUNTER_SUCCESS++ ))              
            else              
              (( COUNTER_FAIL++ ))
              echo "$TEST1" >> log.txt
          fi
          rm s21_cat.txt cat.txt
done

for var in -b -e -n -s -t -v -E -T #--number-nonblank --number --squeeze-blank
do
  for var2 in -b -e -n -s -t -v -E -T #--number-nonblank --number --squeeze-blank
  do
        if [ $var != $var2 ]
        then
          TEST1="$var $var2 $TEST_FILE"
          $LEAKS ./s21_cat $TEST1 > s21_cat.txt
          cat $TEST1 > cat.txt
          DIFF_RES="$(diff -s s21_cat.txt cat.txt)"
          if [ "$DIFF_RES" == "$ENG" ]
            then
              (( COUNTER_SUCCESS++ ))             
            else 
              echo "$TEST1" >> log.txt             
              (( COUNTER_FAIL++ ))
          fi
          rm s21_cat.txt cat.txt
        fi
  done
done

for var in -b -e -n -s -t -v -E -T #--number-nonblank --number --squeeze-blank
do
  for var2 in -b -e -n -s -t -v -E -T #--number-nonblank --number --squeeze-blank
  do
      for var3 in -b -e -n -s -t -v -E -T #--number-nonblank --number --squeeze-blank
      do
        if [ $var != $var2 ] && [ $var2 != $var3 ] && [ $var != $var3 ]
        then
          TEST1="$var $var2 $var3 $TEST_FILE"
          $LEAKS ./s21_cat $TEST1 > s21_cat.txt
          cat $TEST1 > cat.txt
          DIFF_RES="$(diff -s s21_cat.txt cat.txt)"
          if [ "$DIFF_RES" == "$ENG" ]
            then
              (( COUNTER_SUCCESS++ ))              
            else
              echo "$TEST1" >> log.txt              
              (( COUNTER_FAIL++ ))
          fi
          rm s21_cat.txt cat.txt
        fi
      done
  done
done

for var in -b -e -n -s -t -v -E -T #--number-nonblank --number --squeeze-blank
do
  for var2 in -b -e -n -s -t -v -E -T #--number-nonblank --number --squeeze-blank
  do
      for var3 in -b -e -n -s -t -v -E -T #--number-nonblank --number --squeeze-blank
      do
          for var4 in -b -e -n -s -t -v -E -T #--number-nonblank --number --squeeze-blank
          do
            if [ $var != $var2 ] && [ $var2 != $var3 ] && [ $var != $var3 ] && [ $var != $var4 ] && [ $var2 != $var4 ] && [ $var3 != $var4 ]
            then
              TEST1="$var $var2 $var3 $var4 $TEST_FILE"
              $LEAKS ./s21_cat $TEST1 > s21_cat.txt
              cat $TEST1 > cat.txt
              DIFF_RES="$(diff -s s21_cat.txt cat.txt)"
              if [ "$DIFF_RES" == "$ENG" ]
                then
                  (( COUNTER_SUCCESS++ ))                  
                else   
                  echo "$TEST1" >> log.txt              
                  (( COUNTER_FAIL++ ))
              fi
              rm s21_cat.txt cat.txt
            fi
          done
      done
  done
done

echo "SUCCESS: $COUNTER_SUCCESS"
echo "FAIL: $COUNTER_FAIL"