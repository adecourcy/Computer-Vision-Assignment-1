# Report
## Part 1
### How to Run
For 1.1 run "p2 1.1 inputfile outputfile"
For 1.2 run "p2 1.2 inputfile outputfile"
For 1.3 (add watermark) run "p2 1.3 inputfile outputfile add N"
For 1.3 (check watermark) run "p2 1.3 inputfile outputfile check N", where outputfile is a dummy

### Design Decisions
We used several "magic" parameters in part 1.3, including **l** (number of binary digits produced by a random number generator seeded with N), **r** (radius of circular pattern for fourier transform), **alpha** (multiplication constant in fourier transform) and **t** (threshold for correlation coefficient to be above)


Part 2
