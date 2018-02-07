# A1 Report
#### Alex DeCourcy (adecourc) and Katie Spoon (kspoon)  
## Part 1
### How to Run
For 1.1 run "./watermark p2 1.1 inputfile outputfile"  
For 1.2 run "./watermark p2 1.2 inputfile outputfile"  
For 1.3 (add watermark) run "./watermark p2 1.3 inputfile outputfile add N"  
For 1.3 (check watermark) run "./watermark p2 1.3 inputfile outputfile check N", which checks inputfile for N

### Design Decisions
We used several "magic" parameters in part 1.3, including **l** (number of binary digits produced by a random number generator seeded with N), **r** (radius of circular pattern for fourier transform), **alpha** (multiplication constant in fourier transform) and **t** (threshold for correlation coefficient to be above). Here we will explain our reasoning for the values we chose for each one.
* **l = 5**: we found that with high values of **l**, strange artifacts (mostly patterns of dots) would appear in the resulting image
* **r = 40**: we found that low values or **r** were necessary for our program to detect the watermark in the image, but high values of **r** combined with high values of **alpha** resulted in odd circular artifacts (rows of blobs) in the background of the image
* **alpha = 2**: as mentioned before, high values of **alpha** resulted in weird artifacts (whether **r** was high as well or not), so we knew we needed a low **alpha**. However, we struggled because in order to detect the watermark in the image, higher values of **alpha** were helpful. We compromised by using a lower **alpha** but also lowering **r**.
* **t = 0.5**: we found that the threshold needed to be very high in order to weed out false positives. However, it needed to be low enough to actually detect the correct watermark. 0.5 was the highest it could be in order to still detect the watermark. See the experiment we did to test this in Performance Evaluation (Quantitative Results).

### Performance Evaluation
For 1.2, we hard-coded the removal of the noise (allowable by the assignment), but it could've been done in a more sophisticated way.

For 1.3, below is a summary (both qualitative and quantitative) of the parameters (**l**, **r**, **alpha** and **t**) we chose.  
#### Qualitative Results (Example Images)
##### Original Image (Tree):
![alt text](https://github.iu.edu/cs-b657-sp2018/adecourc-kspoon-a1/blob/master/part1/watermark_images/tree_watermark_original.png)

##### For high value of l:
![alt text](https://github.iu.edu/cs-b657-sp2018/adecourc-kspoon-a1/blob/master/part1/watermark_images/tree_watermark_high_l_value.png)

##### For low value of r:
![alt text](https://github.iu.edu/cs-b657-sp2018/adecourc-kspoon-a1/blob/master/part1/watermark_images/tree_watermark_low_r_value.png)

##### For high value of alpha:
![alt text](https://github.iu.edu/cs-b657-sp2018/adecourc-kspoon-a1/blob/master/part1/watermark_images/tree_watermark_high_alpha_value.png)

One other important assumption that we make with this specific set of parameters is that the png images that are used do not have transparent backgrounds. See an example below.
##### Original Image (person - with transparent background):
![alt text](https://github.iu.edu/cs-b657-sp2018/adecourc-kspoon-a1/blob/master/part1/watermark_images/people.png)

##### For set of parameters above (l = 5, r = 40, alpha = 2, t = 0.5):
![alt text](https://github.iu.edu/cs-b657-sp2018/adecourc-kspoon-a1/blob/master/part1/watermark_images/people_watermark.png)

#### Quantitative Results 
To test the false positive rate of our parameters, we embedded the watermark "35" into the tree image above using the "add" operation, then tested all integers 1-100 with the "check" operation. We had 7 false positives (16, 21, 40, 43, 49, 86, and 100), resulting in a false positive rate of 7%.
  
## Part 2
### How to Run

./detect input_image output_image

### Design Decisions

We deviated quite a bit from the suggestions in the PDF in this assignment. We found that using a simple blur and edge filter alone resulting in images that were extremely noisy and very difficult to use. We attempted to use very conservative and very aggressive blurring, with gaussians with a deviation of 3, 5, 7, 9, and mean filters of size 3, 5, 7, and 11, however this didn't seem to drastically improve our results with the edge filter. We even tried doing various forms of edge filters, making them smaller and larger but, again, the improvement wasn't very great. That's not to say that there was no improvement at all, just that it brought the noise level from very bad to pretty bad.

We were asked on Piazza to compare our method to the suggested method in the PDF, however we found that it would have taken a very large amount of work to get results from the suggested PDF method and it was very obvious that those results would be very poor. As we had already spent a large amount of time trying to improve the results through the use of different blurs and edge filters, we decided to finish the project using our modified method and only report those results here.I

We started by importing the image as a color image. We applied an aggressive blur to each color plane though the use of a mean filter. We then passed these color planes to a function that filtered out any pixel that wasn't "gray" enough given some parameters. This was a very conservative filter, erring on the side of leaving too much noise behind rather than than accidentally removing some signal from that noise. Any removed points were turned white. This color filter returned a grayscale image of the remaining pixels.

The remaining pixels were run through a clustering algorithm, which was a very simplified form of DBScan. This algorithm kept any point that was surrounded by all non-white points as a "core" point. It also kept any point that was in direct contact with any "core" point (which were called "edge" points in the code). This helped remove small, isolated patches of gray pixels that were left behind in the color filtering step.

After the clustering step, the image was turned to black and white. This helped solve some problems with gradient noise in the the grayscale image that couldn't be removed by the blurring, and caused false edges to be added during edge filtering.

Next, an edge filter was applied. There was still a large amount of noise, much of which was due to pins on the ICs or gradient noise in the IC images themselves, so a gaussian filter was applied to the results to blur the edges and "connect" various edge points to one another. Each edge blur was applied independent of the other (that is to say that the vertical and horizontal edges were blurred before being combined together).

### Performance Evaluation.


