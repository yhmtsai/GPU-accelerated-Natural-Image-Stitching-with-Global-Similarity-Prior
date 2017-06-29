## GPU-accelerated Natural Image Stitching with Global Similarity Prior
This is the final project of 2017S GPGPU course. We follow this paper, Natural Image Stitching with Global Similarity Prior [1], and get its github source code[3]. We use GPU and change some settings to improve its performace.

## Profiling
We found the bottleneck of original version of image stitching are from three main part:
1. get feature pairs distance
2. least squares conjugate gradient solving
3. SVD

![Profiling Before](images/profiling-before.png)
## Change the getdistance function on GPU
Orignal version calculate the distance of each pair. We use linear algebra to express the function. It can be done by blas-2 and blas-3 operations.
![Get Distance Math](images/get_distance_math.png)
## Solve CG with initial guess
Original version use zero-initialization for iteration. We use vertices information as initial guess to accelerate the convergence of CG solving.
Below is the comparison between initial guess and zero-initialization under same iteration number.
Unfortunately, initial guess with vertices goes bad when iteration carry on, it seems that this thought only applys when we terminates the iteration at early phase.

Solarized dark             |  Solarized Ocean
:-------------------------:|:-------------------------:
![Get Distance Math](images/initial_guess.png) | ![Get Distance Math](images/zero_initialization.png)

## Change other settings
We found the linear systems are same. It just use different blending. Thus, we reduce the number of linear system. Moreover, human eyes can't distinguish tiny details, so we use less tolarence = 1e-8 not 1e-16 to improve the performance.
## Results
After all improvement, we can use 55 ~ 65 % time of orignal version.
Each step contains those improving steps before it.
![Time Percent](images/time_percent.png)
Below is the profiling after all improvement, we successfully reduce the percentage of distance computing and CG solving.

![Profiling After](images/profiling-after.png)
## Future work
1. Change the orignal data structure. Make it store data as matrix not many short vectors.
2. Change the solver to make it run another case.
3. 
## More detail
There is our presentation slides : [google slide](https://goo.gl/pu3Eih)
## Author
Yuhsiang Tsai, Institute of Applied Mathematical Sciences, National Taiwan University (yhmtsai@gmail.com)

ChiaHo Lin, Institute of Electronics Engineering, National Taiwan University (chiahoo2004@gmail.com)

## Reference
- [1] Yu-Sheng Chen and Yung-Yu Chuang. “Natural Image Stitching with Global Similarity Prior.” Proceedings of European Conference on Computer Vision 2016 (ECCV 2016)
- [2] Zaragoza, J., Chin, T.J., Brown, M.S., Suter, D.: As-projective-as-possible image stitching with moving DLT. In: Proceedings of the 2013 IEEE Conference on Computer Vision and Pattern Recognition, CVPR 2013, pp. 2339–2346 (2013)
- [3] [github source](https://github.com/nothinglo/NISwGSP), C++ implementation of [1]

## Welcome to GitHub Pages

You can use the [editor on GitHub](https://github.com/yhmtsai/GPU-accelerated-Natural-Image-Stitching-with-Global-Similarity-Prior/edit/master/docs/README.md) to maintain and preview the content for your website in Markdown files.

Whenever you commit to this repository, GitHub Pages will run [Jekyll](https://jekyllrb.com/) to rebuild the pages in your site, from the content in your Markdown files.

### Markdown

Markdown is a lightweight and easy-to-use syntax for styling your writing. It includes conventions for

```markdown
Syntax highlighted code block

# Header 1
## Header 2
### Header 3

- Bulleted
- List

1. Numbered
2. List

**Bold** and _Italic_ and `Code` text

[Link](url) and ![Image](src)
```

For more details see [GitHub Flavored Markdown](https://guides.github.com/features/mastering-markdown/).

### Jekyll Themes

Your Pages site will use the layout and styles from the Jekyll theme you have selected in your [repository settings](https://github.com/yhmtsai/GPU-accelerated-Natural-Image-Stitching-with-Global-Similarity-Prior/settings). The name of this theme is saved in the Jekyll `_config.yml` configuration file.

### Support or Contact

Having trouble with Pages? Check out our [documentation](https://help.github.com/categories/github-pages-basics/) or [contact support](https://github.com/contact) and we’ll help you sort it out.
