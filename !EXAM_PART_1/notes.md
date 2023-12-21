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

## Results after optimizing kernel code
- MxM Killian runs between 10 - 68 (lowest and highest i got at this point)

```bash
***** Version Name *****     | min(us)   | mean(us)  | stddev    | GOp/s     |  GB/s     |   CPI     | speedup   |
MxM with optimizations           |     20561 |     20843 | 1.59286% |  0.778172 | 0.0233452 |   12.8506 |         1 |
MxM Multithreaded                |     20798 |     20923 | 0.425369% |  0.769305 | 0.0230791 |   12.9987 |  0.988605 |
MxM Killian                      |        10 |        34 | 70.5882% |      1600 |        48 |   0.00625 |    2056.1 |
```

