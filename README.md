# DSP2018SPRING
Fundamentals of Speech Signal Processing at NTU 2018 Spring.

This course has three homeworks.

# Table of Contents
<!--ts-->
   1. [Homework 1](https://github.com/JasonYao81000/DSP2018SPRING#homework-1)
   2. [Homework 2](https://github.com/JasonYao81000/DSP2018SPRING#homework-2)
   3. [Homework 3](https://github.com/JasonYao81000/DSP2018SPRING#homework-3)
<!--te-->

# Homework 1

## I. ENVIRONMENT
r06922002@linux6.csie.ntu.edu.tw

## II. HOW TO EXECUTE
### 2.1. Make
```
cd hw1_r06922002/
make
```

### 2.2. Train
```
./train 1000 ../model_init.txt ../seq_model_01.txt ./model_01.txt
./train 1000 ../model_init.txt ../seq_model_02.txt ./model_02.txt
./train 1000 ../model_init.txt ../seq_model_03.txt ./model_03.txt
./train 1000 ../model_init.txt ../seq_model_04.txt ./model_04.txt
./train 1000 ../model_init.txt ../seq_model_05.txt ./model_05.txt
```

### 2.3. Test
```
./test ../modellist.txt ../testing_data1.txt ../result1.txt
./test ../modellist.txt ../testing_data2.txt ../result2.txt
```

## III. RESULTS
![iteration_acc.png](https://github.com/JasonYao81000/DSP2018SPRING/blob/master/figures/iteration_acc.png)

***

# Homework 2

## I. ENVIRONMENT
r06922002@linux3.csie.ntu.edu.tw

## II. HOW TO EXECUTE
```
cd hw2_r06922002/
bash 00_clean_all.sh
bash 01_run_HCopy.sh
bash 02_run_HCompV.sh
bash 03_training.sh
bash 04_testing.sh
cat result/accuracy
```

## III. Run Baseline (40%)
![hw2_baseline.png](https://github.com/JasonYao81000/DSP2018SPRING/blob/master/figures/hw2_baseline.png)

## IV. Imporve Accuracy (40%)
![hw2_improved.png](https://github.com/JasonYao81000/DSP2018SPRING/blob/master/figures/hw2_improved.png)

***

# Homework 3

## I. ENVIRONMENT
r06922002@linux3.csie.ntu.edu.tw

## II. HOW TO COMPILE
```
cd hw3_r06922002/
make clean
copy TA’s bigram.lm, Big5-ZhuYin.map, testdata to ./hw3_r06922002/
make MACHINE_TYPE=i686-m64 SRIPATH=/home/master/06/r06922002/DSP2018Spring/srilm-1.5.10 all
```

## III. HOW TO EXECUTE
```
cd hw3_r06922002/
make clean
copy TA’s bigram.lm, Big5-ZhuYin.map, testdata to ./r06922002/
make MACHINE_TYPE=i686-m64 SRIPATH=/home/master/06/r06922002/DSP2018Spring/srilm-1.5.10 all
make map
make MACHINE_TYPE=i686-m64 SRIPATH=/home/master/06/r06922002/DSP2018Spring/srilm-1.5.10 run
```

## IV. WHAT I HAVE DONE
### 4.1. Segment corpus and all test data into characters
```
./separator_big5.pl corpus.txt > corpus_seg.txt
./separator_big5.pl testdata/xx.txt > xx.txt
```

### 4.2. Train character-based bigram LM (Bigram)
```
#!/bin/bash
SRIPATH="/home/master/06/r06922002/DSP2018Spring/srilm-1.5.10"
SRIPATH_BIN="$SRIPATH/bin/i686-m64"
$SRIPATH_BIN/ngram-count -text corpus_seg.txt -write lm.cnt -order 2
$SRIPATH_BIN/ngram-count -read lm.cnt -lm bigram.lm -unk -order 2
```

### 4.3. Generate ZhuYin-Big5.map from Big5-ZhuYin.map
```
python mapping.py Big5-ZhuYin.map ZhuYin-Big5.map
```
or
```
make map
```

### 4.4. Using disambig to decode testdata/xx.txt (Bigram)
```
#!/bin/bash
SRIPATH="/home/master/06/r06922002/DSP2018Spring/srilm-1.5.10"
SRIPATH_BIN="$SRIPATH/bin/i686-m64"
$SRIPATH_BIN/disambig -text testdata/xx.txt -map ZhuYin-Big5.map -lm bigram.lm -order 2 > result1/xx.txt
```

### 4.5. Using mydisambig to decode testdata/xx.txt (Bigram)
```
./mydisambig -text testdata/xx.txt -map ZhuYin-Big5.map -lm bigram.lm -order 2 > result2/xx.txt
```
or
```
make run
```

### 4.6. Results (Bigram)

![hw3_results_comparison.png](https://github.com/JasonYao81000/DSP2018SPRING/blob/master/figures/hw3_results_comparison.png)
