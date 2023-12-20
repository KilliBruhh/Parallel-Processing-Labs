# Notes part 1 of Exam

## Results before personal optimizations:
    ***** Version Name *****     | min(us)   | mean(us)  | stddev    | GOp/s     |  GB/s     |   CPI     | speedup   |
- MxM with optimizations           |     22661 |     22968 | 1.00139% |  0.706059 | 0.0211818 |   14.1631 |         1 |
- MxM Multithreaded                |     21405 |     22371 | 5.58312% |  0.747489 | 0.0224247 |   13.3781 |   1.05868 |

## What do wee see in the fucntion:
- 3 for loops nested
  - All the same
  - Start at 0
  - must me smaller than N
  - and add 1 every loop
- N is defined at 200
  - Why?
    - size of the Matrix (200x200);
- so loop firts loops every 