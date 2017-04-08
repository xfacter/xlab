#include <stdlib.h>

#include "xastar.h"

static int mapWidth = 0;
static int mapHeight = 0;
static int numEntities = 0;
static int onClosedList = 10;

int *pathLength;
int *pathLocation;
int *pathStatus;
int *xPath;
int *yPath;
bool **walkable;

static int *openList;
static int *openX;
static int *openY;
static int *Fcost;
static int *Hcost;
static int **whichList;
static int **parentX;
static int **parentY;
static int **Gcost;
static int **pathBank;

int abs(int x)
{
    if (x < 0) return -x;
    return x;
}

bool InitAstar(int width, int height, int entities)
{
    if (width <= 0 || height <= 0 || entities <= 0) return false;
    int i = 0;
    pathLength = (int*)malloc((entities+1)*sizeof(int));
    pathLocation = (int*)malloc((entities+1)*sizeof(int));
    pathStatus = (int*)malloc((entities+1)*sizeof(int));
    xPath = (int*)malloc((entities+1)*sizeof(int));
    yPath = (int*)malloc((entities+1)*sizeof(int));
    walkable = (bool**)malloc(width*sizeof(bool*));
    for (i = 0; i < width; i++) walkable[i] = (bool*)malloc(height*sizeof(bool));
    
    openList = (int*)malloc((width*height+2)*sizeof(int));
    openX = (int*)malloc((width*height+2)*sizeof(int));
    openY = (int*)malloc((width*height+2)*sizeof(int));
    Fcost = (int*)malloc((width*height+2)*sizeof(int));
    Hcost = (int*)malloc((width*height+2)*sizeof(int));
    pathBank = (int**)malloc((entities+1)*sizeof(int*));
    for (i = 0; i < entities+1; i++) pathBank[i] = (int*)malloc(sizeof(int));
    whichList = (int**)malloc((width+1)*sizeof(int*));
    parentX = (int**)malloc((width+1)*sizeof(int*));
    parentY = (int**)malloc((width+1)*sizeof(int*));
    Gcost = (int**)malloc((width+1)*sizeof(int*));
    for (i = 0; i < width+1; i++)
    {
        whichList[i] = (int*)malloc((height+1)*sizeof(int));
        parentX[i] = (int*)malloc((height+1)*sizeof(int));
        parentY[i] = (int*)malloc((height+1)*sizeof(int));
        Gcost[i] = (int*)malloc((height+1)*sizeof(int));
    }
    
    for (i = 0; i < entities+1; i++)
    {
        pathStatus[i] = 0;
        xPath[i] = 0;
        yPath[i] = 0;
    }
    mapWidth = width;
    mapHeight = height;
    numEntities = entities;
    return true;
}

void EndAstar()
{
    free(pathLength);
    free(pathLocation);
    free(pathStatus);
    free(xPath);
    free(yPath);
    for (int i = 0; i < mapWidth; i++) free(walkable[i]);
    free(walkable);
    
    free(openList);
    free(openX);
    free(openY);
    free(Fcost);
    free(Hcost);
    for (int i = 0; i < numEntities+1; i++) free(pathBank[i]);
    free(pathBank);
    
    for (int i = 0; i < mapWidth+1; i++)
    {
        free(whichList[i]);
        free(parentX[i]);
        free(parentY[i]);
        free(Gcost[i]);
    }
    free(whichList);
    free(parentX);
    free(parentY);
    free(Gcost);
}

int FindPath (int ID,int startX, int startY, int targetX, int targetY)
{
    if (ID < 0 || ID > numEntities ||
    startX < 0 || startY < 0 || targetX < 0 || targetY < 0 ||
    startX > mapWidth || startY > mapHeight ||
    targetX > mapWidth || targetY > mapHeight) return 0;
    
    int onOpenList=0;
    int parentXval=0;
    int parentYval=0;
    int a=0, b=0, m=0, u=0, v=0;
    int temp=0, corner=0;
    int numberOfOpenListItems=0;
	int addedGCost=0, tempGcost = 0, path = 0;
    int tempx, pathX, pathY;
    int cellPosition;
    int newOpenListItemID=0;
    
	if (startX == targetX && startY == targetY) return 0;
	
	if (walkable[targetX][targetY] == 0)
	{
    	xPath[ID] = startX;
    	yPath[ID] = startY;
    	return 0;
    }
    
	if (onClosedList > 1000000) //reset whichList occasionally
	{
		for (int x = 0; x < mapWidth; x++)
        {
		    for (int y = 0; y < mapHeight; y++)
            {
                whichList [x][y] = 0;
            }
		}
		onClosedList = 10;	
	}
	
	onClosedList += 2; //changing the values of onOpenList and onClosed list is faster than redimming whichList() array
	onOpenList = onClosedList - 1;
	pathLength [ID] = 0;
	pathLocation [ID] = 0;
	Gcost[startX][startY] = 0;
	
//4.Add the starting location to the open list of squares to be checked.
	numberOfOpenListItems = 1;
	openList[1] = 1;//assign it as the top (and currently only) item in the open list, which is maintained as a binary heap (explained below)
	openX[1] = startX;
    openY[1] = startY;
    
//5.Do the following until a path is found or deemed nonexistent.
	do
	{
        //6.If the open list is not empty, take the first cell off of the list.
        //	This is the lowest F cost cell on the open list.
    	if (numberOfOpenListItems > 0)
    	{
            //7. Pop the first item off the open list.
        	parentXval = openX[openList[1]];
        	parentYval = openY[openList[1]]; //record cell coordinates of the item
        	whichList[parentXval][parentYval] = onClosedList;//add the item to the closed list
        
            //	Open List = Binary Heap: Delete this item from the open list, which
            //  is maintained as a binary heap. For more information on binary heaps, see:
            //	http://www.policyalmanac.org/games/binaryHeaps.htm
        	numberOfOpenListItems -= 1;
        		
            //	Delete the top item in binary heap and reorder the heap, with the lowest F cost item rising to the top.
        	openList[1] = openList[numberOfOpenListItems+1];//move the last item in the heap up to slot #1
        	v = 1;
        	
            //	Repeat the following until the new item in slot #1 sinks to its proper spot in the heap.
        	do
        	{
            	u = v;		
            	if (2*u+1 <= numberOfOpenListItems) //if both children exist
            	{
            	 	//Check if the F cost of the parent is greater than each child.
            		//Select the lowest of the two children.
            		if (Fcost[openList[u]] >= Fcost[openList[2*u]]) v = 2*u;
            		if (Fcost[openList[v]] >= Fcost[openList[2*u+1]]) v = 2*u+1;
            	}
            	else
            	{
            		if (2*u <= numberOfOpenListItems) //if only child #1 exists
            		{
                	 	//Check if the F cost of the parent is greater than child #1	
            			if (Fcost[openList[u]] >= Fcost[openList[2*u]]) v = 2*u;
            		}
            	}
            
            	if (u != v) //if parent's F is > one of its children, swap them
            	{
            		temp = openList[u];
            		openList[u] = openList[v];
            		openList[v] = temp;
            	}
            	else break;
        	} while (1); //reorder the binary heap
            
            //7.Check the adjacent squares. (Its "children" -- these path children
            //	are similar, conceptually, to the binary heap children mentioned
            //	above, but don't confuse them. They are different. Path children
            //	are portrayed in Demo 1 with grey pointers pointing toward
            //	their parents.) Add these adjacent child squares to the open list
            //	for later consideration if appropriate (see various if statements
            //	below).
            if (parentXval >= 0 && parentYval >= 0 && parentXval < mapWidth && parentYval < mapHeight)
            {
                for (b = parentYval-1; b <= parentYval+1; b++)
                {
                    for (a = parentXval-1; a <= parentXval+1; a++)
                    {
                        //	If not off the map (do this first to avoid array out-of-bounds errors)
                    	if (a >= 0 && b >= 0 && a < mapWidth && b < mapHeight)
                        {
                            //	If not already on the closed list (items on the closed list have
                            //	already been considered and can now be ignored).
                        	if (whichList[a][b] != onClosedList)
                            {
                                //	If not a wall/obstacle square.
                            	if (walkable [a][b])
                                {
                                    //	Don't cut across corners
                                	corner = 1;
                                	if (a == parentXval-1)
                                	{
                                        if (b == parentYval-1)
                                		{
                                            if (walkable[parentXval-1][parentYval] == 0 ||
                                                walkable[parentXval][parentYval-1] == 0) corner = 0;
                                		}
                                		else if (b == parentYval+1)
                                		{
                                            if (walkable[parentXval][parentYval+1] == 0 ||
                                                walkable[parentXval-1][parentYval] == 0) corner = 0; 
                                		}
                                	}
                                	else if (a == parentXval+1)
                                	{
                                        if (b == parentYval-1)
                                		{
                                            if (walkable[parentXval][parentYval-1] == 0 ||
                                                walkable[parentXval+1][parentYval] == 0) corner = 0;
                                		}
                                		else if (b == parentYval+1)
                                		{
                                            if (walkable[parentXval+1][parentYval] == 0 ||
                                                walkable[parentXval][parentYval+1] == 0) corner = 0; 
                                		}
                                	}
                                	
                                    if (corner == 1)
                                    {
                                        //	If not already on the open list, add it to the open list.
                                    	if (whichList[a][b] != onOpenList) 
                                    	{
                                            //Create a new open list item in the binary heap.
                                            newOpenListItemID += 1; //each new item has a unique ID #
                                            m = numberOfOpenListItems + 1;
                                            openList[m] = newOpenListItemID;//place the new open list item (actually, its ID#) at the bottom of the heap
                                            openX[newOpenListItemID] = a;
                                            openY[newOpenListItemID] = b;//record the x and y coordinates of the new item
                                            
                                            //Figure out its G cost
                                            if (abs(a-parentXval) == 1 && abs(b-parentYval) == 1) addedGCost = 14;//cost of going to diagonal squares
                                            else addedGCost = 10;//cost of going to non-diagonal squares
                                            
                                            Gcost[a][b] = Gcost[parentXval][parentYval] + addedGCost;
                                            
                                            //Figure out its H and F costs and parent
                                            Hcost[openList[m]] = 10*(abs(a - targetX) + abs(b - targetY));
                                            Fcost[openList[m]] = Gcost[a][b] + Hcost[openList[m]];
                                            parentX[a][b] = parentXval ; parentY[a][b] = parentYval;
                                            
                                            //Move the new open list item to the proper place in the binary heap.
                                            //Starting at the bottom, successively compare to parent items,
                                            //swapping as needed until the item finds its place in the heap
                                            //or bubbles all the way to the top (if it has the lowest F cost).
                                            while (m > 1) //While item hasn't bubbled to the top (m=1)	
                                            {
                                    			//Check if child's F cost is < parent's F cost. If so, swap them.	
                                    			if (Fcost[openList[m]] <= Fcost[openList[m/2]])
                                    			{
                                    				temp = openList[m/2];
                                    				openList[m/2] = openList[m];
                                    				openList[m] = temp;
                                    				m = m/2;
                                    			}
                                    			else break;
                                    		}
                                    		numberOfOpenListItems += 1;//add one to the number of items in the heap
                                    		
                                    		//Change whichList to show that the new item is on the open list.
                                    		whichList[a][b] = onOpenList;
                                    	}
                                    	
                                        //8.If adjacent cell is already on the open list, check to see if this 
                                        //	path to that cell from the starting location is a better one. 
                                        //	If so, change the parent of the cell and its G and F costs.	
                                    	else //If whichList(a,b) = onOpenList
                                    	{
                                            //Figure out the G cost of this possible new path
                                    		if (abs(a-parentXval) == 1 && abs(b-parentYval) == 1) addedGCost = 14; //cost of going to diagonal tiles
                                    		else addedGCost = 10; //cost of going to non-diagonal tiles
                                    		tempGcost = Gcost[parentXval][parentYval] + addedGCost;
                                    		
                                    		//If this path is shorter (G cost is lower) then change
                                    		//the parent cell, G cost and F cost. 		
                                    		if (tempGcost < Gcost[a][b]) //if G cost is less,
                                    		{
                                    			parentX[a][b] = parentXval; //change the square's parent
                                    			parentY[a][b] = parentYval;
                                    			Gcost[a][b] = tempGcost;//change the G cost			
                                    
                                    			//Because changing the G cost also changes the F cost, if
                                    			//the item is on the open list we need to change the item's
                                    			//recorded F cost and its position on the open list to make
                                    			//sure that we maintain a properly ordered open list.
                                    			for (int x = 1; x <= numberOfOpenListItems; x++) //look for the item in the heap
                                    			{
                                        			if (openX[openList[x]] == a && openY[openList[x]] == b) //item found
                                        			{
                                        				Fcost[openList[x]] = Gcost[a][b] + Hcost[openList[x]];//change the F cost
                                        				
                                        				//See if changing the F score bubbles the item up from it's current location in the heap
                                        				m = x;
                                        				while (m > 1) //While item hasn't bubbled to the top (m=1)	
                                        				{
                                        					//Check if child is < parent. If so, swap them.	
                                        					if (Fcost[openList[m]] < Fcost[openList[m/2]])
                                        					{
                                        						temp = openList[m/2];
                                        						openList[m/2] = openList[m];
                                        						openList[m] = temp;
                                        						m = m/2;
                                        					}
                                        					else break;
                                        				} 
                                        				break; //exit for x = loop
                                        			} //If openX(openList(x)) = a
                                    			} //For x = 1 To numberOfOpenListItems
                                    		}//If tempGcost < Gcost(a,b)
                                    	}//else If whichList(a,b) = onOpenList	
                                	}//If not cutting a corner
                            	}//If not a wall/obstacle square.
                        	}//If not already on the closed list 
                    	}//If not off the map
                	}//for (a = parentXval-1; a <= parentXval+1; a++)
            	}//for (b = parentYval-1; b <= parentYval+1; b++)
            }
    	}//if (numberOfOpenListItems != 0)
    	
        //9.If open list is empty then there is no path.
    	else
    	{
    		path = 0;
            break;
    	}
    	
    	//If target is added to open list then path has been found.
    	if (whichList[targetX][targetY] == onOpenList)
    	{
    		path = 1;
            break;
    	}
	} while (1);//Do until path is found or deemed nonexistent
	
    //10.Save the path if it exists.
	if (path == 1)
	{
        //a.Working backwards from the target to the starting location by checking
        //	each cell's parent, figure out the length of the path.
    	pathX = targetX; pathY = targetY;
    	do
    	{
    		//Look up the parent of the current cell.	
    		tempx = parentX[pathX][pathY];		
    		pathY = parentY[pathX][pathY];
    		pathX = tempx;
    
    		//Figure out the path length
    		pathLength[ID] = pathLength[ID] + 1;
    	} while (pathX != startX || pathY != startY);
    	
        //b.Resize the data bank to the right size in bytes
       	pathBank[ID] = (int*)realloc(pathBank[ID], pathLength[ID]*8);
       	
        //c. Now copy the path information over to the databank. Since we are
        //	working backwards from the target to the start location, we copy
        //	the information to the data bank in reverse order. The result is
        //	a properly ordered set of path data, from the first step to the
        //	last.
    	pathX = targetX ; pathY = targetY;
    	cellPosition = pathLength[ID]*2;//start at the end	
    	do
    	{
        	cellPosition -= 2;//work backwards 2 integers
        	pathBank[ID] [cellPosition] = pathX;
        	pathBank[ID] [cellPosition+1] = pathY;
        	
            //d.Look up the parent of the current cell.	
        	tempx = parentX[pathX][pathY];
        	pathY = parentY[pathX][pathY];
        	pathX = tempx;
        //e.If we have reached the starting square, exit the loop.
    	} while (pathX != startX || pathY != startY);
    	
        //11.Read the first path step into xPath/yPath arrays
    	UpdatePath(ID);
	}
	return path;
}

//-----------------------------------------------------------------------------
// Name: ReadPathX
// Desc: Reads the x coordinate of the next path step
//-----------------------------------------------------------------------------
static int ReadPathX(int ID,int pathLocation)
{
    if (pathLocation > pathLength[ID]) return -1;
	return pathBank[ID][pathLocation*2-2];
}

//-----------------------------------------------------------------------------
// Name: ReadPathY
// Desc: Reads the y coordinate of the next path step
//-----------------------------------------------------------------------------
static int ReadPathY(int ID,int pathLocation)
{
    if (pathLocation > pathLength[ID]) return -1;
	return pathBank[ID][pathLocation*2-1];
}

void UpdatePath(int ID)
{
	//If a path has been found for the pathfinder	...
	//if (pathStatus[ID] <= 0) return;
    pathLocation[ID]++;
    
	//Read the path data.
	xPath[ID] = ReadPathX(ID,pathLocation[ID]);
	yPath[ID] = ReadPathY(ID,pathLocation[ID]);
	
    //If the last path square on the path has been reached then reset.
	if (xPath[ID] < 0 || yPath[ID] < 0 ||
        xPath[ID] >= mapWidth || yPath[ID] >= mapHeight ||
        pathLocation[ID] > pathLength[ID]) pathStatus[ID] = 0;
}
