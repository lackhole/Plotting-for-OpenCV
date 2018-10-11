# Plotting with OpenCV
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

        example1.draw(100,x,y,Scalar(0,0,0));           // draw first 100 numbers of array x and y
        example1.draw(30,x,y,Scalar(255,0,0));          // draw first 30 numbers of array x and y
        example2.draw(70,x,y,Scalar(0,0,255),"legend"); // draw first 70 numbers of array x and y, with name shown with colors.
        
        Alert : ranges are set by first given plot.
        if you draw x[]={1,2,3} and y[]={-1,-2,-3} first, then draw x2[]={4,5,6},y2[]={7,8,9} later,
        the range is fixed to 1 to 3 and -3 to -1, so x2 and y2 is not visible.
        You can solve this by setting range manually, or draw big invisible(white) dummy plot first.
        
        
After you draw a plot, you have to call show() function.

        example1.show();
        example2.show();
        You can also show multiple plot at once.
        Given example will show 2 plots.
        
Additional Options:

        example1.grid(true)     // turns on grid
        example2.expand(false)  // turns off expanding axis range
        example3.range(0,10,20,50)      // set range manually.
        example1.write("C:\myfolder\","plot.png")       //save plot.png at C:\myfolder\
        
        


Currently in develop! :D
