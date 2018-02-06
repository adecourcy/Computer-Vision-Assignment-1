# A1 Report
#### Alex DeCourcy (adecourc) and Katie Spoon (kspoon)  
## Part 1
### How to Run
For 1.1 run "p2 1.1 inputfile outputfile"  
For 1.2 run "p2 1.2 inputfile outputfile"  
For 1.3 (add watermark) run "p2 1.3 inputfile outputfile add N"  
For 1.3 (check watermark) run "p2 1.3 inputfile outputfile check N", which checks inputfile for watermark N

### Design Decisions
We used several "magic" parameters in part 1.3, including **l** (number of binary digits produced by a random number generator seeded with N), **r** (radius of circular pattern for fourier transform), **alpha** (multiplication constant in fourier transform) and **t** (threshold for correlation coefficient to be above). Here we will explain our reasoning for the values we chose for each one.
* **l = 5**: we found that with high values of **l**, strange artifacts (mostly patterns of dots) would appear in the resulting image
* **r = 40**: we found that low values or **r** were necessary for our program to detect the watermark in the image, but high values of **r** combined with high values of **alpha** resulted in odd circular artifacts (rows of blobs) in the background of the image
* **alpha = 2**: as mentioned before, high values of **alpha** resulted in weird artifacts (whether **r** was high as well or not), so we knew we needed a low **alpha**. However, we struggled because in order to detect the watermark in the image, higher values of **alpha** were helpful. We compromised by using a lower **alpha** but also lowering **r**.
* **t = 0**: we found that the threshold needed to be very low in order to detect the watermark. By setting it to be 0, we had some concerns that our program would start detecting watermarks that weren't there (false positives), however that was not the case, so we decided on a threshold of 0.

### Performance Evaluation
For 1.2, we hard-coded the removal of the noise (allowable by the assignment), but it could've been done in a more sophisticated way.

For 1.3, below is a summary (both qualitative and quantitative) of the parameters (**l**, **r**, **alpha** and **t**) we chose.  
#### Qualitative (Example Images)
##### Original Image (Tree):
<img src="https://github.iu.edu/cs-b657-sp2018/adecourc-kspoon-a1/blob/master/part1/watermark_images/tree_watermark_original.png" alt="Drawing" style="width: 256px; height: 256px;"/>

##### For high value of l:

##### For low value of r:

##### For high value of alpha:

##### For high value of t:

One other important assumption that we make with this specific set of parameters is that the png images that are used do not have transparent backgrounds. See an example below.
##### Original Image (person - with transparent background):

##### For set of parameters above (l = 5, r = 40, alpha = 2, t = 0):

#### Quantitative (100 Test Watermarks)
Here are the results of embedding the watermark 35 into the tree image above using the "add" operation, then testing all integers 1-100 with the "check" operation.
  
## Part 2
### How to Run

### Design Decisions

### Performance Evaluation


