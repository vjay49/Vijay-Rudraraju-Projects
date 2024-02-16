//Driver Program

#include "Matrix.h"
#include "Image.h"
#include "processing.h"
//#include "Matrix_test_helpers.h"
//#include "Image_test_helpers.h"
#include <fstream>
#include <cstdlib>

using namespace std;

int main(int argc, char *argv[]){
    Image *img = new Image;
    
    //read from file
    string filename = argv[1];
    ifstream fin;
    fin.open(filename);
    if (!fin.is_open()) {
        cout << "Error opening file: " << filename << endl;
        delete img;
        return 1;
    }
    Image_init(img, fin);
    fin.close();
    
    if(argv[4] != 0){
        if(((argc == 4) || (argc == 5)) && ((atoi(argv[3]) > 0) &&
            (atoi(argv[3]) <= Image_width(img)))
            && ((atoi(argv[4]) > 0)
            && (atoi(argv[4]) <= Image_height(img)))){
            //cout << Image_width(img) << " top";
            seam_carve(img, atoi(argv[3]), atoi(argv[4]));
        }
        else{
            //cout << Image_width(img) << " second";
            cout << "Usage: resize.exe IN_FILENAME OUT_FILENAME WIDTH [HEIGHT]\n"
            << "WIDTH and HEIGHT must be less than or equal to original" << endl;
            delete img;
            return 1;
        }
    }
    else{
        if(((argc == 4) || (argc == 5)) && ((atoi(argv[3]) > 0)
            && (atoi(argv[3]) <= Image_width(img)))){
            //cout << Image_width(img) << " third";
            seam_carve(img, atoi(argv[3]), Image_height(img));
        }
        else{
            //cout << Image_width(img) << " 4th";
            cout << "Usage: resize.exe IN_FILENAME OUT_FILENAME WIDTH [HEIGHT]\n"
            << "WIDTH and HEIGHT must be less than or equal to original" << endl;
            delete img;
            return 1;
        }
    }
    
    //print to ppm file
    string outName = argv[2];
    
    ofstream out;
    out.open(outName);
    if (!out.is_open()) {
        cout << "Error opening file: " << outName << endl;
        delete img;
        return 1;
    }
    Image_print(img, out);
    out.close();
    
    delete img;
}
