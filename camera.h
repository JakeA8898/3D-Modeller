#ifndef CAMERA_H
#define CAMERA_H

class camera{
    public:
        static void moveFoward(float camPos[], float eyes[]);
        static void moveBackwards(float camPos[], float eyes[]);
        static void moveLeft(float camPos[], float eyes[]);
        static void moveRight(float camPos[], float eyes[]);
        static void mouseLook(int x, int y,float camPos[], float eyes[],int lastX, int lastY, int *ylook,float *degOfRot);

};


#endif