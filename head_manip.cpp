#include "OVR.h"
#include <iostream>
#include "hubo.h"


// for ach
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include <inttypes.h>
#include "ach.h"



using namespace OVR;
using namespace std;

Ptr<DeviceManager>	pManager;
Ptr<HMDDevice>		pHMD;
Ptr<SensorDevice>	pSensor;
SensorFusion*		pFusionResult;
HMDInfo			Info;
bool			InfoLoaded;

ach_channel_t chan_hubo_ref;
ach_channel_t chan_hubo_state;

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

	int r = ach_open(&chan_hubo_ref, HUBO_CHAN_REF_NECK_NAME, NULL);
	assert( ACH_OK == r );

	r = ach_open(&chan_hubo_state, HUBO_CHAN_STATE_NAME, NULL);
	assert( ACH_OK == r );
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
	cout << "----- Oculus Console -----" << endl;

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
	// Create strctures to read and write

	struct hubo_ref ref;
	struct hubo_state state;
	memset( &ref, 0, sizeof(ref));
	memset( &state, 0, sizeof(state));
	
	size_t fs;
	
	cout << endl << "hubo-ach ready" << endl;

	cout << endl << " Press ENTER to continue" << endl;

	cin.get();
	if (!pSensor)
	{
		cout << "Power Cycle the Oculus Rift then restart Program" << endl;
	}
	while(pSensor)
	{
		Quatf quaternion = pFusionResult->GetOrientation();

		float yaw, pitch, roll;
		quaternion.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&yaw, &pitch, &roll);
		cout << " Yaw: " << yaw << 
			", Pitch: " << pitch << 
			", Roll: " << roll << endl;

		ref.ref[NKY]=yaw;
		ref.ref[NK1]=pitch;
		ach_put( &chan_hubo_ref, &ref, sizeof(ref));
		usleep(500);	


	}
}


int main()
{
	Init();
	Output();
	Clear();
}
