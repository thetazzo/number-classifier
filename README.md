# Number Classifier

* A machine learning model for classifying handwritten digits

---

* The model will is suppoesd to be trained on the *mnist* database of handwritten digits and the same database can also be used for testing the trained model
* The application uses *Raylib* as its "frontend framework" - visual display of the neural network model, its trainging/testing, etc.

---

* This project was heavily inspired by TSoding's machine learing series and the machine learing framework used in the project is a heavily inspired recreation of TSoding's neural network framework
* All inspirations for the project can be found in the *References* sestion of the **README**

---

## Dependencies

* This project uses Raylib (`raylib v5.1-dev`) for its rendering needs
    * Please refer to [Raylib build/installation instructions](https://github.com/raysan5/raylib?tab=readme-ov-file#build-and-installation) 

---

# Project Plans
* [x] Setup basic window with Raylib
* [x] Introduce `nui.h` and `nf.h` as libraries
* [ ] Introduce `mnist` database
* [ ] Setup basic neural network
* [ ] Read two images
    * Use argv to supply the path to the images
* [ ] Render two images with `nui.h` ~ ui library for neural network visualisaion based on raylib 
* [ ] Train the basic neural network on two images
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

# References
* 3b1b machine learning series: https://youtube.com/playlist?list=PLZHQObOWTQDNU6R1_67000Dx_ZCJB-3pi&si=4IbV97Q8pbC5Ildy
* tsoding machine learning series: https://youtube.com/playlist?list=PLpM-Dvs8t0VZPZKggcql-MmjaBdZKeDMw&si=3kYCTFRk-8TX8tLN
* mnist database: http://yann.lecun.com/exdb/mnist/
