# Report
## Part 1
### How to Run
For 1.1 run "p2 1.1 inputfile outputfile"  
For 1.2 run "p2 1.2 inputfile outputfile"  
For 1.3 (add watermark) run "p2 1.3 inputfile outputfile add N"  
For 1.3 (check watermark) run "p2 1.3 inputfile outputfile check N", where outputfile is a dummy  

### Design Decisions
We used several "magic" parameters in part 1.3, including **l** (number of binary digits produced by a random number generator seeded with N), **r** (radius of circular pattern for fourier transform), **alpha** (multiplication constant in fourier transform) and **t** (threshold for correlation coefficient to be above). Here we will explain our reasoning for the values we chose for each one.
* **l = 5**: we found that with high values of **l**, strange artifacts (mostly patterns of dots) would appear in the resulting image
* **r = 40**: we found that low values or **r** were necessary for our program to detect the watermark in the image, but high values of **r** combined with high values of **alpha** resulted in odd circular artifacts (rows of blobs) in the background of the image
* **alpha = 2**: as mentioned before, high values of **alpha** resulted in weird artifacts (whether **r** was high as well or not), so we knew we needed a low **alpha**. However, we struggled because in order to detect the watermark in the image, higher values of **alpha** were helpful. We compromised by using a lower **alpha** but also lowering **r**.
* **t = 0**: we found that the threshold needed to be very low in order to detect the watermark. By setting it to be 0, we had some concerns that our program would start detecting watermarks that weren't there (false positives), however that was not the case, so we decided on a threshold of 0.

### Performance Evaluation
  
## Part 2
### How to Run

### Design Decisions

### Performance Evaluation


