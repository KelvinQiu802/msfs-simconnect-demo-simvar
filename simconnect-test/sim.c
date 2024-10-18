#include <stdio.h>
#include <windows.h>
#include <SimConnect.h>

struct Data {
	double latitude;
	double longitude;
	double altitude;
};

HANDLE hSimConnect = NULL;

void CALLBACK MyDispatchProc(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext) {
	SIMCONNECT_RECV_SIMOBJECT_DATA* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;
	switch (pData->dwID) {
	case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
		if (pObjData->dwRequestID == 1) {
			struct Data* pS = (struct Data*)&pObjData->dwData;
			printf("Latitude: %.4f, Longitude: %.4f, Altitude: %.4f\n", pS->latitude, pS->longitude, pS->altitude);
		}
		else {
			printf("Unknown request ID: %d\n", pObjData->dwRequestID);
		}
		break;
	default:
		printf("Unknown packet ID: %d\n", pData->dwID);
		break;
	}
}

int main() {
	HRESULT hr;

	if (SUCCEEDED(SimConnect_Open(&hSimConnect, "SimConnect Test", NULL, 0, 0, 0))) {
		printf("Connected to Flight Simulator!\n");

		hr = SimConnect_AddToDataDefinition(hSimConnect, 0, "Plane Latitude", "degrees");
		hr = SimConnect_AddToDataDefinition(hSimConnect, 0, "Plane Longitude", "degrees");
		hr = SimConnect_AddToDataDefinition(hSimConnect, 0, "Plane Altitude", "feet");

		hr = SimConnect_RequestDataOnSimObject(hSimConnect, 1, 0, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND);

		while (1) {
			SimConnect_CallDispatch(hSimConnect, MyDispatchProc, NULL);
			Sleep(1000);
		}

		hr = SimConnect_Close(hSimConnect);
	}
	else {
		printf("Failed to connect to Flight Simulator!\n");
	}

	return 0;
}
