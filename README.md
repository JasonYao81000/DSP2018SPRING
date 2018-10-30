# DSP2018SPRING
Fundamentals of Speech Signal Processing at NTU 2018 Spring.

# HW1

## ENVIRONMENT
r06922002@linux6.csie.ntu.edu.tw

## HOW TO EXECUTE
1. Make
```
cd hw1_r06922002/
make
```
2. Train
```
./train 1000 ../model_init.txt ../seq_model_01.txt ./model_01.txt
./train 1000 ../model_init.txt ../seq_model_02.txt ./model_02.txt
./train 1000 ../model_init.txt ../seq_model_03.txt ./model_03.txt
./train 1000 ../model_init.txt ../seq_model_04.txt ./model_04.txt
./train 1000 ../model_init.txt ../seq_model_05.txt ./model_05.txt
```
3. Test
```
./test ../modellist.txt ../testing_data1.txt ../result1.txt
./test ../modellist.txt ../testing_data2.txt ../result2.txt
```

## RESULTS
![iteration_acc.png](https://github.com/JasonYao81000/DSP2018SPRING/blob/master/figures/iteration_acc.png)
