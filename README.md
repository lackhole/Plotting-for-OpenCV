# OpenCV_Plotting
Plotting for OpenCV(C++)

Requirements :
        -OpenCV version 3.4.0 or above

In Plot.cpp, there is Canvas class for creating plot image.

Include Plot.cpp file to your main project.

Creating Canvas Examples:

        Canvas example1(600,800,"Example Plot");
        Canvas example2(700,700);
        Canvas example3;
        example3.create(400,500);
        Canvas example4;
        example4.create(500,600,"Example Plot");
        
After you declare a empty Canvas, you can draw a plot with number of dots, array of x and y data, and Scalar color(provided by OpenCV).


For example, let's assume that x[100] and y[100] is initialized.

        example1.draw(100,x,y,Scalar(0,0,0));
        example2.draw(30,x,y,Scalar(255,0,0));
        example3.draw(70,x,y,Scalar(0,0,255),"legend");
        
After you draw a plot, you have to call show() function.

        example1.show();
        example2.show();
        example3.show();
        You can also show multiple plot at once.
        Given example will show 3 plots.
        

Currently in develop! :D
