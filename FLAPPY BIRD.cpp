#include<iostream>
#include<conio.h>
#include<windows.h>
#include<fstream>
#include<ctime>
#include<string>

#define SCREEN_WIDTH 90
#define SCREEN_HEIGHT 26
#define WIN_WIDTH 70
#define GAP_SIZE 7

using namespace std;
 
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD CursorPosition;

int pipePos[3];
int gapPos[3];
int pipeFlag[3];
char bird[2][6] = { '/','-','-','o','\\',' ',
					'|','_','_','_',' ','>' };
int birdPos = 6;
int score = 0;
bool playing = false;

void clear() {
    COORD topLeft  = { 0, 0 };
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;

    GetConsoleScreenBufferInfo(console, &screen);
    FillConsoleOutputCharacterA(
        console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    FillConsoleOutputAttribute(
        console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
        screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    SetConsoleCursorPosition(console, topLeft);
}

void gotoxy(int x, int y)
{
	CursorPosition.X = x;
	CursorPosition.Y = y;
	SetConsoleCursorPosition(console, CursorPosition);
}

void setcursor(bool visible, DWORD size) 
{
	CONSOLE_CURSOR_INFO lpCursor;	
	lpCursor.bVisible = visible;
	lpCursor.dwSize = size;
	SetConsoleCursorInfo(console,&lpCursor);
}

void set_leaderboard(int num){
	ifstream f("data.txt");
	int data_text;
	if (f.is_open()){
		f>>data_text;
	};

	// compare and set
	if (data_text <= num){
		time_t t = std::time(0);   // get time now
		tm* now = std::localtime(&t);

		ofstream output("data.txt");
		output << num << endl << now->tm_mday << '-' 
         << (now->tm_mon + 1) << '-'
         << (now->tm_year + 1900)
         << "\n"; // store data
		output.close();
	}
}

void drawBorder(){ 
	
	for(int i=0; i<SCREEN_WIDTH; i++){
		gotoxy(i,0); cout<<"±";
		gotoxy(i,SCREEN_HEIGHT); cout<<"±";
	}
	
	for(int i=0; i<SCREEN_HEIGHT; i++){
		gotoxy(0,i); cout<<"±";
		gotoxy(SCREEN_WIDTH,i); cout<<"±";
	}
	for(int i=0; i<SCREEN_HEIGHT; i++){
		gotoxy(WIN_WIDTH,i); cout<<"±";
	}
}
void genPipe(int ind){
	gapPos[ind] = 3 + rand()%14; 
}
void drawPipe(int ind){
	if( pipeFlag[ind] == true ){
		for(int i=0; i<gapPos[ind]; i++){ //atas
			SetConsoleTextAttribute(console, 10);
			gotoxy(WIN_WIDTH-pipePos[ind],i+1); cout<<"***"; 
		}
		for(int i=gapPos[ind]+GAP_SIZE; i<SCREEN_HEIGHT-1; i++){ //bawah
			SetConsoleTextAttribute(console, 10);
			gotoxy(WIN_WIDTH-pipePos[ind],i+1); cout<<"***"; 
		}
	} 
}

void erasePipe(int ind){
	if( pipeFlag[ind] == true ){
		for(int i=0; i<gapPos[ind]; i++){ 
			gotoxy(WIN_WIDTH-pipePos[ind],i+1); cout<<"   "; 
		}
		for(int i=gapPos[ind]+GAP_SIZE; i<SCREEN_HEIGHT-1; i++){ 
			gotoxy(WIN_WIDTH-pipePos[ind],i+1); cout<<"   "; 
		}
	}
}

void drawBird(){
	for(int i=0; i<2; i++){
		for(int j=0; j<6; j++){
			SetConsoleTextAttribute(console, 14);
			gotoxy(j+2,i+birdPos); cout<<bird[i][j];
		}
	}
}

void eraseBird(){
	for(int i=0; i<2; i++){
		for(int j=0; j<6; j++){
			gotoxy(j+2,i+birdPos); cout<<" ";
		}
	}
}

int collision(){
	if( pipePos[0] >= 61  ){ // area burung
		if( birdPos<gapPos[0] || birdPos >gapPos[0]+GAP_SIZE ){
			return 1;
		}
	}
	return 0;
}


void gameover(){
	playing = false;
	set_leaderboard(score);
	clear();;
	cout<<endl;
	SetConsoleTextAttribute(console, 8);
	cout<<"\t\t--------------------------"<<endl;
	SetConsoleTextAttribute(console, 12);
	cout<<"\t\t-------- Game Over -------"<<endl;
	SetConsoleTextAttribute(console, 8);
	cout<<"\t\t--------------------------"<<endl<<endl;
	SetConsoleTextAttribute(console, 10);
	cout<<"\t\tSkor : "<<score<<endl;
	SetConsoleTextAttribute(console, 7);
	cout<<"\t\tPencet apapun untuk kembali ke menu.";
	getch();
}

void updateScore(){
	SetConsoleTextAttribute(console, 10);
	gotoxy(WIN_WIDTH + 7, 5); cout<<"Skor: "<<score<<endl;
	SetConsoleTextAttribute(console, 7);
}

void instructions(){
	
	clear();
	SetConsoleTextAttribute(console, 10);
	cout<<"Instruksi";
	SetConsoleTextAttribute(console, 7);
	cout<<"\n-----------------";
	cout<<"\nPencet spasi untuk membuat burung melompat";
	cout<<"\nHindari pipa untuk mendapatkan skor";
	cout<<"\nRaih skor sebanyak mungkin";
	cout<<"\n\nPencet apapun untuk kembali ke menu";
	getch();
}

void leaderboard(){
	ifstream file ("data.txt");

    if ( file.is_open() ) {
		clear();
		SetConsoleTextAttribute(console, 10);
		cout<<"Papan Skor";
		SetConsoleTextAttribute(console, 7);
		cout<<"\n-----------------";

		string line;
      	for (int i = 0; i < 2; i++) {
			getline (file, line);
			if (i == 0){
				cout<<"\nSkor tertinggi: "<<line;
			} 
			else {
				cout<<"\nDiraih pada tanggal: "<<line;
			}
			
        }
		cout<<"\n\nPencet apapun untuk kembali ke menu";
    } else{
		clear();
		cout << "Error details: " << strerror(errno)
                 << endl;
		//return;
	}
	getch();
}

void play(){
	
	birdPos = 6; //y
	score = 0;
	pipeFlag[0] = 1;
	pipeFlag[1] = 0;
	pipePos[0] = pipePos[1] = 4;
	
	clear();
	drawBorder();
	genPipe(0);
	updateScore();
	
	SetConsoleTextAttribute(console, 14);
	gotoxy(WIN_WIDTH + 5, 2); cout<<"FLAPPY BIRD";
	SetConsoleTextAttribute(console, 7);
	gotoxy(WIN_WIDTH + 6, 4); cout<<"-----------";
	gotoxy(WIN_WIDTH + 6, 6); cout<<"-----------";
	gotoxy(WIN_WIDTH + 7, 12); cout<<"Kontrol";
	gotoxy(WIN_WIDTH + 7, 13); cout<<"--------";
	gotoxy(WIN_WIDTH + 4, 14); cout<<"Spasi = Lompat";
	
	gotoxy(10, 5); cout<<"Pencet apapun untuk memulai";
	getch();
	gotoxy(10,5); cout<<"                           ";
	
	// set playing to true
	playing = true;
	
	while(playing){
		
		if(kbhit()){
			char ch = getch();
			if( ch==32){
				if( birdPos > 3)
					birdPos-=3;
			}
		}	
	
	
		drawBird();
		drawPipe(0);
		drawPipe(1);
		if( collision()==1 ){
			gameover();
		}
		Sleep(100);
		eraseBird();
		erasePipe(0);
		erasePipe(1);
		birdPos += 1;
		
		if(  birdPos > SCREEN_HEIGHT -2 ){
			gameover();
	
		}
		
		if(  pipeFlag[0] == 1 )
			pipePos[0] +=2;
		if(  pipeFlag[1] == 1 )
			pipePos[1] +=2;
		if ( pipePos[0] >= 40 && pipePos[0] <42){
			pipeFlag[1] = 1;
			genPipe(1);
		}
		if(pipePos[0]> 68){
			score++;
			updateScore();
			// set current pipe to pipe 0
			pipeFlag[0] = 1;
			pipePos[0] = pipePos[1];
			gapPos[0] = gapPos[1];
			// clear last pipe for new pipe
			pipeFlag[1] = 0;
			pipePos[1] = 4;
			gapPos[1] = 0;
		}
		
		}
}

int main(){
	
	SetConsoleTitle(TEXT("Flappy Bird"));
	setcursor(0,20);
	srand( (unsigned)time(NULL));
	
	do{
		
		clear();;
		SetConsoleTextAttribute(console, 14);
		gotoxy(10,0); cout<<R"(
  ______ _                           ____  _         _ 
 |  ____| |                         |  _ \(_)       | |
 | |__  | | __ _ _ __  _ __  _   _  | |_) |_ _ __ __| |
 |  __| | |/ _` | '_ \| '_ \| | | | |  _ <| | '__/ _` |
 | |    | | (_| | |_) | |_) | |_| | | |_) | | | | (_| |
 |_|    |_|\__,_| .__/| .__/ \__, | |____/|_|_|  \__,_|
                | |   | |     __/ |                    
                |_|   |_|    |___/                     
)";
		SetConsoleTextAttribute(console, 7);
		gotoxy(10,11); cout<<"1. Mulai Game ";
		gotoxy(10,12); cout<<"2. Instruksi";
		gotoxy(10,13); cout<<"3. Papan Skor";
		gotoxy(10,14); cout<<"4. Keluar";
		gotoxy(10,16); cout<<"Pilih salah satu opsi.";
 		char op = getch();
 		
 		if( op =='1') play();
 		else if( op =='2') instructions();
		else if( op =='3') leaderboard();
 		else if(op == '4') exit(0);
	}while(1);
	
	return 0;
	
}


