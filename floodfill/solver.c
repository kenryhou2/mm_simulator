#include "solver.h"
#include "API.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define QSIZE 512
#define MAX 512

// Coord* q_floodfill[QSIZE];
// int q_front = -1, q_rear = -1;

int blank_manhattan_arr[16][16] = {
        {14,13,12,11,10,9,8,7,7,8,9,10,11,12,13,14},    //col 0
        {13,12,11,10,9,8,7,6,6,7,8,9,10,11,12,13},      //col 1
        {12,11,10,9,8,7,6,5,5,6,7,8,9,10,11,12},        //col 2
        {11,10,9,8,7,6,5,4,4,5,6,7,8,9,10,11},          //col 3
        {10,9,8,7,6,5,4,3,3,4,5,6,7,8,9,10},            //col 4
        {9,8,7,6,5,4,3,2,2,3,4,5,6,7,8,9},              //col 5
        {8,7,6,5,4,3,2,1,1,2,3,4,5,6,7,8},              //col 6
        {7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7},              //col 7         G_NORTH
        {7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7},              //col 8
        {8,7,6,5,4,3,2,1,1,2,3,4,5,6,7,8},              //col 9
        {9,8,7,6,5,4,3,2,2,3,4,5,6,7,8,9},              //col 10
        {10,9,8,7,6,5,4,3,3,4,5,6,7,8,9,10},            //col 11
        {11,10,9,8,7,6,5,4,4,5,6,7,8,9,10,11},          //col 12
        {12,11,10,9,8,7,6,5,5,6,7,8,9,10,11,12},        //col 13
        {13,12,11,10,9,8,7,6,6,7,8,9,10,11,12,13},      //col 14
        {14,13,12,11,10,9,8,7,7,8,9,10,11,12,13,14}     //col 15
                        //G_EAST
    };//Row 0, Col 0 is bottom left of physical maze. Tilt your head 90 to right

struct Cell
{
    int nWall;
    int sWall;
    int eWall;
    int wWall;
};

struct Coord
{
    int row;
    int col;
};

Coord coordArray[MAX];
int front = 0;
int rear = -1;
int itemCount = 0;

Coord peek() {
   return coordArray[front];
}

int isEmpty() {
	if(itemCount == 0)
	{
		debug_log("queue is empty");
		return 1;
	}
   return 0;
}

int isFull() {
	if(itemCount == MAX)
	{
		debug_log("queue is full");
		return 1;
	}
	return 0;
}

int size() {
   return itemCount;
}  

void insert(Coord data) {
	fprintf(stderr,"inserting col:%d, row:%d\n",data.col,data.row);

   if(!isFull()) {
	
      if(rear == MAX-1) {
         rear = -1;            
      }       

      coordArray[++rear] = data;
      itemCount++;
   }
}

Coord removeData() {
   Coord data = coordArray[front++];
   fprintf(stderr,"removing col:%d, row:%d\n",data.col,data.row);
   if(front == MAX) {
      front = 0;
   }
	
   itemCount--;
   return data;  
}


struct Pose
{
    int col;
    int row;
    char* g_heading_str;
} mouse_pose;

struct Floodfill
{
    //Floodfill variables

    Cell cells[16][16];
    // Mouse manhattan array
    int manhattan_arr[16][16];
    //member functions
    //int (*nextMovement_funct) (int x, int y); //need help connecting this to nextMovement
} floodfill_obj;

int getMan(int col, int row)
{
    return floodfill_obj.manhattan_arr[col][row]; 
}

void setMan(int col, int row, int man)
{
    floodfill_obj.manhattan_arr[col][row] = man;
}

void print_mouse_info(Pose* p_ptr)
{
    char colbuff[10];
    char rowbuff[10];
    char manbuff[10];
    int col_val = p_ptr->col;
    int row_val = p_ptr->row;
    int man_val = getMan(p_ptr->col,p_ptr->row);
    char* col_str = itoa(col_val,colbuff,10);
    char* row_str = itoa(row_val,rowbuff,10);
    char* man_str = itoa(man_val,manbuff,10);
    char* head = p_ptr->g_heading_str;
    char src[100] = "";
    strcpy(src, "Col: ");
    strcat(src, col_str);
    strcat(src, " Row: ");
    strcat(src, row_str);
    strcat(src, " Heading: ");
    strcat(src, head);
    strcat(src, " Man: ");
    strcat(src, man_str);
    debug_log(src);
}

int isAtCenter(int col, int row)
{
    
    if(
        (row == 7 && col == 7) ||
        (row == 7 && col == 8) ||
        (row == 8 && col == 7) ||
        (row == 8 && col == 8)
    )
    {
        return 1;
        debug_log("center reached");
    }
    else
    {
        return 0;
    }
}

int isOB(int col, int row)
{
    if(row < 0 || row > 15 || col < 0 || col > 15)
    {
        debug_log("OB sensed");
        return 1;
    }
    else
    {
        return 0;
    }
}

char* CW(char* dir)
{
    if(dir == "N")
    {
        return "E"; 
    }
    else if(dir == "E")
    {
        return "S";
    }
    else if(dir == "S")
    {
        return "W";
    }
    else if(dir == "W")
    {
        return "N";
    }
    else
    {
        return "L";
    }
}

char* counterCW(char* dir)
{
    if(dir == "N")
    {
        return "W";
    }
    else if(dir == "W")
    {
        return "S";
    }
    else if(dir == "S")
    {
        return "E";
    }
    else if(dir == "E")
    {
        return "N";
    }
    else
    {
        return "L";
    }
}

int isWall(int col, int row, char* dir)
{
    if(dir == "N" && floodfill_obj.cells[col][row].nWall == 1)
    {
       return 1;
    }
    if(dir == "W" && floodfill_obj.cells[col][row].wWall == 1)
    {
        return 1;
    }
    if(dir == "E" && floodfill_obj.cells[col][row].eWall == 1)
    {
        return 1;
    }
    if(dir == "S" && floodfill_obj.cells[col][row].sWall == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }

}

void setWall(int col, int row, char* dir)
{
    if(dir == "N")
    {
        API_setWall(col,row,'n');
        floodfill_obj.cells[col][row].nWall = 1;
        if(!isOB(col,row+1))
        {
            floodfill_obj.cells[col][row+1].sWall = 1;
        }
    }
    if(dir == "W")
    {
        API_setWall(col,row,'w');
        floodfill_obj.cells[col][row].wWall = 1;
        if(!isOB(col-1,row))
        {
            floodfill_obj.cells[col-1][row].eWall = 1;
        }
    }
    if(dir == "E")
    {
        API_setWall(col,row,'e');
        floodfill_obj.cells[col][row].eWall = 1;
        if(!isOB(col+1,row))
        {
            floodfill_obj.cells[col+1][row].wWall = 1;
        }
    }
    if(dir == "S")
    {
        API_setWall(col,row,'s');
        floodfill_obj.cells[col][row].sWall = 1;
        if(!isOB(col,row-1))
        {
            floodfill_obj.cells[col][row-1].nWall = 1;
        }
    }
    if(dir == "L")
    {
        debug_log("Error in setting wall");
    }
}

void update_Man_display()
{
	for(int col = 0; col < 16; col++)
	{
		for(int row = 0; row < 16; row++)
		{
			int man = floodfill_obj.manhattan_arr[col][row];
			char manbuff[10];
    		char* str = itoa(man,manbuff,10);
			API_setText(col,row,str);
		}
	}
}

void print_Q()
{
	debug_log("printing Q:");
	for(int i = front; i < rear+1; i++)
	{
		fprintf(stderr,"index:%d, col:%drow:%d\n", i,coordArray[i].col, coordArray[i].row);
	}
}

void run_floodfill(Pose* mouse_pose)
{
    debug_log("running floodfill");
    /*
    Add current cell to queue
    While queue is not empty:
    Take front cell in queue “out of line” for consideration
    Get the front cell’s minimum value amongst accessible neighbors.
    If the front cell’s value ≤ minimum of its neighbors, set the front cell’s value to minimum + 1 and add all accessible neighbors to the queue.
    Else, continue!
    */
    Coord mouse_coord;
    mouse_coord.col = mouse_pose->col;
    mouse_coord.row = mouse_pose->row;
    //fprintf(stderr, "mouse_coord col:%d, mouse_coord row:%d\n", mouse_coord.col, mouse_coord.row);
    
    insert(mouse_coord);
    while(!isEmpty())
    {
        Coord sample_coord = removeData();
        int temp_sample_col = sample_coord.col; //capture queue removed data immedately...
        int temp_sample_row = sample_coord.row;
        fprintf(stderr, "queue state is empty?: %d\n",isEmpty());
        print_Q();
        fprintf(stderr,"sample cell is col:%d row:%d\n", temp_sample_col, temp_sample_row);
        int current_Man = getMan(temp_sample_col, temp_sample_row);
        int min_Man = 999;
        //print_Q();
        //take into all available adjacent  cells and check for min manhattan
        Coord N_mouse_coord,S_mouse_coord,E_mouse_coord,W_mouse_coord;
        
        N_mouse_coord.col = temp_sample_col;
        N_mouse_coord.row = temp_sample_row+1;

        S_mouse_coord.col = temp_sample_col;
        S_mouse_coord.row = temp_sample_row-1;

        E_mouse_coord.col = temp_sample_col+1;
        E_mouse_coord.row = temp_sample_row;

        W_mouse_coord.col = temp_sample_col-1;
        W_mouse_coord.row = temp_sample_row;

        // print_Q();
        // fprintf(stderr,"sample cell is col:%d row:%d\n", sample_coord->col, sample_coord->row);
        // fprintf(stderr,"sample cell is col:%d row:%d\n", temp_sample_col, temp_sample_row);
        if(!isOB(N_mouse_coord.col,N_mouse_coord.row))
        {
            debug_log("N cell is in bounds");
            if (!isWall(N_mouse_coord.col,N_mouse_coord.row,"S"))
            {
                debug_log("no sWall in N cell");
                int nman = getMan(N_mouse_coord.col,N_mouse_coord.row); 
                if(nman < min_Man)
                {
                    min_Man = nman;
                }
            }
        }

        if(!isOB(W_mouse_coord.col,W_mouse_coord.row))
        {
            debug_log("W cell is in bounds");
            if(!isWall(W_mouse_coord.col,W_mouse_coord.row,"E"))
            {
                debug_log("no eWall in W cell");
                int wman = getMan(W_mouse_coord.col,W_mouse_coord.row); 
                if(wman < min_Man)
                {
                    min_Man = wman;
                }
            }
        }

        if(!isOB(E_mouse_coord.col,E_mouse_coord.row))
        {
            debug_log("E cell is in bounds");
            if(!isWall(E_mouse_coord.col,E_mouse_coord.row,"W"))
            {
                debug_log("no wWall in E cell");
                int eman = getMan(E_mouse_coord.col,E_mouse_coord.row); 
                if(eman < min_Man)
                {
                    min_Man = eman;
                }
            }
        }
        
        if(!isOB(S_mouse_coord.col,S_mouse_coord.row))
        {
            debug_log("S cell is in bounds");
            if(!isWall(S_mouse_coord.col,S_mouse_coord.row,"N"))
            {
                debug_log("no nWall in S cell");
                int sman = getMan(S_mouse_coord.col,S_mouse_coord.row); 
                if(sman < min_Man)
                {
                    min_Man = sman;
                }
            }
        }

               
        //print_Q();
        fprintf(stderr,"sample cell man is:%d min man is: %d\n",current_Man, min_Man); // at this point, current man should be less than min man..

        if(current_Man < min_Man || current_Man == min_Man)
        {
            debug_log("current man is <= minMan");
            int newman = min_Man + 1;
            //setMan(sample_coord->col,sample_coord->row, newman); //adding 1 to current coord
            setMan(temp_sample_col,temp_sample_row, newman);
            //int temp = getMan(sample_coord->col,sample_coord->row);
            int temp = getMan(temp_sample_col,temp_sample_row);
            fprintf(stderr,"current man is:%d\n", temp);

            if(!isOB(N_mouse_coord.col,N_mouse_coord.row))
            {
                //debug_log("N cell is in bounds");
                if (!isWall(N_mouse_coord.col,N_mouse_coord.row,"S"))
                {
                	debug_log("qing Ncell");
                    insert(N_mouse_coord);
                }
            }

            if(!isOB(W_mouse_coord.col,W_mouse_coord.row))
            {
                //debug_log("W cell is in bounds");
                if(!isWall(W_mouse_coord.col,W_mouse_coord.row,"E"))
                {
                	debug_log("qing Wcell");
                    insert(W_mouse_coord);
                }
            }

            if(!isOB(E_mouse_coord.col,E_mouse_coord.row))
            {
                //debug_log("E cell is in bounds");
                if(!isWall(E_mouse_coord.col,E_mouse_coord.row,"W"))
                {
                	debug_log("qing Ecell");
                    insert(E_mouse_coord);
                }
            }
            
            if(!isOB(S_mouse_coord.col,S_mouse_coord.row))
            {
                //debug_log("S cell is in bounds");
                if(!isWall(S_mouse_coord.col,S_mouse_coord.row,"N"))
                {
                	debug_log("qing Scell");
                    insert(S_mouse_coord);
                }
            }
            print_Q();
        }//end min Man check
    }//end while
}//end runFloodfill()

int nextMovement(Pose* mouse_pose) 
//Moving forward 0 face N, 1 face E, 2 face W, 3 face S, Turn left 4, Turn right 5, idle 6
{
	update_Man_display();
    if(isAtCenter(mouse_pose->col,mouse_pose->row))
    {
        debug_log("center reached");
        return 6; // return idle
    }

    //go towards lower Manhattan coord.
    int next_min_Man = getMan(mouse_pose->col,mouse_pose->row);
    char next_Heading = 'L';
    int floodfill_done = 0;
    Coord N_mouse_coord;
    N_mouse_coord.row = mouse_pose->row + 1;
    N_mouse_coord.col = mouse_pose->col;
    
    Coord S_mouse_coord;
    S_mouse_coord.row = mouse_pose->row - 1;
    S_mouse_coord.col = mouse_pose->col;

    Coord E_mouse_coord;
    E_mouse_coord.row = mouse_pose->row;
    E_mouse_coord.col = mouse_pose->col + 1;

    Coord W_mouse_coord;
    W_mouse_coord.row = mouse_pose->row;
    W_mouse_coord.col = mouse_pose->col - 1;
    
    //setting walls
    if(API_wallFront())
    {
        debug_log("wall front");
        setWall(mouse_pose->col,mouse_pose->row,mouse_pose->g_heading_str);
    }

    if(API_wallLeft())
    {
        debug_log("wall left");
        setWall(mouse_pose->col,mouse_pose->row,counterCW(mouse_pose->g_heading_str));
    }

    if(API_wallRight())
    {
        debug_log("wall right");
        setWall(mouse_pose->col,mouse_pose->row,CW(mouse_pose->g_heading_str));
    }

    //Finding next heading via min Man dist

    if(!isOB(N_mouse_coord.col,N_mouse_coord.row))
    {
        debug_log("N cell is in bounds");
        if (!isWall(N_mouse_coord.col,N_mouse_coord.row,"S"))
        {
            debug_log("no sWall in N cell");
            int nman = getMan(N_mouse_coord.col,N_mouse_coord.row); 
            if(nman < next_min_Man)
            {
                next_min_Man = nman;
                next_Heading = 'N';
            }
        }
    }

    if(!isOB(W_mouse_coord.col,W_mouse_coord.row))
    {
        debug_log("W cell is in bounds");
        if(!isWall(W_mouse_coord.col,W_mouse_coord.row,"E"))
        {
            debug_log("no eWall in W cell");
            int wman = getMan(W_mouse_coord.col,W_mouse_coord.row); 
            if(wman < next_min_Man)
            {
                next_min_Man = wman;
                next_Heading = 'W';
            }
        }
    }

    if(!isOB(E_mouse_coord.col,E_mouse_coord.row))
    {
        debug_log("E cell is in bounds");
        if(!isWall(E_mouse_coord.col,E_mouse_coord.row,"W"))
        {
            debug_log("no wWall in E cell");
            int eman = getMan(E_mouse_coord.col,E_mouse_coord.row); 
            if(eman < next_min_Man)
            {
                next_min_Man = eman;
                next_Heading = 'E';
            }
        }
    }
    
    if(!isOB(S_mouse_coord.col,S_mouse_coord.row))
    {
        debug_log("S cell is in bounds");
        if(!isWall(S_mouse_coord.col,S_mouse_coord.row,"N"))
        {
            debug_log("no nWall in S cell");
            int sman = getMan(S_mouse_coord.col,S_mouse_coord.row); 
            if(sman < next_min_Man)
            {
                next_min_Man = sman;
                next_Heading = 'S';
            }
        }
    }

    //if no min man dist, must run floodfill.
    
    if(next_Heading == 'L')
    {
        run_floodfill(mouse_pose);
        update_Man_display();
        fprintf(stderr,"curr man:%d\n",getMan(mouse_pose->col,mouse_pose->row));
        debug_log("finished floodfill");
        return 7;
    }
    
    debug_log("next heading: ");
	debug_log(&next_Heading);
	//at this point we should have a min next heading and man number to traverse to.
	if(next_Heading == 'N')
    {
        if(mouse_pose->g_heading_str == "N")
        {
            return 0;
        }

        if(mouse_pose->g_heading_str == "E")
        {
            mouse_pose->g_heading_str = "N";
            return 4; //turn Left
        }
        
        if(mouse_pose->g_heading_str == "W")
        {
            mouse_pose->g_heading_str = "N";
            return 5; //turn Right
        }
        if(mouse_pose->g_heading_str == "S")
        {
            mouse_pose->g_heading_str = "N";
            return 8; //turn 180 right
        } 

    }

    else if (next_Heading == 'E')
    {
        if(mouse_pose->g_heading_str == "E")
        {
            return 1;
        }

        if(mouse_pose->g_heading_str == "N")
        {
            mouse_pose->g_heading_str = "E";
            return 5; //turn right
        }
        
        if(mouse_pose->g_heading_str == "S")
        {
            mouse_pose->g_heading_str = "E";
            return 4; //turn left
        }
        if(mouse_pose->g_heading_str == "W")
        {
            mouse_pose->g_heading_str = "E";
            return 8; //turn 180 right
        }
    }

    else if (next_Heading == 'W')
    {
        if(mouse_pose->g_heading_str == "W")
        {
            return 2;
        }

        if(mouse_pose->g_heading_str == "N")
        {
            mouse_pose->g_heading_str = "W";
            return 4; //turn left
        }
        
        if(mouse_pose->g_heading_str == "S")
        {
            mouse_pose->g_heading_str = "W";
            return 5; //turn right
        }
        if(mouse_pose->g_heading_str == "E")
        {
            mouse_pose->g_heading_str = "W";
            return 8; //turn 180 right
        } 
    }

    else if (next_Heading == 'S')
    {
        if(mouse_pose->g_heading_str == "S")
        {
            return 3;
        }

        if(mouse_pose->g_heading_str == "E")
        {
            mouse_pose->g_heading_str = "S";
            return 5; //turn right
        }
        
        if(mouse_pose->g_heading_str == "W")
        {
            mouse_pose->g_heading_str = "S";
            return 4; //turn left
        }
        if(mouse_pose->g_heading_str == "N")
        {
            mouse_pose->g_heading_str = "S";
            return 8; //turn 180 right
        } 
    }
    
    

    // else if(next_Heading == 'L')
    // {
    //     debug_log("no next heading computed, taking the L.");
    //     return 6;
    // }
}

void init_mouse(Pose* mp)
{
    mp->col = 0;
    mp->row = 0;
    // mp->coord.row = 0;
    // mp->coord.col = 0;
    mp->g_heading_str = "N";
}

int doNextMove(Pose* mouse_pose) {   
    print_mouse_info(mouse_pose);
    int nextMove = nextMovement(mouse_pose); 
    //nextMove should be a value 0-6

    char nextMove_c = nextMove + '0';
    debug_log(&nextMove_c);
    

    switch(nextMove)
    {
        case 0:
            //move forward facing N
            API_moveForward();
            mouse_pose->row += 1;
            return 1; 
            break;
        case 1: //move forward facing E
            API_moveForward();
            mouse_pose->col += 1;
            return 1;
            break;
        case 2: // move forward facing W
            API_moveForward();
            mouse_pose->col += -1;
            return 1;
            break;
        case 3: //move forward facing S
            API_moveForward();
            mouse_pose->row += -1;
            return 1;
            break;
        case 4: //turn left
            API_turnLeft();
            return 1;
            break;
        case 5:
            API_turnRight();
            return 1;
            break;
        case 6: //finish program
            return 0;
            break;
        case 7:
        	return 1; //cycle thru for floodfill.
        	break;
        case 8:
        	API_turnRight(); //turn 180 to rightside
        	API_turnRight();
        	return 1;
        	break;
    }
}

Action solver() {    
    //Initialize cells to have no walls STEP 1
    for(int c = 0; c < 16; c++)
    {
        for(int r = 0; r < 16; r++)
        {
            floodfill_obj.cells[c][r].nWall = 0;
            floodfill_obj.cells[c][r].eWall = 0;
            floodfill_obj.cells[c][r].sWall = 0;
            floodfill_obj.cells[c][r].wWall = 0;
        }
    }

    //Floodfill initial actions

    //Initialize mouse man to blank man template
    //Initialize cells to have init walls STEP 2
    for(int c = 0; c < 16; c++)
    {
        for(int r = 0; r < 16; r++)
        {
            floodfill_obj.manhattan_arr[c][r] = blank_manhattan_arr[c][r];

            if(c==0)
            {
                floodfill_obj.cells[c][r].wWall = 1;
            }
            if(c==15)
            {
                floodfill_obj.cells[c][r].eWall = 1;
            }
            if(r==0)
            {
                floodfill_obj.cells[c][r].sWall = 1;
            }
            if(r==15)
            {
               floodfill_obj. cells[c][r].nWall = 1;
            }
        }
    }

    Pose mouse_pose;
    init_mouse(&mouse_pose);
    int loop = 1;
    while(loop == 1)
    {
        loop = doNextMove(&mouse_pose);
    }
    debug_log("finished run");
}

