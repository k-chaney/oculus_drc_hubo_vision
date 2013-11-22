#include "OVR.h"


using namespace OVR;

Ptr<DeviceManager>	pManager;
Ptr<HMDDevice>		pHMD;
Ptr<SensorDevice>	pSensor;
SensorFusion*		pFusionResult;
HMDInfo			Info;
bool			InfoLoaded;


void Init()
{
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
}


void Clear()
{
	pSensor.Clear();
        pHMD.Clear();
	pManager.Clear();

	delete pFusionResult;

	System::Destroy();
}


int main()
{
	Init();

	Clear();
}
