#include "Header.h"
#include "Pixel.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

class BinaryReader {
	ifstream file;

public:
	BinaryReader(string filePath) {
		file.open(filePath, ios::in | ios::app | ios_base::binary);
		if (!file.is_open()) {
			cout << "Error, could not open file: " << filePath << endl;
			throw exception();
		}
	}

	void closeFile() {
		file.close();
	}

	template <typename T>
	T Read() {
		T temp;
		file.read((char*)&temp, sizeof(temp));
		return temp;
	}
};

class BinaryWriter {
	ofstream file;

public:
	BinaryWriter(string filePath) {
		file.open(filePath, ios_base::binary);
		if (!file.is_open()) {
			cout << "Error, could not open file: " << filePath << endl;
			throw exception();
		}
	}

	void closeFile() {
		file.close();
	}

	template <typename T>
	void Write(T data) {
		file.write((char*)&data, sizeof(data));
	}
};

void SaveImageData(string inputFile, Header& Info, vector<Pixel>& pixelImage) {
	BinaryReader pictureReader(inputFile);
	Header temp = Header();

	temp.idLength = pictureReader.Read<char>();
	temp.colorMapType = pictureReader.Read<char>();
	temp.dataTypeCode = pictureReader.Read<char>();
	temp.colorMapOrigin = pictureReader.Read<short>();
	temp.colorMapLength = pictureReader.Read<short>();
	temp.colorMapDepth = pictureReader.Read<char>();
	temp.xOrigin = pictureReader.Read<short>();
	temp.yOrigin = pictureReader.Read<short>();
	temp.width = pictureReader.Read<short>();
	temp.height = pictureReader.Read<short>();
	temp.bitsPerPixel = pictureReader.Read<char>();
	temp.imageDescriptor = pictureReader.Read<char>();

	Info = temp;

	Pixel pixelData;

	for (int i = 0; i < (Info.height * Info.width); i++) {
		pixelData.Red = pictureReader.Read<char>();
		pixelData.Green = pictureReader.Read<char>();
		pixelData.Blue = pictureReader.Read<char>();

		pixelImage.push_back(pixelData);
	}

	pictureReader.closeFile();
}

void WriteImage(string inputFile, Header Info, vector<Pixel>& pixelImage) {
	BinaryWriter pictureWriter(inputFile);

	pictureWriter.Write(Info.idLength);
	pictureWriter.Write(Info.colorMapType);
	pictureWriter.Write(Info.dataTypeCode);
	pictureWriter.Write(Info.colorMapOrigin);
	pictureWriter.Write(Info.colorMapLength);
	pictureWriter.Write(Info.colorMapDepth);
	pictureWriter.Write(Info.xOrigin);
	pictureWriter.Write(Info.yOrigin);
	pictureWriter.Write(Info.width);
	pictureWriter.Write(Info.height);
	pictureWriter.Write(Info.bitsPerPixel);
	pictureWriter.Write(Info.imageDescriptor);

	for (int i = 0; i < (Info.height * Info.width); i++) {
		pictureWriter.Write(pixelImage[i].Red);
		pictureWriter.Write(pixelImage[i].Green);
		pictureWriter.Write(pixelImage[i].Blue);
	}

	pixelImage.clear();

	pictureWriter.closeFile();
}

vector<Pixel> Multiply(vector<Pixel>& topImage, vector<Pixel>& botImage, Header Info) {   //Multiplies all the pixels
	vector<Pixel> combinedImage;
	Pixel tempPixel;

	for (int i = 0; i < (Info.height * Info.width); i++) {
		tempPixel.Red = (char)(((topImage[i].Red / 255.0) * (botImage[i].Red / 255.0)) * 255 + 0.5f);
		tempPixel.Green = (char)(((topImage[i].Green / 255.0) * (botImage[i].Green / 255.0)) * 255 + 0.5f);
		tempPixel.Blue = (char)(((topImage[i].Blue / 255.0) * (botImage[i].Blue / 255.0)) * 255 + 0.5f);

		combinedImage.push_back(tempPixel);
	}

	topImage.clear();
	botImage.clear();

	return combinedImage;
	//WHY DO I NOT NEED THE 0.5F WHEN CALCULATING?
}

vector<Pixel> Subtract(vector<Pixel>& topImage, vector<Pixel>& botImage, Header Info) {   //Subtracts the TOP layer FROM the BOTTOM layer
	vector<Pixel> combinedImage;
	Pixel tempPixel;

	for (int i = 0; i < (Info.height * Info.width); i++) {
		if (((int)botImage[i].Red - (int)topImage[i].Red) < 0) {
			tempPixel.Red = 0;
		}
		else {
			tempPixel.Red = (char)(botImage[i].Red - topImage[i].Red);
		}

		if (((int)botImage[i].Green - (int)topImage[i].Green) < 0) {
			tempPixel.Green = 0;
		}
		else {
			tempPixel.Green = (char)(botImage[i].Green - topImage[i].Green);
		}

		if (((int)botImage[i].Blue - (int)topImage[i].Blue) < 0) {
			tempPixel.Blue = 0;
		}
		else {
			tempPixel.Blue = (char)(botImage[i].Blue - topImage[i].Blue);
		}

		combinedImage.push_back(tempPixel);
	}
	topImage.clear();
	botImage.clear();

	return combinedImage;
}

vector<Pixel> Screen(vector<Pixel>& topImage, vector<Pixel>& botImage, Header Info) {   //Opposite of Multiply
	vector<Pixel> combinedImage;
	Pixel tempPixel;

	for (int i = 0; i < (Info.height * Info.width); i++) {
		tempPixel.Red = (char)((1 - ((1 - topImage[i].Red / 255.0) * (1 - botImage[i].Red / 255.0))) * 255.0 + 0.5f);
		tempPixel.Green = (char)((1 - ((1 - topImage[i].Green / 255.0) * (1 - botImage[i].Green / 255.0))) * 255.0 + 0.5f);
		tempPixel.Blue = (char)((1 - ((1 - topImage[i].Blue / 255.0) * (1 - botImage[i].Blue / 255.0))) * 255.0 + 0.5f);

		combinedImage.push_back(tempPixel);
	}
	topImage.clear();
	botImage.clear();

	return combinedImage;
}

vector<Pixel> Overlay(vector<Pixel>& topImage, vector<Pixel>& botImage, Header Info) {
	vector<Pixel> combinedImage;
	Pixel tempPixel;

	for (int i = 0; i < (Info.height * Info.width); i++) {

		if (((botImage[i].Red / 255.0) <= 0.5) && ((botImage[i].Green / 255.0) <= 0.5) && ((botImage[i].Blue / 255.0) <= 0.5)) {
			tempPixel.Red = (char)((2 * ((topImage[i].Red / 255.0) * (botImage[i].Red / 255.0))) * 255.0 + 0.5f);
			tempPixel.Green = (char)((2 * ((topImage[i].Green / 255.0) * (botImage[i].Green / 255.0))) * 255.0 + 0.5f);
			tempPixel.Blue = (char)((2 * ((topImage[i].Blue / 255.0) * (botImage[i].Blue / 255.0))) * 255.0 + 0.5f);
		}

		else if (((botImage[i].Red / 255.0) > 0.5) && ((botImage[i].Green / 255.0) > 0.5) && ((botImage[i].Blue / 255.0) > 0.5)) {
			tempPixel.Red = (char)(((1 - 2 * ((1 - topImage[i].Red / 255.0) * (1 - botImage[i].Red / 255.0)))) * 255.0 + 0.5f);
			tempPixel.Green = (char)(((1 - 2 * ((1 - topImage[i].Green / 255.0) * (1 - botImage[i].Green / 255.0)))) * 255.0 + 0.5f);
			tempPixel.Blue = (char)(((1 - 2 * ((1 - topImage[i].Blue / 255.0) * (1 - botImage[i].Blue / 255.0)))) * 255.0 + 0.5f);
		}
		/*tempPixel.Red = (char)(((1 - 2 * ((1 - topImage[i].Red / 255.0) * (1 - botImage[i].Red / 255.0)))) * 255.0);
		tempPixel.Green = (char)(((1 - 2 * ((1 - topImage[i].Green / 255.0) * (1 - botImage[i].Green / 255.0)))) * 255.0);
		tempPixel.Blue = (char)(((1 - 2 * ((1 - topImage[i].Blue / 255.0) * (1 - botImage[i].Blue / 255.0)))) * 255.0);*/

		combinedImage.push_back(tempPixel);
	}
	topImage.clear();
	botImage.clear();

	return combinedImage;
}

vector<Pixel> Scale(vector<Pixel>& imageEdit, string color, int scale, Header Info) {
	vector<Pixel> combinedImage;
	Pixel tempPixel;

	if (color == "red") {
		for (int i = 0; i < (Info.height * Info.width); i++) {
			if (((float)imageEdit[i].Red * scale) > 255) {
				//imageEdit[i].Red = 255;
				tempPixel.Red = 255;
				tempPixel.Green = imageEdit[i].Green;
				tempPixel.Blue = imageEdit[i].Blue;
			}
			else {
				//imageEdit[i].Red = (char)(imageEdit[i].Red * scale);
				/*imageEdit[i].Red = (char)((float)imageEdit[i].Red * scale / 255.0);*/
				tempPixel.Red = (char)(((imageEdit[i].Red / 255.0 ) * scale) * 255.0 + 0.5f);
				tempPixel.Green = imageEdit[i].Green;
				tempPixel.Blue = imageEdit[i].Blue;
			}
			combinedImage.push_back(tempPixel);
		}
	}
	else if (color == "green") {
		for (int i = 0; i < (Info.height * Info.width); i++) {
			if (((float)imageEdit[i].Green * scale) >= 255) {
				//imageEdit[i].Red = 255;
				tempPixel.Red = imageEdit[i].Red;
				tempPixel.Green = 255;
				tempPixel.Blue = imageEdit[i].Blue;
			}
			else {
				//imageEdit[i].Red = (char)(imageEdit[i].Red * scale);
				/*imageEdit[i].Red = (char)((float)imageEdit[i].Red * scale / 255.0);*/
				tempPixel.Red = imageEdit[i].Red;
				tempPixel.Green = (char)(imageEdit[i].Green * scale);
				tempPixel.Blue = imageEdit[i].Blue;
			}
			combinedImage.push_back(tempPixel);
		}
	}
	else if (color == "blue") {
		for (int i = 0; i < (Info.height * Info.width); i++) {
			//imageEdit[i].Blue *= scale;
			if (((float)imageEdit[i].Blue * scale) > 255) {
				//imageEdit[i].Red = 255;
				tempPixel.Red = imageEdit[i].Red;
				tempPixel.Green = imageEdit[i].Green;
				tempPixel.Blue = 255;
			}
			else {
				//imageEdit[i].Red = (char)(imageEdit[i].Red * scale);
				/*imageEdit[i].Red = (char)((float)imageEdit[i].Red * scale / 255.0);*/
				tempPixel.Red = imageEdit[i].Red;
				tempPixel.Green = imageEdit[i].Green;
				tempPixel.Blue = (char)(((imageEdit[i].Blue / 255.0) * scale) * 255.0 + 0.5f);
			}
			combinedImage.push_back(tempPixel);
		}
	}
	imageEdit.clear();

	return combinedImage;
}

void colorSeparation(vector<Pixel>& pixelImage, Header Info, string redFile, string greenFile, string blueFile) {
	Pixel tempPixel;
	vector<Pixel> red;
	vector<Pixel> green;
	vector<Pixel> blue;

	for (int i = 0; i < (Info.height * Info.width); i++) {
		tempPixel.Red = pixelImage[i].Red;
		tempPixel.Green = pixelImage[i].Red;
		tempPixel.Blue = pixelImage[i].Red;
		red.push_back(tempPixel);
	}

	for (int i = 0; i < (Info.height * Info.width); i++) {
		tempPixel.Red = pixelImage[i].Green;
		tempPixel.Green = pixelImage[i].Green;
		tempPixel.Blue = pixelImage[i].Green;
		green.push_back(tempPixel);
	}

	for (int i = 0; i < (Info.height * Info.width); i++) {
		tempPixel.Red = pixelImage[i].Blue;
		tempPixel.Green = pixelImage[i].Blue;
		tempPixel.Blue = pixelImage[i].Blue;
		blue.push_back(tempPixel);
	}
	pixelImage.clear();

	WriteImage(redFile, Info, red);
	WriteImage(greenFile, Info, green);
	WriteImage(blueFile, Info, blue);
}

vector<Pixel> imageCombination(vector<Pixel>& redImage, vector<Pixel>& greanImage, vector<Pixel>& blueImage, Header Info) {
	vector<Pixel> combinedImage;
	Pixel tempPixel;

	for (int i = 0; i < (Info.height * Info.width); i++) {
		tempPixel.Red = redImage[i].Red;
		tempPixel.Green = greanImage[i].Green;
		tempPixel.Blue = blueImage[i].Blue;

		combinedImage.push_back(tempPixel);
	}
	redImage.clear();
	greanImage.clear();
	blueImage.clear();

	return combinedImage;
}

int main() {

	//----- Task 1 -----//

	string inputFile1 = "input/layer1.tga";
	string inputFile2 = "input/pattern1.tga";


	Header Info = Header();
	vector<Pixel> pixelImage1;
	vector<Pixel> pixelImage2;

	SaveImageData(inputFile1, Info, pixelImage1);
	SaveImageData(inputFile2, Info, pixelImage2);

	string outputFile = "output/part1.tga";

	vector<Pixel> finalImage = Multiply(pixelImage1, pixelImage2, Info);

	WriteImage(outputFile, Info, finalImage);


	////----- Task 2 -----//

	//inputFile1 = "input/layer2.tga";
	//inputFile2 = "input/car.tga";

	//SaveImageData(inputFile1, Info, pixelImage1);
	//SaveImageData(inputFile2, Info, pixelImage2);

	//outputFile = "output/part2.tga";

	//finalImage = Subtract(pixelImage1, pixelImage2, Info);

	//WriteImage(outputFile, Info, finalImage);
	//

	////----- Task 3 -----//

	/*inputFile1 = "input/layer1.tga";
	inputFile2 = "input/pattern2.tga";

	SaveImageData(inputFile1, Info, pixelImage1);
	SaveImageData(inputFile2, Info, pixelImage2);
	finalImage = Multiply(pixelImage1, pixelImage2, Info);

	inputFile1 = "input/text.tga";

	SaveImageData(inputFile1, Info, pixelImage1);
	finalImage = Screen(finalImage, pixelImage1, Info);

	outputFile = "output/part3.tga";
	WriteImage(outputFile, Info, finalImage);*/


	////----- Task 4 -----//

	//inputFile1 = "input/layer2.tga";
	//inputFile2 = "input/circles.tga";

	//SaveImageData(inputFile1, Info, pixelImage1);
	//SaveImageData(inputFile2, Info, pixelImage2);
	//finalImage = Multiply(pixelImage1, pixelImage2, Info);

	//inputFile1 = "input/pattern2.tga";

	//SaveImageData(inputFile1, Info, pixelImage1);
	//finalImage = Subtract(pixelImage1, finalImage, Info);

	//outputFile = "output/part4.tga";
	//WriteImage(outputFile, Info, finalImage);

	//----- Task 5 -----//  

	/*inputFile1 = "input/layer1.tga";
	inputFile2 = "input/pattern1.tga";

	SaveImageData(inputFile1, Info, pixelImage1);
	SaveImageData(inputFile2, Info, pixelImage2);
	finalImage = Overlay(pixelImage1, pixelImage2, Info);

	outputFile = "output/part5.tga";
	WriteImage(outputFile, Info, finalImage);*/


	//----- Task 6 -----//

	/*inputFile1 = "input/car.tga";

	SaveImageData(inputFile1, Info, pixelImage1);

	for (int i = 0; i < (Info.height * Info.width); i++) {
		if ((int)pixelImage1[i].Green + 200 > 255) {
			pixelImage1[i].Green = 255;
		}
		else {
			pixelImage1[i].Green += 200;
		}
	}

	outputFile = "output/part6.tga";
	WriteImage(outputFile, Info, pixelImage1); */

	//----- Task 7 -----// *****************************************

	inputFile1 = "input/car.tga";

	SaveImageData(inputFile1, Info, pixelImage1);

	pixelImage2 = Scale(pixelImage1, "red", 4, Info);
	finalImage = Scale(pixelImage2, "blue", 0, Info);

	outputFile = "output/part7.tga";
	WriteImage(outputFile, Info, finalImage);

	//----- Task 8 -----//

	/*inputFile1 = "input/car.tga";

	SaveImageData(inputFile1, Info, pixelImage1);

	colorSeparation(pixelImage1, Info, "output/part8_r.tga", "output/part8_g.tga", "output/part8_b.tga");*/


	//----- Task 9 -----//

	/*inputFile1 = "input/layer_red.tga";
	inputFile2 = "input/layer_green.tga";
	string inputFile3 = "input/layer_blue.tga";

	vector<Pixel> pixelImage3;

	SaveImageData(inputFile1, Info, pixelImage1);
	SaveImageData(inputFile2, Info, pixelImage2);
	SaveImageData(inputFile3, Info, pixelImage3);

	finalImage = imageCombination(pixelImage1, pixelImage2, pixelImage3, Info);

	outputFile = "output/part9.tga";
	WriteImage(outputFile, Info, finalImage);*/

	//----- Task 10 -----//

}