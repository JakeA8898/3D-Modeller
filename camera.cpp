#include <Math.h>
#include "camera.h"
#include <iostream>
/*
In all of these functions, speed is a float value that controls the speed the camera moves
eyeVector is the vector between the camPos and eyes vector
vLength is the length of eyeVector
normalEyeV is eyeVector normalized

*/

//moves the camera in the direction it is lokking
void camera::moveFoward(float camPos[], float eyes[]){
	float speed = 0.05;
	float eyeVector[3] = {eyes[0]-camPos[0],0,eyes[2]-camPos[2]};
	float vLength = sqrt((eyeVector[0],2) + (eyeVector[1],2) + (eyeVector[2],2));
	float normalEyeV[3] = {eyeVector[0]/vLength,eyeVector[1]/vLength, eyeVector[2]/vLength};
	camPos[0] += normalEyeV[0]*speed;
	camPos[1] += normalEyeV[1]*speed;
	camPos[2] += normalEyeV[2]*speed;

	eyes[0] += normalEyeV[0]*speed;
	eyes[1] += normalEyeV[1]*speed;
	eyes[2] += normalEyeV[2]*speed;

}
//moves the camera backwards, based on where it is looking
void camera::moveBackwards(float camPos[], float eyes[]){
	float speed = 0.05;
	float eyeVector[3] = {eyes[0]-camPos[0],0,eyes[2]-camPos[2]};
	float vLength = sqrt((eyeVector[0],2) + (eyeVector[1],2) + (eyeVector[2],2));
	float normalEyeV[3] = {eyeVector[0]/vLength,eyeVector[1]/vLength, eyeVector[2]/vLength};
	camPos[0] -= normalEyeV[0]*speed;
	camPos[1] -= normalEyeV[1]*speed;
	camPos[2] -= normalEyeV[2]*speed;

	eyes[0] -= normalEyeV[0]*speed;
	eyes[1] -= normalEyeV[1]*speed;
	eyes[2] -= normalEyeV[2]*speed;

}

//moves the camera to the left of where it is looking
void camera::moveLeft(float camPos[], float eyes[]){
	float speed = 0.05;
	float eyeVector[3] = {eyes[0]-camPos[0],0,eyes[2]-camPos[2]};
	float vLength = sqrt((eyeVector[0],2) + (eyeVector[1],2) + (eyeVector[2],2));
	float normalEyeV[3] = {eyeVector[0]/vLength,eyeVector[1]/vLength, eyeVector[2]/vLength};
	float newPVector[3] = {-(normalEyeV[2]), 0, normalEyeV[0]};
	camPos[0] -= newPVector[0]*speed;
	camPos[1] -= newPVector[1]*speed;
	camPos[2] -= newPVector[2]*speed;

	eyes[0] -= newPVector[0]*speed;
	eyes[1] -= newPVector[1]*speed;
	eyes[2] -= newPVector[2]*speed;

}

//moves the camera to the right of where is is looking
void camera::moveRight(float camPos[], float eyes[]){
	float speed = 0.05;
	float eyeVector[3] = {eyes[0]-camPos[0],0,eyes[2]-camPos[2]};
	float vLength = sqrt((eyeVector[0],2) + (eyeVector[1],2) + (eyeVector[2],2));
	float normalEyeV[3] = {eyeVector[0]/vLength,eyeVector[1]/vLength, eyeVector[2]/vLength};
	float newPVector[3] = {-(normalEyeV[2]), 0, normalEyeV[0]};
	camPos[0] += newPVector[0]*speed;
	camPos[1] += newPVector[1]*speed;
	camPos[2] += newPVector[2]*speed;

	eyes[0] += newPVector[0]*speed;
	eyes[1] += newPVector[1]*speed;
	eyes[2] += newPVector[2]*speed;

}

//rotates the camera based on mouse movement
void camera::mouseLook(int x, int y,float camPos[], float eyes[],int lastX, int lastY, int *ylook,float *degOfRot){
	float eyeVector[3] = {eyes[0]-camPos[0],0,eyes[2]-camPos[2]};
	float vLength = sqrt((eyeVector[0],2) + (eyeVector[1],2) + (eyeVector[2],2));
	//difference between last x/y coord and the new ones
	int differenceX = lastX - x;
	int differenceY = lastY - y;

		//bounds for the y look so it doesnt flip
		if(*ylook < 180 && *ylook > -90){
			eyes[1] += differenceY*2;
			*ylook +=differenceY;
		}else if(*ylook >= 180){
			eyes[1]--;
			*ylook = *ylook - 1;
		}else if(*ylook <= -90){
			eyes[1]++;
			*ylook = *ylook + 1;
		}
	//the degree of rotation around the y axis
	*degOfRot += differenceX;
	//converting deg to radins
	float deg = *degOfRot * (3.14159/180);
	//speed to affect how fast the rotation happend
	float speed = 0.25;

	eyes[0] = (camPos[0] + (500* sin(deg))*speed);
	eyes[2] = (camPos[2] + (500* cos(deg))*speed);





}