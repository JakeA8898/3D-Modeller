#include <stdio.h>
#include <stdlib.h>
#include <Math.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <time.h>
#include <string>
#include <sstream>

#include "camera.h"

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif


//structure for each material used
struct material {
    int type; //an integer value to identift the type
    float amb[4];
    float diff[4];
    float spec[4];
    float shy;
};
 

//structure for each object
struct object {
    float pos[3] = {50,10,50}; //the objects current position 
    float rotA[3] = {0,0,0};//the objects current rotation values
    float scale[3] = {1,1,1};//the objects current scale values
    material mat;//the objects material
    int type = 0;//the type of object (cube, sphere, etc)
    int min[3];//min point of bounding box
    int max[3];//max point of bounding box
    int position;//position in the array    
};

//Globals
//last known x coord
int lastX = -1;
//last known y coord
int lastY = -1;

//the change in y. used to limit up/down for camera
int ylook = 0;

//degree of rotation for camera
float degOfRot = 0;


//points for the ray
int point1[3];
int point2[3];

//allows you to select what light to move
bool light1 = true;

//allows you to move the camera
bool moveMode = false;

//which object is selected
int selected = 0;

//temp onject for creating an object
object cObject;

//temp object for loading an object
object lObject;


//camera positions
float camPos[] = {-15,25,-15};	//where the camera is
float camPosT[3] = {};
float eyes[] = {100,10,100}; //where the camera is pointing/ looking


std::vector<object> objects;


//light posiions
float light_pos[] = {0,50,0, 1};
float light_pos2[] = {100, 50,100, 1};

//light 1 values
float amb[4] = {0.5, 0.5, 0.5, 1};
float dif[4] = {1,1,1,1};
float spc[4] = {0, 0, 1, 1};

//light 2 values
float amb2[4] = {0.5, 0.5, 0.5, 1};
float dif2[4] = {1,1,1,1};
float spc2[4] = {0, 0, 1, 1};

//material values
//white and black mat values
float m_amb[] = {0,0,0,1.0};
float m_diff[] = {0.55,0.55,0.55,1.0};
float m_spec[] = {0.7,0.7,0.7,1};
float shiny = 0.25 * 128;


float m_amb2[] = {0,0,0,1};
float m_diff2[] = {0.1,0.1,0.1,1.0};
float m_spec2[] = {0.45,0.55,0.45,1};
float shiny2 = 0.25 * 128;

//global rotation values
int angz = 0;
int angy = 0;


//materials for the objects
material emerald = {1,{0.0215,0.1745,0.0215,1.0},{1,1,1,1.0},{0.633,0.727811,0.633,1},(0.6 * 128)};
material ruby = {2,{0.1745,0.01175,0.01175,1},{0.61425,0.04136,0.04136,1},{0.727811,0.626959,0.626959,1},(0.6*128)};
material brass = {3,{0.329412,0.223529,0.027451,1},{0.780392,0.568627,0.113725,1},{0.992157,0.941176,0.807843,1},(0.21794872 *128)};
material chrome = {4,{0.25,0.25,0.25,1},{0.4,0.4,0.4,1},{0.774597,0.774597,0.774597,1},(0.6*128)};
material obsidian = {5,{0.05375,0.05,0.06625,1},{0.18275,0.17,0.22525,1},{0.332741,0.328634,0.346435,1},(0.3*128)};

//current material for object creation
material cMat = emerald;


/////////////////////////////////////////////////////////////////////////////rays

/*
This function takes a bool that detrmines if it has to delete a selected object or not
rayVector is the vector between near and far planes
vLength is the length of that vector
normalVector is rayVector normalized
tempPoint is point1 because point 1 has to be referenced multiple times
*/

void collisionTest(bool delObj){
    float rayVector[3] = {(float) point2[0] - (float) point1[0], (float) point2[1]- (float) point1[1], (float) point2[2] - (float) point1[2]};
    float vLength = sqrt( pow(rayVector[0],2) + ( pow(rayVector[1],2)) + (pow(rayVector[2],2)));
    float normalVector[3] = {rayVector[0]/vLength, rayVector[1]/vLength, rayVector[2]/vLength};
    float tempPoint[3] = {(float) point1[0],(float) point1[1],(float) point1[2]};
    int tempPointI;
    std::vector <std::vector<int>> hits;
    std::vector<int> hit;
    int cycles = 0;
    //check if th ray passes through the planes
    bool xCheck = false;
    bool yCheck = false;
    bool zCheck = false;
    //runs for each object in the scene
    for(int i = 0; i < objects.size(); i++){
        //resets the temp points
        tempPoint[0] = (float) point1[0];
        tempPoint[1] = (float) point1[1];
        tempPoint[2] = (float) point1[2];
        tempPointI = 0;
        cycles = 0;
        //while the ray is not at the far plane
        while((tempPoint[0] != point2[0]) && (tempPoint[1] !=point2[1]) && (tempPoint[2] != point2[2])){
            xCheck = ((tempPoint[0]> ((objects[i].scale[0]*(objects[i].min[0])+objects[i].pos[0])) && ((tempPoint[0]< (objects[i].scale[0]*objects[i].max[0])+objects[i].pos[0]))));
            yCheck = ((tempPoint[1]> ((objects[i].scale[1]*(objects[i].min[1])+objects[i].pos[1])) && ((tempPoint[1]< (objects[i].scale[1]*objects[i].max[1])+objects[i].pos[1]))));
            zCheck = ((tempPoint[2]> ((objects[i].scale[2]*(objects[i].min[2])+objects[i].pos[2])) && ((tempPoint[2]< (objects[i].scale[2]*objects[i].max[2])+objects[i].pos[2]))));
            //if the ray colides, add a colission
            if (xCheck && yCheck && zCheck){
                hit.push_back(objects[i].position);
                hit.push_back(tempPointI);
                hits.push_back(hit);
                hit.clear();
                tempPoint[0] = point2[0];
                tempPoint[1] = point2[1];
                tempPoint[2] = point2[2];
                
            }else{
                //move the vector by its normal vector and try again
                tempPoint[0]+= normalVector[0];
                tempPoint[1]+= normalVector[1];
                tempPoint[2]+= normalVector[2];
                tempPointI++;
                if (cycles > 200){
                    tempPoint[0] = point2[0];
                    tempPoint[1] = point2[1];
                    tempPoint[2] = point2[2];
                }

            }
            cycles++;
        }
    }
    //if there is at least 1 hit, check which one is the closest and set it as selected
    if (hits.size() > 0){
        int currentPos = hits[0][0];
        int closest = hits[0][1];
        for(int i = 1; i < hits.size(); i++){
            if(hits[i][1] < closest){
                int currentPos = hits[i][0];
                int closest = hits[i][1];

            }
        }
        selected = currentPos;

        //if the object is to be deleted, rermove the poect and shift the position value if the rest
        if(delObj){
            objects.erase(objects.begin() + currentPos);
            for(int i = currentPos; i < objects.size(); i++){
                objects[i].position--;
            }
        }

    }

    hits.clear();
}

//creates a ray based on where the user clicked
void raySelect(double winx, double winy,bool delObj){
    double objX;
    double objY;
    double objZ;
    double model[16];
    double proj[16];
    int view[4];

    glGetDoublev(GL_MODELVIEW_MATRIX,model);
    glGetDoublev(GL_PROJECTION_MATRIX,proj);
    glGetIntegerv(GL_VIEWPORT,view);
    double winY = view[3]-winy;
    gluUnProject(winx,winY, 0.0, model, proj, view, &objX, &objY, &objZ);
    point1[0] = objX;
    point1[1] = objY;
    point1[2] = objZ;

    gluUnProject(winx,winY, 1.0, model, proj, view, &objX, &objY, &objZ);
    point2[0] = objX;
    point2[1] = objY;
    point2[2] = objZ;
    collisionTest(delObj);

}


///////////////////////////////////////////////////////////////////////////////



//saves the current scene to a .grph file -- just a plain text file with a special extention
void save(){
    std::string saveName = "";
    std::cout << "Please input a file name, without any extentions\n";
    std::getline(std::cin,saveName);
    std::ofstream file;
    file.open(saveName + ".grph");
    for (int i = 0; i < objects.size(); i++){
        file << objects[i].pos[0] <<",";
        file << objects[i].pos[1] <<",";
        file << objects[i].pos[2] <<",";
        file << objects[i].rotA[0] <<",";
        file << objects[i].rotA[1] <<",";
        file << objects[i].rotA[2] <<",";
        file << objects[i].scale[0] <<",";
        file << objects[i].scale[1] <<",";
        file << objects[i].scale[2] <<",";
        file << objects[i].mat.type <<",";
        file << objects[i].type << ",";
        file << objects[i].min[0] << ",";
        file << objects[i].min[1] << ",";
        file << objects[i].min[2] << ",";
        file << objects[i].max[0] << ",";
        file << objects[i].max[1] << ",";
        file << objects[i].max[2] << ",";
        file << objects[i].position<<",\n";


    }
    file.close();
    std::cout << "Save complete";
}

//loads a file from a .grph file
void load(){
    std::vector<int> tempObj;
    int tempI = 0;
    std::string fileName = "";
    std::string line;
    std::cout << "Please enter a file name with no extention\n";
    std::cout<<"Please make sure the file is a .grph file and is in the current working directory\n";
    std::getline(std::cin,fileName);
    std::ifstream check(fileName + ".grph");
    int pos = 0;
    std::string cItem;
    std::string split = ",";
    if(check){
        objects.clear();
        std::ifstream inputFile (fileName + ".grph");
        while(std::getline(inputFile, line)){
            while((pos = line.find(split)) != std::string::npos){
                cItem = line.substr(0,pos);
                std::stringstream tempInt(cItem);
                tempInt >> tempI;
                tempObj.push_back(tempI);
                tempI = 0;
                line.erase(0, pos+split.length());
            }
            lObject.pos[0] = tempObj[0];
            lObject.pos[1] = tempObj[1];
            lObject.pos[2] = tempObj[2];
            lObject.rotA[0] = tempObj[3];
            lObject.rotA[1] = tempObj[4];
            lObject.rotA[2] = tempObj[5];
            lObject.scale[0] = tempObj[6];
            lObject.scale[1] = tempObj[7];
            lObject.scale[2] = tempObj[8];
            lObject.type = tempObj[10];
            lObject.min[0] = tempObj[11];
            lObject.min[1] = tempObj[12];
            lObject.min[2] = tempObj[13];
            lObject.max[0] = tempObj[14];
            lObject.max[1] = tempObj[15];
            lObject.max[2] = tempObj[16];
            lObject.position = tempObj[17];


            if(tempObj[9] == 1){
                lObject.mat = emerald;

            }else if(tempObj[9] == 2){
                lObject.mat = ruby;

            }else if (tempObj[9] == 3){
                lObject.mat = brass;

            }else if(tempObj[9] == 4){
                lObject.mat = chrome;

            }else if(tempObj[9] == 5){
                lObject.mat = obsidian;
                
            }


            objects.push_back(lObject);
            tempObj.clear();

        }
        inputFile.close();
        std::cout << "Load Competed\n";
        
    }else{
        std::cout << "The file you entered either does not exist or is not a .grph file\n";
        
    }

}





//sets the bounds based on the object 
void bounds(){
    if (cObject.type == 1){
        cObject.min[0] = -0.5*2;
        cObject.min[1] = -0.5*2;
        cObject.min[2] = -0.5*2;

        cObject.max[0] = 0.5*2;
        cObject.max[1] = 0.5*2;
        cObject.max[2] = 0.5*2;

    }else if (cObject.type == 2){
        cObject.min[0] = -0.5*2;
        cObject.min[1] = -0.5*2;
        cObject.min[2] = -0.5*2;

        cObject.max[0] = 0.5*2;
        cObject.max[1] = 0.5*2;
        cObject.max[2] = 0.5*2;

    }else if(cObject.type == 3){
        cObject.min[0] = -1;
        cObject.min[1] = -1;
        cObject.min[2] = -1;

        cObject.max[0] = 1;
        cObject.max[1] = 1;
        cObject.max[2] = 1;

    }else if(cObject.type == 4){
        cObject.min[0] = 0;
        cObject.min[1] = 0;
        cObject.min[2] = 0;

        cObject.max[0] = 1;
        cObject.max[1] = 1;
        cObject.max[2] = 1;

    }else if(cObject.type == 5){
        cObject.min[0] = -0.5*2;
        cObject.min[1] = -0.5*2;
        cObject.min[2] = -0.5*2;

        cObject.max[0] = 0.5*2;
        cObject.max[1] = 0.5*2;
        cObject.max[2] = 0.5*2;

    }


}





//init for window
void init(void)
{
	glClearColor(0,0,0,0);
	glColor3f(1, 1, 1);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 500);

	
}



//display for window
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    
	
	//sets camera pos and scene rotation
    glRotatef(angz,0,0,1);
    glRotatef(angy,0,1,0);
	gluLookAt(camPos[0], camPos[1], camPos[2], eyes[0], eyes[1], eyes[2], 0,1,0);

    


	glColor3f(1,1,1);
	//light 1 settings
    glPushMatrix();
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
        glLightfv(GL_LIGHT0, GL_SPECULAR, spc);
        glTranslatef(light_pos[0],light_pos[1],light_pos[2]);
        glutSolidSphere(1,10,10);
    glPopMatrix();
	//light 2 settings
    glPushMatrix();
        glLightfv(GL_LIGHT1, GL_POSITION, light_pos2);
        glLightfv(GL_LIGHT1, GL_AMBIENT, amb2);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, dif2);
        glLightfv(GL_LIGHT1, GL_SPECULAR, spc2);
        glTranslatef(light_pos2[0],light_pos2[1],light_pos2[2]);
        glutSolidSphere(1,10,10);
    glPopMatrix();

    

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,m_amb);
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,m_diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);

    glPushMatrix();
    glBegin(GL_QUAD_STRIP);
    glNormal3f(0,1,0);
    //the floor
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 100; j++){
            glVertex3f(i,0,j);
            glVertex3f(i+1,0,j);
            glVertex3f(i,0,j+1);
            glVertex3f(i+1,0,j+1);

        }
    }
    glEnd();
    glPopMatrix();

    

    for(int i = 0; i < objects.size(); i++){

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,objects[i].mat.amb);
        glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,objects[i].mat.diff);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, objects[i].mat.spec);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, objects[i].mat.shy);
        //draws each object based on what type they are, and applies their transformations to them
        if(objects[i].type == 1){
            glPushMatrix();
                glTranslatef(objects[i].pos[0],objects[i].pos[1],objects[i].pos[2]);
                glScalef(objects[i].scale[0],objects[i].scale[1],objects[i].scale[2]);
                glRotatef(objects[i].rotA[0],1,0,0);
                glRotatef(objects[i].rotA[1],0,1,0);
                glRotatef(objects[i].rotA[2],0,0,1);
                //moveBounds(i);//here is where I try to set the new points, after the transformations have happened.
                glutSolidCube(1);
            glPopMatrix();

        }else if (objects[i].type == 2){
            glPushMatrix();
                glTranslatef(objects[i].pos[0],objects[i].pos[1],objects[i].pos[2]);
                glScalef(objects[i].scale[0],objects[i].scale[1],objects[i].scale[2]);
                glRotatef(objects[i].rotA[0],1,0,0);
                glRotatef(objects[i].rotA[1],0,1,0);
                glRotatef(objects[i].rotA[2],0,0,1);
                //moveBounds(i);
                glutSolidSphere(1,50,10);
            glPopMatrix();

        }else if (objects[i].type == 3){
            glPushMatrix();
                glTranslatef(objects[i].pos[0],objects[i].pos[1],objects[i].pos[2]);
                glScalef(objects[i].scale[0],objects[i].scale[1],objects[i].scale[2]);
                glRotatef(objects[i].rotA[0],1,0,0);
                glRotatef(objects[i].rotA[1],0,1,0);
                glRotatef(objects[i].rotA[2],0,0,1);
                //moveBounds(i);
                glutSolidTetrahedron();
            glPopMatrix();

        }else if (objects[i].type == 4){
            glPushMatrix();
                glTranslatef(objects[i].pos[0],objects[i].pos[1],objects[i].pos[2]);
                glScalef(objects[i].scale[0],objects[i].scale[1],objects[i].scale[2]);
                glRotatef(objects[i].rotA[0],1,0,0);
                glRotatef(objects[i].rotA[1],0,1,0);
                glRotatef(objects[i].rotA[2],0,0,1);
                //moveBounds(i);
                glutSolidIcosahedron();
            glPopMatrix();

        }else if (objects[i].type == 5){
            glPushMatrix();
                glTranslatef(objects[i].pos[0],objects[i].pos[1],objects[i].pos[2]);
                glScalef(objects[i].scale[0],objects[i].scale[1],objects[i].scale[2]);
                glRotatef(objects[i].rotA[0],1,0,0);
                glRotatef(objects[i].rotA[1],0,1,0);
                glRotatef(objects[i].rotA[2],0,0,1);
                //moveBounds(i);
                glutSolidOctahedron();
            glPopMatrix();

        }

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,m_amb2);
        glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,m_diff2);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_spec2);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny2);

        //draws the bounded object around the selected object
        glPushMatrix();
            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
            glTranslatef(objects[selected].pos[0],objects[selected].pos[1],objects[selected].pos[2]);
            glScalef(objects[selected].scale[0],objects[selected].scale[1],objects[selected].scale[2]);
            glRotatef(objects[selected].rotA[0],1,0,0);
            glRotatef(objects[selected].rotA[1],0,1,0);
            glRotatef(objects[selected].rotA[2],0,0,1);

            glBegin(GL_TRIANGLE_STRIP);
            
            glVertex3f(objects[selected].min[0],objects[selected].min[1],objects[selected].min[2]);//p1
            glVertex3f(objects[selected].min[0],objects[selected].min[1],objects[selected].max[2]);//p2
            glVertex3f(objects[selected].min[0],objects[selected].max[1],objects[selected].min[2]);//p3
            glVertex3f(objects[selected].min[0],objects[selected].max[1],objects[selected].max[2]);//p4
            glVertex3f(objects[selected].max[0],objects[selected].max[1],objects[selected].min[2]);//p5
            glVertex3f(objects[selected].max[0],objects[selected].max[1],objects[selected].max[2]);//p6
            glVertex3f(objects[selected].max[0],objects[selected].min[1],objects[selected].min[2]);//p7
            glVertex3f(objects[selected].max[0],objects[selected].min[1],objects[selected].max[2]);//p8
            glVertex3f(objects[selected].min[0],objects[selected].min[1],objects[selected].min[2]);//p1
            glVertex3f(objects[selected].min[0],objects[selected].min[1],objects[selected].max[2]);//p2
            
            glEnd();


            glBegin(GL_TRIANGLE_STRIP);
            
            glVertex3f(objects[selected].max[0],objects[selected].min[1],objects[selected].min[2]);//p7
            glVertex3f(objects[selected].min[0],objects[selected].min[1],objects[selected].min[2]);//p1
            glVertex3f(objects[selected].max[0],objects[selected].max[1],objects[selected].min[2]);//p5
            glVertex3f(objects[selected].min[0],objects[selected].max[1],objects[selected].min[2]);//p3
            
            glEnd();


            glBegin(GL_TRIANGLE_STRIP);
            
            
            glVertex3f(objects[selected].max[0],objects[selected].max[1],objects[selected].max[2]);//p6
            glVertex3f(objects[selected].min[0],objects[selected].max[1],objects[selected].max[2]);//p4
            glVertex3f(objects[selected].max[0],objects[selected].min[1],objects[selected].max[2]);//p8
            glVertex3f(objects[selected].min[0],objects[selected].min[1],objects[selected].max[2]);//p2
            
            glEnd();

                    
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

        glPopMatrix();

        


    }

	//switch buffers for a smooth animation
	glutSwapBuffers();
}


//keyboard function for window 1
//instructions for what these do are in the main function/printed when the program runs
void keyboard(unsigned char key, int xIn, int yIn)
{
    int mod = glutGetModifiers();
    switch(key){
        
        case 27:
            exit(0);
            break;

        case '6':
            
            cObject.type = 1;
            cObject.position = objects.size();
            selected = cObject.position;
            cObject.mat = cMat;
            bounds();
            objects.push_back(cObject);
            break;

        case '7':
            cObject.type = 2;
            cObject.position = objects.size();
            selected = cObject.position;
            cObject.mat = cMat;
            bounds();
            objects.push_back(cObject);
            break;

        case '8':
            cObject.type = 3;
            cObject.position = objects.size();
            selected = cObject.position;
            cObject.mat = cMat;
            bounds();
            objects.push_back(cObject);
            break;

        case'9':
            cObject.type = 4;
            cObject.position = objects.size();
            selected = cObject.position;
            cObject.mat = cMat;
            objects.push_back(cObject);
            bounds();
            break;

        case '0':
            cObject.type = 5;
            cObject.position = objects.size();
            selected = cObject.position;
            bounds();
            objects.push_back(cObject);
            break;
        case 'z':
            if(mod == GLUT_ACTIVE_ALT){
                objects[selected].scale[2]--;
                

            }else{
                objects[selected].scale[2]++;
            }
            break;

        case 'x':
            if(mod == GLUT_ACTIVE_ALT){
                objects[selected].scale[0]--;

            }else{
                objects[selected].scale[0]++;
            }
            break;

        case 'c':
            if(mod == GLUT_ACTIVE_ALT){
                objects[selected].scale[1]--;

            }else{
                objects[selected].scale[1]++;
            }
            break;

        case 'u':
            if(mod == GLUT_ACTIVE_ALT){
                objects[selected].scale[2]--;
                objects[selected].scale[1]--;
                objects[selected].scale[0]--;
            }else{
                objects[selected].scale[2]++;
                objects[selected].scale[1]++;
                objects[selected].scale[0]++;
            }
            break;
        
        case 'v':
            if(mod == GLUT_ACTIVE_ALT){
                objects[selected].rotA[2]--;
            }else{
                objects[selected].rotA[2]++;
            }
            break;
        case 'b':
            if(mod == GLUT_ACTIVE_ALT){
                objects[selected].rotA[0]--;
            }else{
                objects[selected].rotA[0]++;
            }
            break;
        case'n':
            if(mod == GLUT_ACTIVE_ALT){
                objects[selected].rotA[1]--;
            }else{
                objects[selected].rotA[1]++;
            }
            break;


        case '1':
            cMat = emerald;
            break;
        case '2':
            cMat = ruby;
            break;
        case '3':
            cMat = brass;
            break;
        case '4':
            cMat = chrome;
            break;
        case '5':
            cMat = obsidian;
            break;
        case 'r':
            objects.clear();
            break;
        case 'y':
            light1 = (!light1);
            break;
        case 'i':
            if(mod == GLUT_ACTIVE_ALT){
                if(light1){
                    light_pos[0]--;
                }else{
                    light_pos2[0]--;
                }
            
            }else{
                if(light1){
                    light_pos[0]++;
                }else{
                    light_pos2[0]++;

                }
            }
            break;
            
        case 'o':
            if(mod == GLUT_ACTIVE_ALT){
                if(light1){
                    light_pos[1]--;
                }else{
                    light_pos2[1]--;
                }
            
            }else{
                if(light1){
                    light_pos[1]++;
                }else{
                    light_pos2[1]++;

                }
            }
            break;
        case 'p':
            if(mod == GLUT_ACTIVE_ALT){
                if(light1){
                    light_pos[2]--;
                }else{
                    light_pos2[2]--;
                }
            
            }else{
                if(light1){
                    light_pos[2]++;
                }else{
                    light_pos2[2]++;

                }
            }
            break;
        case ',':
            if(mod == GLUT_ACTIVE_ALT){
                angy--;
            }else{
                angy++;
            }
            break;
        case '.':
            if(mod==GLUT_ACTIVE_ALT){
                angz--;
            }else{
                angz++;
            }
            break;

        case 'w':
            if(moveMode){
                camera::moveFoward(camPos, eyes);
            }
            break;
        case 'a':
            if(moveMode){
                camera::moveLeft(camPos,eyes);
            }
            
            break;
        case's':
            
            if(mod == GLUT_ACTIVE_ALT){
                save();

            }else{
                if(moveMode){
                    camera::moveBackwards(camPos,eyes);
                }
                

            }
            break;
        case 'd':
            if(moveMode){
                camera::moveRight(camPos,eyes);
            }
            
            break;
        case 'l':
            load();
            break;
        case 'q':
            moveMode = !moveMode;
            if(moveMode){
                glutSetCursor(GLUT_CURSOR_NONE);
            }else{
                glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
            }
            break;
        case 32:
            if(moveMode){
                if(mod == (GLUT_ACTIVE_ALT)){
                    camPos[1]--;
                    eyes[1]--;
                }else{
                    camPos[1] ++;
                    eyes[1]++;
                }
                
            }
            break;      

    }

}

void passive(int x, int y){
    if(moveMode){
        if(lastX < 0){
            lastX = 400;
            lastY = 300;
        }else{
            camera::mouseLook(x,y,camPos,eyes,lastX,lastY,&ylook,&degOfRot);
		
        }
        glutWarpPointer(400,300);
    }
	

}

//arrow keys
void special(int key, int x, int y){
    int mod = glutGetModifiers();
    switch(key){
        if(objects.size() > 0){
            case GLUT_KEY_UP:
                if (mod == GLUT_ACTIVE_CTRL){
                    objects[selected].pos[1]++;
                } else {
                    objects[selected].pos[0]++;
                }
                break;
            
            case GLUT_KEY_DOWN:
                if (mod == GLUT_ACTIVE_CTRL){
                    objects[selected].pos[1]--;
                } else {
                    objects[selected].pos[0]--;
                }
                break;
            
            case GLUT_KEY_LEFT:
                objects[selected].pos[2]--;
                break;

            case GLUT_KEY_RIGHT:
                objects[selected].pos[2]++;
                break;
        }

    }
        
}

void mouse(int btn, int state, int x, int y){
    if (btn == GLUT_LEFT_BUTTON){
        if(state == GLUT_DOWN){
            raySelect(x,y,false);
        }
        
    }
    if (btn == GLUT_RIGHT_BUTTON){
        if(state == GLUT_DOWN){
            raySelect(x,y,true);
        }
    }
}




void reshape(int w, int h)
{

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluOrtho2D(0, w, 0, h);
	gluPerspective(45, (float)((w+0.0f)/h), 1, 500);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

void FPS(int val){
	glutPostRedisplay();
	glutTimerFunc(17, FPS, 0); // 1sec = 1000, 60fps = 1000/60 = ~17
}

void callBackInit(){
	glutDisplayFunc(display);	//registers "display" as the display callback function
	glutKeyboardFunc(keyboard);
    glutPassiveMotionFunc(passive);
	glutSpecialFunc(special);
	glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
	glutTimerFunc(0, FPS, 0);
}



/* main function - program entry point */
int main(int argc, char** argv)
{
    printf("CONTROLS:\n");    //instructions/controls
    printf("Press 'ESC' to exit program\n");
    printf("Press '1' to set current material to emerald\n");
    printf("Press '2' to set current material to ruby\n");
    printf("Press '3' to set current material to brass\n");
    printf("Press '4' to set current material to chrome\n");
    printf("Press '5' to set current material to obsidian\n");
    printf("Press '6' to set current object to cube\n");
    printf("Press '7' to set current object to sphere\n");
    printf("Press '8' to set current object to tetrahedron\n");
    printf("Press '9' to set current object to icosahedron\n");
    printf("Press '0' to set current object to octahedron\n");
    printf("Press 'z' / 'ALT+z' to scale current object in the z-axis up/down\n");
    printf("Press 'x' / 'ALT+x' to scale current object in the x-axis up/down\n");
    printf("Press 'c' / 'ALT+c' to scale current object in the y-axis up/down\n");
    printf("Press 'u' / 'ALT+u' to scale current object in all axis up/down\n");
    printf("Press 'v' / 'ALT+v' to rotate current object about the z-axis\n");
    printf("Press 'b' / 'ALT+b' to rotate current object about the x-axis\n");
    printf("Press 'n' / 'ALT+n' to rotate current object about the y-axis\n");
    printf("Press 'r' to clear the scene of the objects\n");
    printf("Press 'y' to switch control between the two lights\n");
    printf("Press 'i' / 'ALT+i' to move currently selected light up/down the x-axis\n");
    printf("Press 'o' / 'ALT+o' to move currently selected light up/down the y-axis\n");
    printf("Press 'p' / 'ALT+p' to move currently selected light up/down the z-axis\n");
    printf("Press ',' / 'ALT+,' to rotate the scene about the y-axis\n");
    printf("Press '.' / 'ALT+.' to rotate the scene about the z-axis\n");
    printf("Press 'ALT+s' to save the current scene to a .grph file\n");
    printf("Press 'l' to load the current scene from a .grph file\n");
    printf("Press 'q' to enter move mode which allows you to move the camera\n");
    printf("Press 'space_bar' / 'ALT+space_bar' to move camera up/down the y-axis while in move mode\n");
    printf("Press 'space_bar' / 'ALT+space_bar' to move camera up/down the y-axis while in move mode\n");
    printf("Press 'w' to move in the direction your facing while in move mode\n");
    printf("Press 's' to move in the opposite direction your facing while in move mode\n");
    printf("Press 'a' to move left relative to the direction your facing while in move mode\n");
    printf("Press 'd' to move right relative to the direction your facing while in move mode\n");
    printf("Move 'mouse' / 'trackpad' to move camera relative to mouse cursor position on screen while in move mode\n");
    printf("Press 'left_mouse_button' on an object to select that object in the scene\n");
    printf("Press 'right_mouse_button' on an object to delete that object from the scene\n");
    printf("Press 'up_arrow' to move selected object up the x-axis\n");
    printf("Press 'down_arrow' to move selected object down the x-axis\n");
    printf("Press 'left_arrow' to move selected object down the z-axis\n");
    printf("Press 'right_arrow' to move selected object up the z-axis\n");
    printf("Press 'CTRL+up_arrow' to move selected object up the y-axis\n");
    printf("Press 'CTRL+down_arrow' to move selected object down the y-axis\n");
	glutInit(&argc, argv);		//starts up GLUT
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(800,600);
	glutInitWindowPosition(50, 50);

	glutCreateWindow("Modeller");

	callBackInit();	

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	
	
    glShadeModel(GL_SMOOTH);


	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	init();


	glutMainLoop();				//starts the event glutMainLoop
	return(0);					//return may not be necessary on all compilers
}