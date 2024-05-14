# Number Classifier

* A machine learning model for classifying handwritten digits

---

## Short description

* This application trains a neural network on **mnist** database of handwritten digits and produces a neural network model than can classify which digit appears on the given image

--- 

# MNIST database
* For my own training and testing I used the **mnist database** obtained from **kaggle** - https://www.kaggle.com/datasets/jidhumohan/mnist-png
* I downloaded the `archive` and placed its content into `assets/mnist-images`

---

## Build And Usage


* The project can be built using the provided *build.sh* script
```bash
./build.sh
```
* To run the program execute
```bash
./classifier
```
---

## Dependencies
* This project uses Raylib (`raylib v5.1-dev`) for its visualisaion needs
    * Please refer to [Raylib build/installation instructions](https://github.com/raysan5/raylib?tab=readme-ov-file#build-and-installation) 

---

# Project Plans
* [x] Setup basic window with Raylib
* [x] Introduce `nui.h` and `nf.h` as libraries
* [x] Introduce `mnist` database
* [x] Setup basic neural network
* [x] Read two images
    * Use argv to supply the path to the images
* [x] Render two images with `nui.h` ~ ui library for neural network visualisaion based on raylib 
* [x] Train the basic neural network on two images
* [ ] Visualize the training of the neural netowrk using `nui.h`
* [ ] Read the third image (testing image)
    * Use argv to supply the path to the image
* [ ] Test the neural model with the testing image
* [ ] Render the result of the test in the app
* [ ] Introduce `tia.h` for loading multiple images into one single data structure 
* [ ] Use `tia.h` for loading all `mnist` training images
* [ ] Use `tia.h` for loading all `mnist` testing images
* [ ] Enable switching between random testing images for testing
* [ ] Validate the correctnes of the current training image
* [ ] Test validation on all testing images and count the amout of correct validations

---

## Acknowledgements

* This project was heavily inspired by TSoding's machine learing series and the machine learing framework used in the project is a heavily inspired recreation of TSoding's neural network framework
* This project also uses `nothings/stb`, more specifically `stb_image.h`, for it's image file loading

---

# References
* 3b1b machine learning series: https://youtube.com/playlist?list=PLZHQObOWTQDNU6R1_67000Dx_ZCJB-3pi&si=4IbV97Q8pbC5Ildy
* tsoding machine learning series: https://youtube.com/playlist?list=PLpM-Dvs8t0VZPZKggcql-MmjaBdZKeDMw&si=3kYCTFRk-8TX8tLN
* mnist database: http://yann.lecun.com/exdb/mnist/
* mnist images (kaggle): https://www.kaggle.com/datasets/jidhumohan/mnist-png
* nothings's stb: https://github.com/nothings/stb
