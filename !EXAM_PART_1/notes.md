# Notes part 1 of Exam

## Results before personal optimizations:
## Results after adding openCl functions
```bash
***** Version Name *****     | min(us)   | mean(us)  | stddev    | GOp/s     |  GB/s     |   CPI     | speedup   |
- MxM with optimizations           |     22661 |     22968 | 1.00139% |  0.706059 | 0.0211818 |   14.1631 |         1 |
- MxM Multithreaded                |     21405 |     22371 | 5.58312% |  0.747489 | 0.0224247 |   13.3781 |   1.05868 |

```
## Results after adding openCl functions
- MxM Killian Avarages between 1866 (lowest i got) until 1950+-
```bash
***** Version Name *****     | min(us)   | mean(us)  | stddev    | GOp/s     |  GB/s     |   CPI     | speedup   |
MxM with optimizations           |     20595 |     22417 | 5.75456% |  0.776888 | 0.0233066 |   12.8719 |         1 |
MxM Multithreaded                |     20818 |     21178 | 2.40344% |  0.768566 |  0.023057 |   13.0112 |  0.989288 |
MxM Killian                      |      1866 |      1933 | 3.31092% |   8.57449 |  0.257235 |   1.16625 |    11.037 |
```

##Results after optimizing kernel code
```bash
***** Version Name *****     | min(us)   | mean(us)  | stddev    | GOp/s     |  GB/s     |   CPI     | speedup   |
MxM with optimizations           |     21020 |     21495 | 1.61433% |   0.76118 | 0.0228354 |   13.1375 |         1 |
MxM Multithreaded                |     20575 |     21286 | 3.02076% |  0.777643 | 0.0233293 |   12.8594 |   1.02163 |
MxM Killian                      |        65 |    200128 | 81.6103% |   246.154 |   7.38462 |  0.040625 |   323.385 |
```

