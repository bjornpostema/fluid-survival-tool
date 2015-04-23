# Installation #

  * Install GNU plot and libmatheval in the software centre or in the terminal
`> sudo apt-get install gnuplot-x11 libmatheval1`
  * A version of QT Core and QT GUI might be required to install for the GUI version
`> sudo apt-get install libqtcore4 libqtgui4`
  * Make sure you have the right binary executable, x86\_64 is 64-bit version x86 is 32-bit version. (The GUI version only supports 64-bit)
`> uname -a`
  * Make the file executable if that is not the case. Go to the directory of the executable binary file in your terminal and type:
`> sudo chmod +x ./HPnG`.
  * Download the most recent executable file from the download tab http://code.google.com/p/fluid-survival-tool/downloads/list or compile your code from the source in the SVN from http://code.google.com/p/fluid-survival-tool/source/checkout .
  * Please report all the bugs, desired features or unexpected behaviour you encounter by creating an new issue http://code.google.com/p/fluid-survival-tool/issues/list .

# Models #

  * Obtain a model from the downloads http://code.google.com/p/fluid-survival-tool/downloads/list or from the SVN http://code.google.com/p/fluid-survival-tool/source/checkout.
  * Each model file describes the places, transitions and arcs of the HPnGs. Above the example models you can find a description of the syntax for each of these. Before understanding the syntax knowledge of the definitions and limitations of HPnGs is required.

# Execution #

  * Assume the executable is in the current folder, run the executable with the command: ./HPnG path\_to\_model/modelfilename.m fluid\_place\_name
  * If GNU plot is installed correctly a 3D plot should pop-up. Also a file is generated that contains the information that is in the plot.
  * If you use the latest GUI version of the tool you can run it either through the terminal or by double clicking ./FST