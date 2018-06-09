#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <termios.h>

typedef struct
{
  int x;
  int y;
  char move;
}enemy;

typedef enum
{
  BLACK,
  RED,
  GREEN,
  BROWN,
  BLUE,
  MAGENTA,
  CYAN,
  LIGHTGRAY,
  DARKGRAY,
  LIGHTRED,
  LIGHTGREEN,
  YELLOW,
  LIGHTBLUE,
  LIGHTMAGENTA,
  LIGHTCYAN,
  WHITE
} COULEUR_TERMINAL;

int color_printf(COULEUR_TERMINAL fg, COULEUR_TERMINAL bg, const char * format, ...)
{
  int res;
  va_list args;
  printf("\x1B[1;%02d;%02dm", fg + 30, bg + 40);
  va_start(args, format);
  res = vprintf(format, args);
  va_end(args);
  printf("\x1B[00m");
  return res;
}

int getch()
{

	struct termios oldt, newt;
	int ch;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

	return ch;
}

void init_enemy(int* nbr_enemy, enemy grp_enmy[], FILE *f, int **tab2)
{
  fscanf(f, "%d", nbr_enemy);
  int k;
  for(k=0; k<*nbr_enemy; k++)
  {
    fscanf(f, "%d", &(grp_enmy[k].x));
    fscanf(f, "%d", &(grp_enmy[k].y));
    if(tab2[(grp_enmy[k].x)-1][grp_enmy[k].y]==1)
    {
      grp_enmy[k].move='d';
    }
    else
    {
      grp_enmy[k].move='q';
    }
  }
}

int** init_board(char level[], int* cote, int* nbr_enemy, enemy grp_enmy[])
{
  int i=0;
  int j=0;
  FILE *f = fopen(level, "rt");

  if (f == NULL)
  {
    printf("Probleme lors de la lecture du fichier\n");
    exit(1);
  }
  fscanf(f, "%d", cote);
  int **tab2=malloc(sizeof(int*)*(*cote));
  if(tab2==NULL)
  {
    printf("ERREUR : Probleme de memoire !!!!!\n");
    exit(1);
  }

  while(i<(*cote))
  {
    tab2[i]=malloc(sizeof(int)*(*cote));

    if(tab2[i]==NULL)
    {
      printf("ERREUR : Probleme de memoire !!!!!\n");
      exit(1);
    }
    while (j<(*cote))
    {
      fscanf(f, "%d", &tab2[i][j]);
      j++;
    }
    j=0;
    i++;
  }
  init_enemy(nbr_enemy, grp_enmy, f, tab2);
  fclose(f);
  return tab2;
}

int** select_level(int *lvl, int* cote, int* nbr_enemy, enemy grp_enmy[])
{
  char s[30];
  (*lvl)++;
  sprintf( s, "level/lv%d.txt", *lvl);
  return(init_board(s, cote, nbr_enemy, grp_enmy));
}

void free_board(int **tab2, int cote)
{
  int i=0;
  while(i<cote)
  {
    free(tab2[i]);
    i++;
  }
  free(tab2);
}

void print_board(int **tab2, int cote)
{
  int i,j;
  for(i=0; i<cote;i++)
  {
    for(j=0; j<cote;j++)
    {
      if(!tab2[i][j])
      {
        color_printf(YELLOW, BROWN, "   ");
      }
      else if(tab2[i][j]==-1)
      {
        color_printf(BLACK, BROWN, " O ");
      }
      else if(tab2[i][j]==2)
      {
        color_printf(WHITE, BROWN, "  ");
      }
      else if(tab2[i][j]==-2)
      {
        color_printf(RED, BROWN, " ø ");
      }
      else
      {
        color_printf(YELLOW, RED, "   ");
      }
    }
    printf("\n");
  }
}

void get_position_player(int** tab2, int cote, int* x, int* y)
{
  int i,j;
  for(i=0; i<cote; i++)
  {
    for(j=0; j<cote; j++)
    {
      if(tab2[i][j]==-1)
      {
        *x=i;
        *y=j;
        return;
      }
    }
  }
}

int test_move(int** tab2, char move, int cote, int x, int y)
{
  if((move=='z' || move=='w') && (tab2[x-1][y]==1 || tab2[x-1][y]==-1))
  {
    return 0;
  }

  else if(move=='s' && (tab2[x+1][y]==1 || tab2[x+1][y]==-1))
  {
    return 0;
  }

  else if(move=='d' && (tab2[x][y+1]==1 || tab2[x][y+1]==-1))
  {
    return 0;
  }

  else if((move=='q' || move=='a') && (tab2[x][y-1]==1 || tab2[x][y-1]==-1))
  {
    return 0;
  }

  else
  {
    return 1;
  }
}

int win(int** tab2, char move, int cote, int x, int y)
{
  if((move=='z' || move=='w') && tab2[x-1][y]==2)
  {
    printf("EPIC WIN\n");
    return 1;
  }

  else if(move=='s' && tab2[x+1][y]==2)
  {
    printf("EPIC WIN\n");
    return 1;
  }

  else if(move=='d' && tab2[x][y+1]==2)
  {
    printf("EPIC WIN\n");
    return 1;
  }

  else if((move=='q' || move=='a') && tab2[x][y-1]==2)
  {
    printf("EPIC WIN\n");
    return 1;
  }

  else
  {
    return 0;
  }
}

int game_over(int** tab2, char move, int cote, int x, int y)
{
  if((move=='z' || move=='w') && tab2[x-1][y]==-2)
  {
    printf("EPIC FAIL\n");
    return 1;
  }

  else if(move=='s' && tab2[x+1][y]==-2)
  {
    printf("EPIC FAIL\n");
    return 1;
  }

  else if(move=='d' && tab2[x][y+1]==-2)
  {
    printf("EPIC FAIL\n");
    return 1;
  }

  else if((move=='q' || move=='a') && tab2[x][y-1]==-2)
  {
    printf("EPIC FAIL\n");
    return 1;
  }

  else
  {
    return 0;
  }
}

void fct_move(int** tab2, char move, int cote, int x, int y)
{
  if((move=='z' || move=='w'))
  {
    tab2[x-1][y]=-1;
    tab2[x][y]=0;
  }

  else if(move=='s')
  {
    tab2[x+1][y]=-1;
    tab2[x][y]=0;
  }

  else if(move=='d')
  {
    tab2[x][y+1]=-1;
    tab2[x][y]=0;
  }

  else if((move=='q' || move=='a'))
  {
    tab2[x][y-1]=-1;
    tab2[x][y]=0;
  }
}

void fct_move_enmy(int** tab2, enemy grp_enmy[], int cote, int nbr_enemy)
{
  int i;
  for(i=0; i<nbr_enemy; i++)
  {
    if(test_move(tab2, grp_enmy[i].move, cote, grp_enmy[i].x, grp_enmy[i].y))
    {
      if(grp_enmy[i].move=='d')
      {
        tab2[grp_enmy[i].x][(grp_enmy[i].y)+1]=-2;
        tab2[grp_enmy[i].x][grp_enmy[i].y]=0;
        grp_enmy[i].y++;
      }

      else
      {
        tab2[grp_enmy[i].x][(grp_enmy[i].y)-1]=-2;
        tab2[grp_enmy[i].x][grp_enmy[i].y]=0;
        grp_enmy[i].y--;
      }
    }
    else
    {
      if(grp_enmy[i].move=='d')
      {
        grp_enmy[i].move='q';
      }

      else
      {
        grp_enmy[i].move='d';
      }
    }
  }
}

int movement(int** tab2, char move, int cote, int* GO)
{
  int x,y;
  get_position_player(tab2, cote, &x, &y);
  if(test_move(tab2, move, cote, x, y))
  {
    *GO=game_over(tab2, move, cote, x, y);
    if(win(tab2, move, cote, x, y))
    {
      return 1;
    }
    fct_move(tab2, move, cote, x, y);
  }
  return 0;
}

void play(int lvl)
{
  int win=0;
  int GO=0;
  int cote, nbr_enemy;
  char move;
  enemy grp_enmy[20];
  int** tab2 = select_level(&lvl, &cote, &nbr_enemy, grp_enmy);
  printf("Level: %d\n", lvl);
  do
  {
    system("clear");
    print_board(tab2, cote);
    printf("\nmove(w,a,s,d / z,q,s,d): \n");
    move=getch();
    fct_move_enmy(tab2, grp_enmy, cote, nbr_enemy);
    win=movement(tab2, move, cote, &GO);
  } while(!win && !GO);
  if(GO)
  {
    lvl--;
  }
  free_board(tab2, cote);
  if(lvl<8)
  {
    printf("Press any key to continue\n");
    getch();
    play(lvl);
  }
}

int main()
{
  int lvl=0;
  play(lvl);
  return 0;
}
