# Eyespeak-project
C++ code using in Eyespeak Project 

In the code, you can see some /////ACAPELA/////  
and then commented code. In fact, that's the part that activate the vocal synthesis.
If you want to use it, you need a license. To have it, contact the "Acapela" group (http://www.acapela-group.com)

Another clarification is necessary : 
The prototype used to run this project was built by us. 
So, the camera was and the filmed image was inverted, that is why we used methods to reinvert it.

Now, if you want to use this code follow these steps (We ran it on CodeBlocks (Windows 7) ) :
1)Download OpenFrameworks (http://openframeworks.cc/) 
2)Download OpenCV (http://opencv.org/)
3)Create a new project using the OpenFramework ProjectGenerator (you will find it in : of_path\projectGenerator) 
  You don't have to select addons, just create a default project
4)Follow these steps to use openCV with CodeBlocks (that's the way we used) :
    
            Here is the step I followed to set code::blocks-10.04 global compiler setting for OpenCV, in Windows 7 64 bit.
      
          Open code::blocks go to Setting->Compiler and debugger, a popup window will open.
      
          Now select Search directories tab, and below select Compiler tab, now OpenCV include path, generally C:\opencv_path\build\include
      
          Click on Linker tab next to Compiler tab add OpenCV bin directory, like C:\opencv_path\build\x86\mingw\bin, here I used 32 bit binaries.
      
          Now open Linker settings tab and below Link libraries add OpenCV libraries, C:\opencv_path\build\x86\mingw\bin
      
      Note that the above steps are global compiler setting for code:blocks and if you want to configure for a single project, just right click on the project name, select Build option and follow above steps.
      
      And if you are using pre-built libraries there might be some issue with mingw, for me pre-built libraries works fine with Windows7 64-bit (32-bit binaries) but not on Windows 7 32-bit.
      
      Any way just follow above and see it's working, if no you may have to build your own libraries for mingw from source.
      
5) You can now run the project (Of course, you need a camera)


