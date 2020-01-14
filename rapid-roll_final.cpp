#include"graphics.h"
#include"winbgim.h"
#include<string.h>
#include<ctime>
#include<stdio.h>
#include<math.h>
#define INF 9999999
#define rightx -9999
#define leftx 9999
#define xmin 300
//#define bounce 4
#define xmax 1300
#define ymax 700
#define ymin 0
#define bar_width 100
#define bar_height 2
#define segment 10
#define rad 16
#define sit 1
#define fall 2
#define jump 3
#define vi 20 // vy/vx = slope,m(dir)
				// (m*m+1)*vx*vx = vi*vi

#define g 1 // (g=)vy/vx = slope = dir; vx = vy/slope

int ballstate,ballx,bally,stop,nrow,xpoint,ypoint,vy,vx,yrow,ballregion,score,exiti,click,level,thorn;
char s[20],name[15],players[15][15];

int scores[15];

double dir,jumpto;

int region[2][15];	//whole screen is divided into 9 bar regions (X axis) : 300-400,400-500,...,1200-1300
					/*
					region[][]=0 : no bar
							1,2,3  : normal bar
							4,5    : thorn
							6	   : bonus
					*/

void drawheart(int x, int y); // x= middle point on x axis
							//y = lowest point on y axis
void drawball(int x, int y); //(x,y) is centre
void select(void);	//selects whether a region will be empty or contain a bar
void drawbar(int y,int row);	/*draws bars at the selected regions and y co-ordinate 
							(lowest y), (xmin+(region-1)*100 = lowest x)
							There will be at least 1 and at most 2 rows of bar on the screen
							the upper/older row is 1, lower/newer is 0
							*/

void setdirection(void);
int obstacle(int x1,int y1, int x2,int y2); //return 0 = no obstacle, return 1 = sit on upper row, return 2 = sit on lower row, return 3 = bounce

void updatescore(void);
void bounce(int x, int towards);
int pos,low;
void sort(int len);
void intro(void);

void showscore(void);
void showins(void);
int play(void);
void setlevel(void);
void quit(void);

void quit(void)
{
	FILE *f;
	setfillstyle(SOLID_FILL,BLACK);
		fillellipse(750,400,1300,1300);
		setcolor(WHITE);
		if(score<100)
		{
			readimagefile("loser.jpg",400,200,900,400);
		}
		else if(score<150)
		{
			settextstyle(0,0,16);
			outtextxy(500,200,"GOOD! :)");
			//readimagefile("good.jpg",600,200,1000,400);
		}
		else if(score<200)
		{
			settextstyle(0,0,16);
			outtextxy(500,200,"VERY GOOD! :D");
			//readimagefile("verygood.jpg",600,200,1000,400);
		}
		else
		{
			settextstyle(0,0,16);
			outtextxy(500,200,"EXCELLENT! ^_^");
			//readimagefile("excellent.jpg",600,200,1000,400);
		}
		setcolor(WHITE);
		settextstyle(1,0,20);
		sprintf(s,"%d",score);
		outtextxy(460,410,"Your Score is  ");
		outtextxy(760,410,s);
		char c;
		int i,j;
		if((f=fopen("score.txt","r"))==NULL)
		{
			f=fopen("score.txt","w");
			fclose(f);
		}
		fscanf(f,"%d",&low);
		if(low >= score)
			getch();
		if(low<score)
		{
		i=0;
		outtextxy(460,440,"Please enter your name : ");
		while(fscanf(f,"%d",&scores[i])==1)
		{
			fscanf(f,"%s",players[i]);
			++i;
		}
		sort(i);
		printf("position %d\n",pos);
		if(i>=10)
			j=9;
		else
			j=i;
		fclose(f);
		i=0;
		for(i=0; i<20; ++i)
			s[i]=' ';
		s[19]='\0';
		i=0;
		while(1)
		{
			c=s[i]=getch();
			if((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='.' || c=='_')
			{
				++i;
				outtextxy(580,480,s);
				break;
			}
		}
		while(((c=getch()) != 13 && i<10) || (i==0))
		{
			if((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='.' || c=='_')
			{
				s[i]=c;
				++i;
				outtextxy(580,480,s);
			}
			else if(c == '\b'&& i>0)
			{
				s[--i]=' ';
				outtextxy(560,470,s);
				//setfillstyle(SOLID_FILL,CYAN);
				printf("i %d\n",i);
				//bar(560+i*10,470,570+i*10,510);
			}
		}
		f=fopen("score.txt","w");
		fprintf(f,"%d\n",low);
		for(i=0; i<pos; ++i)
		{
			fprintf(f,"%10d %s\n",scores[i],players[i]);
		}
		low=i;
		fprintf(f,"%10d %s\n",score,s);
		fprintf(f,"\n");
		i=low;
		for(; i<j; ++i)
		{
			fprintf(f,"%10d %s\n",scores[i],players[i]);
		}
		fclose(f);
}
}
void setlevel (void)
{
	int x,y;
	clearmouseclick(WM_LBUTTONDOWN);
	readimagefile("level.jpg",0,0,xmax,ymax);
	while(1)
	{
		x=mousex();
		y=mousey();
		if(x>=500 && x<=741 && y>=200 && y<=250)
		{
			readimagefile("fair_light.jpg",500,200,500+241,200+50);
			readimagefile("feisty_def.jpg",500,300,500+279,300+49);
			if(ismouseclick(WM_LBUTTONDOWN))
			{
				level=1;
				break;
			}
		}
		else if(x>=500 && x<=779 && y>=300 && y<=349)
		{
			readimagefile("feisty_light.jpg",500,300,500+279,300+49);
			readimagefile("fair_def.jpg",500,200,500+241,200+50);
			if(ismouseclick(WM_LBUTTONDOWN))
			{
				level=2;
				printf("level %d\n",level);
				break;
			}
		}
		else
		{
			readimagefile("feisty_def.jpg",500,300,500+279,300+49);
			readimagefile("fair_def.jpg",500,200,500+241,200+50);
			if(ismouseclick(WM_LBUTTONDOWN))
				clearmouseclick(WM_LBUTTONDOWN);
		}
	}
	clearmouseclick(WM_LBUTTONDOWN);
}

void showins(void)
{
	clearmouseclick(WM_LBUTTONDOWN);
	readimagefile("instruct.jpg",0,ymin,xmax,ymax);
	while(!ismouseclick(WM_LBUTTONDOWN ));
	clearmouseclick(WM_LBUTTONDOWN);
}

void showscore(void)
{
	char name[15],val[15];
	int num,i=0;
	FILE *f;
	clearmouseclick(WM_LBUTTONDOWN);
	readimagefile("highscores.jpg",0,ymin,xmax,ymax);
	if((f=fopen("score.txt","r")) != NULL)
	{
		fscanf(f,"%d",&num);
		while(fscanf(f,"%d",&num)==1)
		{
			sprintf(val,"%d",num);
			outtextxy(1000,200+i*30,val);
			fscanf(f,"%s",name);
			outtextxy(400,200+i*30,name);
			++i;
		}
		fclose(f);
	}
	while(!ismouseclick(WM_LBUTTONDOWN));
	clearmouseclick(WM_LBUTTONDOWN);
}

void sort(int len)
{
	int i,j,k;
	for(i=len-1; i>=0; --i)
	{
		if(score <= scores[i])
		{
			pos=i+1;
			break;
		}
	}
	if(i<0)
		pos=0;
	if(pos==9)
		low=score;
	else if(len>=9)
		low=scores[8];
	else
		low=0;
}

void bounce(int x, int towards)
{
			vx*=-1;
			if(towards == leftx)
				ballx = x-rad;
			else
				ballx = x+rad;
			if(vy>0)
				jumpto*=-1;
}

void updatescore(void)
{
	setcolor(WHITE);
	settextstyle(1,0,20);
	sprintf(s,"%d",score);
	outtextxy(10,20,"Score:");
	outtextxy(10,70,s);
}
void moveball(void)
{
	int oldx,oldy,state,xx,yy,leftreg,rightreg;
	oldx=ballx;
	oldy=bally;
	if(ballstate == sit)
	{
		bally-=5;
	}
	if(ballstate == jump)
	{
		vy+=g;
		bally += vy;
		ballx+=vx;
		if(ballx+rad >= xmax)
		{
			bounce(xmax,leftx);

		}
		else if( ballx-rad <= xmin)
		{
			bounce(xmin,rightx);
		}
		else
		{
			if(bally > oldy )
			{
				if((bally >= yrow && oldy <= yrow ) || (bally >= yrow+4 && oldy <= yrow+4))
				{
				
					yy = yrow-rad;
					if(yy<oldy)
						xx=oldx;
					else if(vy)
						xx = oldx+((yy-oldy)/((double)(vy)))*vx;
					else
						xx=ballx;
					ballregion=1+(xx-xmin)/100;
					leftreg=1+(xx-xmin-rad+2)/100;
					rightreg=1+(xx-xmin+rad-2)/100;
					if((region[1][leftreg] != 0 ) || (region[1][rightreg] != 0))
					{
						bally=yy;
						ballx=xx;
						if((region[1][leftreg] != 0 && region[1][leftreg] != 4 && region[1][leftreg] != 5) || (region[1][rightreg] != 0 && region[1][rightreg] != 4 && region[1][rightreg] != 5))
										thorn=0;
									else
										thorn=1;
						if(ballstate != sit)
						{
							if(thorn==0)
								score+=10;
							else
							{
								if(score>0)
									score-=10;
								else
								{
									exiti=1;
									return;
								}
							}
							updatescore();
						}
						ballstate=sit;
						
					}
					
				}
				else if(nrow == 2 && (bally >= yrow+400 && oldy <= yrow+400) || (bally >= yrow+400+4 && oldy <= yrow+400+4))
				{
					
					yy = yrow-rad+400;
					if(yy<oldy)
						xx=oldx;
					else if(vy)
						xx = oldx+((yy-oldy)/((double)(vy)))*vx;
					else
						xx=ballx;
					leftreg=1+(xx-xmin-rad+2)/100;
					rightreg=1+(xx-xmin+rad-2)/100;
					if((region[0][leftreg] != 0) || (region[0][rightreg] != 0 ))
					{
						ballx=xx;
						bally=yy;
						if((region[0][leftreg] != 0 && region[0][leftreg] != 4 && region[0][leftreg] != 5) || (region[0][rightreg] != 0 && region[0][rightreg] != 4 && region[0][rightreg] != 5))
							thorn=0;
						else
							thorn=1;
						if(ballstate != sit)
						{
							if(thorn==0)
								score+=10;
							else
							{
								if(score>0)
									score-=10;
								else
								{
									exiti=1;
									return;
								}
							}
							updatescore();
						}
						
						ballstate=sit;
					
					}
				}
			}
			else if(bally < oldy)
			{
				
				if((bally <= yrow && oldy >= yrow ) || (bally <= yrow-4 && oldy >= yrow-4))
				{
					yy = yrow+rad;
					if(vy)
						xx = oldx + ((yy-oldy)/((double)(vy)))*vx;
					else
						xx=ballx;
					ballregion=1+(xx-xmin)/100;
					leftreg=1+(xx-xmin-rad+2)/100;
					rightreg=1+(xx-xmin+rad-2)/100;
					if((region[1][leftreg] != 0 ) || (region[1][rightreg] != 0 ))
					{
						bally=yy;
						ballx=xx;
						vy=g;
					}
				}
				else if(nrow == 2 && (bally <= yrow+400 && oldy >= yrow+400) || (bally >= yrow+400-4 && oldy <= yrow+400-4))
				{
					yy = yrow+rad+400;
					if(vy)
						xx = oldx + ((yy-oldy)/((double)(vy)))*vx;
					else
						xx=ballx;
					ballregion=1+(xx-xmin)/100;
					leftreg=1+(xx-xmin-rad+2)/100;
					rightreg=1+(xx-xmin+rad-2)/100;
					if((region[0][leftreg] != 0 ) || (region[0][rightreg] != 0 ))
					{
						ballx=xx;
						bally=yy;
						
						vy=g;
					}
				}
			}
			if(ballx+rad >= xmax)
			{
				
				bounce(xmax,leftx);

			}
			else if( ballx-rad <= xmin)
			{
				bounce(xmin,rightx);
			}
		}
		
		
	}
}



void setdirection(void)
{
	int x,y,dis,tox,toy,xtoball,ytoball;
	double m,n;
	
	x = mousex();
	y = mousey();
	dis = sqrt((double)((x-(ballx))*(x-(ballx))+(y-bally)*(y-bally)));
	if(dis > 100)
	{
		m = 100.0/dis;
		n = 1-m;
		tox = m*x+n*(ballx);
		toy = m*y+n*(bally);

	}
	else
	{
		m=1;
		n=1-(dis/100.0);
		if(m != n)
		{
			tox = (m*x-n*(ballx))/(m-n);
			toy = (m*y-n*(bally))/(m-n); 
		}
		else
		{
			tox=x;
			toy=y;
		}
		 
	}
	if(toy > bally)
	{
		toy = bally;
		if(tox > (ballx))
		{
			tox = (ballx)+100;
			
		}
		else
		{
			tox = (ballx)-100;
			
		}
	}
	setcolor(BLACK);
	if(ballx >= xmin+100)
		arc(ballx,bally,0,180,100);
	else
	{
		arc(ballx,bally,0,180-(acos((ballx-xmin)/100.0)*(180/acos(-1.0))),100);
	}
	if(tox >= xmin)
		line(ballx, bally,tox,toy);
	else
		line(ballx, bally,xmin,toy);
	
	if(ballx == tox)
		dir = INF;
	else if(bally == toy && tox >= ballx)
		dir = rightx;
	else if(bally==toy && tox < ballx)
		dir = leftx;
	else
		dir = ((double)(toy-(bally)))/((ballx)-tox);
	
}
void drawball(int x, int y)
{

	setcolor(BLACK);
	setfillstyle(SOLID_FILL,YELLOW);
	fillellipse(x,y,rad,rad);
	setcolor(BLACK);
	setlinestyle(0,0,2);
	if(ballstate==sit && thorn==0)
		arc(x,y+3,180,360,5);
	else if(ballstate != sit)
		arc(x,y+7,20,160,5);
	setfillstyle(SOLID_FILL,BLACK);
	fillellipse(x-8,y-5,3,2);
	fillellipse(x+8,y-5,3,2);

}
void drawheart(int x, int y)
{
	int a[12]= {x,y,x-10,y-20,x-10+5,y-20-5,x,y-20,x+5,y-20-5,x+10,y-20};
	setfillstyle(SOLID_FILL,RED);
	fillpoly(6,a);
}

void drawbar(int y,int row)
{
	int i,left,x,j;
	setfillstyle(SOLID_FILL,RED);
	for(i=1; i<=segment; ++i)
	{
		if(region[row][i] ==4 || region[row][i] == 5)	//thorn
		{
			setcolor(BLACK);
			x= xmin+(i-1)*100;
			for(j=0; j<5; ++j)
			{
				line(x,y,x+10,y-20);
				x+=10;
				line(x,y-20,x+10,y);
				x+=10;
			}
		}
		else if(region[row][i])		//not thorn
		{
			
			left=xmin+(i-1)*100;
			setfillstyle(SOLID_FILL,RED);
			bar(left,y-bar_height,left+bar_width,y);
			if(region[row][i]==6)
				drawheart(left+(bar_width/2),y-bar_height);
		}
	}
}

void select(void)
{
	int i,last=1,pick;
	for(i=1+rand()%4; i<segment; )
	{
		if(last < 4)
		{
			if(level==1)
				last = region[0][i]=1+rand()%3;
			else
				last = region[0][i]=1+rand()%4;
		}
		else if(last==6)
			last = region[0][i]=1+rand()%3;
		else
		{
			pick = rand()%2;
			if(!pick)		//normal bar
				last = region[0][i]=1;
			else
				last=region[0][i]=1;
		}
		i = i+ 2+rand()%3;
	}
	putchar('\n');
	
}
void intro()
{
	int x,y,sel=0;
	//clearmouseclick(WM_LBUTTONDOWN);
	while(1)
	{
		readimagefile("intro_default.jpg",0,0,xmax,ymax);
	while(!ismouseclick(WM_LBUTTONDOWN ))
	{
		sel=0;
		x=mousex();
		y=mousey();
		if(x>=800 && x<=800+215 && y>=200 && y<=200+35)
		{
			readimagefile("instructions_light.jpg",800,200,800+215,200+35);
			sel=2;
		}
		else
		{
			readimagefile("instructions_def.jpg",800,200,800+215,200+35);
		}
		if(x>=800 && x<=800+252 && y>=300 && y<=300+44)
		{
			readimagefile("settings_light.jpg",800,300,800+252,300+44);
			sel=3;
		}
		else
		{
			readimagefile("settings_def.jpg",800,300,800+252,300+44);
		}
		if(x>=800 && x<=800+190 && y>=400 && y<=400+46)
		{
			readimagefile("highscores_light.jpg",800,400,800+190,400+46);
			sel=4;
		}
		else
		{
			readimagefile("highscores_def.jpg",800,400,800+190,400+46);
		}
		if(x>=800 && x<=800+71 && y>=500 && y<=500+34)
		{
			readimagefile("exit_light.jpg",800,500,800+71,500+34);
			sel=5;
		}
		else
		{
			readimagefile("exit_def.jpg",800,500,800+71,500+34);
		}
		if(x >= 150 && x <= 150+381 && y >= 400 && y <= 400+158 )
		{
			readimagefile("play_light.jpg",150,400,150+381,400+158);
			sel=6;
		}
		else
		{
			readimagefile("play_def.jpg",150,400,150+381,400+158);
		}
	}
	clearmouseclick(WM_LBUTTONDOWN);
	if(exiti==1)
		return;
	if(sel==6)
	{
		play() ;
		quit();
	}
	else if(sel==2)
	{
		showins();
	}
	else if(sel==3)
	{
		setlevel();
	}
	else if(sel==4)
	{
		showscore();
	}
	else if(sel==5)
	{
		break;
	}
}
}

int main()
{
	srand(time(0));
	initwindow(xmax,ymax);
	settextstyle(1,0,20);
	level=1;
	intro();
	return 0;
}

int play(void)
{
	int y,x,i,maxy;
	char c;
	FILE *f;
	//initwindow(xmax,ymax);
	exiti=0;
	while(1)
	{
		memset(region,0,sizeof(region));
		memset(s,0,sizeof(s));
		memset(scores,0,sizeof(scores));
		ballstate=ballx=bally=nrow=xpoint=ypoint=vy=vx=yrow=ballregion=score=pos=low=stop=thorn=0;
		dir=jumpto=0;
	//intro();
	if(exiti==1)
	{
		delay(50);
		break;
	}
	clearmouseclick(WM_LBUTTONDOWN);
	delay(200);
	readimagefile("scoreboard.jpg",0,0,xmin,ymax);
	setcolor(BLACK);
	updatescore();
	select();
	ballstate = sit;
	nrow=1;
	for(i=1;i<=segment; ++i)
	{
		region[1][i]=region[0][i];
		if(((region[1][i]>=1 && region[1][i]<=3) ||  region[1][i]==6) && !ballx && !bally)
		{
			ballx = xmin+(bar_width/2)+(i-1)*100;
			bally = ymax+5-rad;
			if(region[1][i]==6)
				region[1][i]=1;
		}
	}
		maxy=ymax;
	
		while(1)
		{
			printf("%d %d\n",ballx,1+(ballx-xmin)/100);
			nrow=1;
			for(y=maxy; y>= ymin+bar_height; y-=5)
			{
				//updatescore();
				yrow=y;
				readimagefile("back.jpg",xmin,ymin,xmax,ymax);
				drawbar(y,1);						////for the only or the older/upper row
				if(y==300)							////if older_row(y) == 300, select regions for new row
				{
					for(i=0; i<=segment; ++i)
						region[0][i]=0;
					select();
				}									
				if(y <= 300)
				{
					//draw new row
					drawbar(y+400,0);
					nrow=2;
				}
				
				
				if(ballstate==sit)
					setdirection();
				
				if(ismouseclick(WM_LBUTTONDOWN ))
				{
					click=0;
					jumpto=dir;
					clearmouseclick(WM_LBUTTONDOWN );
					
						vx= (vi*vi)/((jumpto*jumpto)+1);
						if(jumpto==0)
						{
							if(vx>=0)
								jumpto=rightx;
							else
								jumpto=leftx;

						}
						if(jumpto==INF)
						{
							ballstate=jump;
							vx = 0;
							vy=-vi;
						}
						else if(jumpto == rightx)
						{
							vy = g+bar_height;
							ballx+=20;
							if(ballstate==sit)
							{
								if(bally<yrow  && region[1][1+((ballx-xmin)/100)])
								{
									if(region[1][1+((ballx-xmin)/100)] != 4 && region[1][(1+((ballx-xmin)/100))] != 5)
										thorn=0;
									else
										thorn=1;
										
									ballstate=sit;

								}
								else if(nrow==2 && bally>yrow && region[0][1+((ballx-xmin)/100)])
								{		
									if(region[0][1+((ballx-xmin)/100)] != 4 && region[0][(1+((ballx-xmin)/100))] != 5)
										thorn=0;
									else
										thorn=1;
									ballstate=sit;
								}
								else
								{
									ballstate=jump;
									bally= bally+rad+bar_height+2+4;
								}

							}
							else
							{
								ballstate=jump;
								bally= bally+rad+bar_height+2+4;
							}
							
							vx = vi;
						}
						else if(jumpto==leftx)
						{
							vy=g+bar_height;
							ballx-=20;
							if(ballx<xmin+rad)
								ballx=xmin+rad;
							if((ballx+20)/100 == (ballx/100) && ballstate==sit)
							{
								if(bally<yrow  && region[1][1+((ballx-xmin)/100)])
								{
									if(region[1][1+((ballx-xmin)/100)] != 4 && region[1][(1+((ballx-xmin)/100))] != 5)
										thorn=0;
									else
										thorn=1;
										ballstate=sit;
								}
								else if(nrow==2 && bally>yrow && region[0][1+((ballx-xmin)/100)])
								{	
									if(region[0][1+((ballx-xmin)/100)] != 4 && region[0][(1+((ballx-xmin)/100))] != 5)
										thorn=0;
									else
										thorn=1;
									ballstate=sit;
								}
								else
								{
									ballstate=jump;
									bally= bally+rad+bar_height+2+4;
								}
							}
							else
							{
								ballstate=jump;
								bally= bally+rad+bar_height+2+4;
							}
							vx = -vi;
						}
						else if(jumpto > 0)
						{
							vx = sqrt((double)(vx));
							vy=-sqrt((double)((vi*vi)-(vx*vx)));
							ballstate=jump;
						}
						else if(jumpto < 0)
						{
							vx = -sqrt((double)vx);
							vy=-sqrt((double)((vi*vi)-(vx*vx)));
								ballstate=jump;
						}
						
				}
				if(bally <(rad) || bally > ymax-bar_height+5)
				{
					if(bally<rad)
						bally=rad;
					else
						bally = ymax-rad;
					drawball(ballx,bally);
					printf("%d\n",bally);
					stop=1;
					break;
				}
				moveball();
				if(exiti==1)
				{
					
					exiti=0;
					return 0;
				}
				if(y == bally+rad+bar_height && region[1][1+((ballx-xmin)/100)])
				{	
					if(region[1][1+((ballx-xmin)/100)] != 4 && region[1][(1+((ballx-xmin)/100))] != 5)
						thorn=0;
					else
						thorn=1;
					if(ballstate != sit)
					{
						
							if(thorn==0)
								score+=10;
							else
							{
								if(score>0)
									score-=10;
								else
								{
									
									return 0;
								}
							}
							updatescore();
						}
					ballstate=sit;
					
				}
				else if(nrow==2 && y == bally+rad+bar_height-400 && region[0][1+((ballx-xmin)/100)] )
				{	
					if(region[0][1+((ballx-xmin)/100)] != 4 && region[0][(1+((ballx-xmin)/100))] != 5)
						thorn=0;
					else
						thorn=1;
					if(ballstate != sit)
					{
							
							if(thorn==0)
								score+=10;
							else
							{
								if(score>0)
									score-=10;
								else
								{
									
									return 0;
								}
							}
							updatescore();
					}
					ballstate=sit;
					
				}
			
				ballregion=1+(ballx-xmin)/100;
				drawball(ballx,bally);
				delay(30);
			}
			if(stop)
				break;
			maxy=y+400;
			for(i=1;i<=segment; ++i)
			{
				region[1][i]=region[0][i];
			}
		}
		return 0;
		}
		exiti=0;
		return 0;
		//intro();
		//if(exiti==1)
			//break;
	exiti=0;
	return 0;
}







