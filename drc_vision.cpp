#include "OVR.h"
#include <iostream>
#include "opencv.hpp"
#include "highgui/highgui.hpp"
#include "imgproc/imgproc.hpp"


using namespace OVR;
using namespace std;

Ptr<DeviceManager>      pManager;
Ptr<HMDDevice>          pHMD;
Ptr<SensorDevice>       pSensor;
SensorFusion*           pFusionResult;
HMDInfo                 Info;
bool                    InfoLoaded;


void Init()
{

        //Oculus Rift Stuffs

        System::Init();

        pFusionResult = new SensorFusion();
        pManager = *DeviceManager::Create();

        pHMD = *pManager->EnumerateDevices<HMDDevice>().CreateDevice();

        if (pHMD)
        {
           InfoLoaded = pHMD->GetDeviceInfo(&Info);

           pSensor = *pHMD->GetSensor();
        }
        else
        {
           pSensor = *pManager->EnumerateDevices<SensorDevice>().CreateDevice();
        }

        if (pSensor)
        {
           pFusionResult->AttachToSensor(pSensor);
        }

        //Hubo-ach stuffs

}

cv::Mat* getFull(cv::Mat &right, cv::Mat &left)
{
	cv::Mat* full = new cv::Mat(cv::Size(1280,800),CV_8UC3);
	*full = cv::Scalar(255,0,0);
	double interPupillaryPixels = ((double)Info.InterpupillaryDistance*100.0*73.85);
	int width = interPupillaryPixels;
	resize(right,right,cv::Size(width,right.rows*((double)width/(double)right.cols)));
	int rightx=(full->cols/2);
	int righty=(full->rows-right.rows)/2;
        resize(left,left,cv::Size(width,left.rows*((double)width/(double)left.cols)));
	int leftx = (full->cols/2-left.cols);
	int lefty = (full->rows-left.rows)/2;
	//cout << "fs "<< full->cols << "x"<< full->rows << " rx: " << rightx << " ry: " << righty<< " rs: " << right.cols << "x" <<right.rows << " lx: " << leftx << " ly: " << lefty << " ls: " << left.cols<<"x"<<left.rows << endl;
	cv::Mat rightside(*full, cv::Rect(rightx, righty, right.cols, right.rows)); // Copy constructor
	right.copyTo(rightside);
	cv::Mat leftside(*full, cv::Rect(leftx, lefty, left.cols, left.rows)); // Copy constructor
	left.copyTo(leftside);
	return full;
}

void display(cv::Mat &right, cv::Mat &left)
{
	cv::imshow("right",right);
	cv::imshow("full",*getFull(right,left));
}

void Clear()
{
        pSensor.Clear();
        pHMD.Clear();
        pManager.Clear();

        delete pFusionResult;

        System::Destroy();
}

void Output()
{
        cout << "----- DRC Hubo Vision -----" << endl;

        if (pHMD)
        {
                cout << " [x] HMD Found" << endl;
        }
        else
        {
                cout << " [ ] HMD Not Found" << endl;
        }

        if (pSensor)
        {
                cout << " [x] Sensor Found" << endl;
        }
        else
        {
                cout << " [ ] Sensor Not Found" << endl;
        }

        cout << "--------------------------" << endl;

        if (InfoLoaded)
        {
                cout << " DisplayDeviceName: " << Info.DisplayDeviceName << endl;
                cout << " ProductName: " << Info.ProductName << endl;
                cout << " Manufacturer: " << Info.Manufacturer << endl;
                cout << " Version: " << Info.Version << endl;
                cout << " HResolution: " << Info.HResolution<< endl;
                cout << " VResolution: " << Info.VResolution<< endl;
                cout << " HScreenSize: " << Info.HScreenSize<< endl;
                cout << " VScreenSize: " << Info.VScreenSize<< endl;
                cout << " VScreenCenter: " << Info.VScreenCenter<< endl;
                cout << " EyeToScreenDistance: " << Info.EyeToScreenDistance << endl;
                cout << " LensSeparationDistance: " << Info.LensSeparationDistance << endl;
                cout << " InterpupillaryDistance: " << Info.InterpupillaryDistance << endl;
                cout << " DistortionK[0]: " << Info.DistortionK[0] << endl;
                cout << " DistortionK[1]: " << Info.DistortionK[1] << endl;
                cout << " DistortionK[2]: " << Info.DistortionK[2] << endl;
                cout << "--------------------------" << endl;
        }

        Quatf quaternion = pFusionResult->GetOrientation();

        cout << endl << " Press ENTER to continue" << endl;
	
        cv::VideoCapture cap(0);
	if(!cap.isOpened())
	{
		return;
	}
	
	cin.get();
        if (!pSensor||!pHMD)
        {
                cout << "Power Cycle the Oculus Rift then restart Program" << endl;
        }
        while(pSensor && pHMD)
        {
		cv::Mat r;
		cap >> r;
		cv::Mat l = r.clone();
		display(r,l);
		if(cv::waitKey(30)>=0) break;

        }
}



int main(void)
{
Init();
Output();
Clear();
}
