# LRVM_Implementation-CS-6210
Implementation of light weight recoverable memory.


This is the implementation of the paper LRVM [Light weight Recoverable Memory](https://people.eecs.berkeley.edu/~brewer/cs262/lrvm.pdf)  tweaked a little bit. 

supports opening a file, reading a file , writing a file , aborting a write transaction , syncing the logs, cleaning operation ( truncation of the logs), provides
speed and bounded persistance in the form of logs ( sync_write).
