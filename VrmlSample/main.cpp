
#include <SudaRA.h>
#include <sstream>
#include <math.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <time.h>
/*#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif*/

using namespace sudara;


#define VELOMOVIMENTO 2   //velocidade de movimento da moto
#define PI 3.14159265
#define TEMPOMAX 30

class VrmlSample : public ARApplication
{
	private:
		Audio *voltaCompleta,*marcadorSome,*marcadorAparece, *musicaFundo;
		GuiImage *imgLogo;
		Cube *cube;
		GuiText *infoScene;
		Vector direction,mov,mov2,dSeta,movAnterior,posSeta,rotSeta,aux,controleVelocidade,controlePonteiro;
		StaticModel *guidao;
		StaticModel *seta;
		StaticModel	*pista;
		StaticModel *ponteiro;
		StaticModel *velocimetro;
		StaticModel *barra,*barra2;
		float angulo,ang,velocidade,dist,valorDist,velocidadeMoto;
		bool apareceu,travaAceleracao,travaReducao,saveImage,fimCalibracao,adicionarTempo,Aplicacao;
		float posPista,volta;
	public:
		VrmlSample() : ARApplication() {}
		Scene *marcador2 ;
		Scene *scene;
		std::string msg;
		std::string nomeArquivo;
		int contagem;
		int tempo_gasto,tmpCalibracao;
		clock_t t0, tf,tempoCalibracao;

		void initialize()
		{
			//CPage* teste;

			Aplicacao=true; //Mostra se o jogo pode ser executado
			tempo_gasto=1;
			t0 = TEMPOMAX;
			adicionarTempo=false; // Marca se o tempo da calibracao foi adicionado ao tempo da aplicacao
			fimCalibracao=false; // Indica quando a calibração terminou
			contagem=0; //Conta a quantidade de fotos tiradas
			saveImage=false;
			travaReducao=false;
			travaAceleracao=false; //
			apareceu=false;     // Variavel que marca se o marcador JEAN apareceu
			velocidade=10;      // Velocidade da seta, ainda em desuso
			movAnterior.x=0;    // Movimentos anteriores de Rotacao para realizar o calculo de posicao
			movAnterior.y=0;	//--
			movAnterior.z=0;	//--
			valorDist=0;        // valor que sera atribuido a variavel Dist que controlará a orientacao da seta
			posPista=0;         //Variavel que indicara a posicao da pista, dando a sensacao de movimento
			velocidadeMoto=-1.5;   // Define a velocidade da descida da pista
			volta=0;            // Voltas dadas no percurso
			controleVelocidade.y=0;
			controleVelocidade.x=0;
			msg="Mova a cabeca para o centro e tecle ENTER.";

			
			//Organizando a cena
			this->guiWindow = new GuiWindow;

			//Carrega a imagem do velocimetro e posiciona-o no canto direito superior
			this->imgLogo = new GuiImage();
			this->imgLogo->load(Vector(0, 0), "../media/lpg.png", true);
			this->imgLogo->setPosition(Vector(540, 60));//60
			

			ponteiro = new StaticModel;
			ponteiro->load("../media/ponteiro.obj");
			ponteiro->setScale(6);
			ponteiro->setRotationX(90);
			ponteiro->setRotationY(180);
			ponteiro->setRotationZ(260);
			controlePonteiro=ponteiro->getRotation();
			
			//Carregando audio
			voltaCompleta = new Audio;
			voltaCompleta->load("../media/volta.wav");
			marcadorSome = new Audio;
			marcadorSome ->load("../media/marcardorsome.wav");
			marcadorAparece = new Audio;
			marcadorAparece->load("../media/marcadoraparece.wav");
			musicaFundo = new Audio;
			musicaFundo->load("../media/musicafundo.wav");
			musicaFundo->play();
			musicaFundo->setLoop(true);

			//Marcador Hiro
			scene = new Scene;
			scene->load("../media/patt.hiro");
			
			//Marcador JeanPersonalizado
			marcador2 = new Scene;
			marcador2->load("../media/MarcadorJean");
			dSeta.z=mov2.z;
			
			//Adicionando marcadores na cena
			addScene(scene,"HIRO");
			addScene(marcador2,"JEAN");
			
			//Configurando objetos que serao exibidos
			//guidao
			guidao = new StaticModel;
			guidao->load("../media/guidao2.obj");

			//Miniatura da moto
			seta = new StaticModel;
			seta->load("../media/moto.obj");
			seta->setScale(30);
			seta->setRotationX(90);
			seta->setRotationY(180);
			seta->setRotationZ(90);

			//Barra de Calibracao
			barra = new StaticModel;
			barra->load("../media/Reta.obj");
			barra->setScale(15);
			barra->setRotationX(180);
			barra->setRotationY(180);
			barra->setRotationZ(180);
			addGlobalEntity(barra);

			barra2 = new StaticModel;
			barra2->load("../media/Reta2.obj");
			barra2->setScale(15);
			barra2->setRotationX(180);
			barra2->setRotationY(180);
			barra2->setRotationZ(270);
			addGlobalEntity(barra2);

			//pista
			pista = new StaticModel;
			pista->load("../media/pistaretablend.obj");
			pista->setScale(33);

			//Definindo valores iniciais para as variaveis de posicionamento
			mov.x=0;
			mov.y=-15;
			mov.z=57;
			dSeta.x=-49;
			dSeta.y=-50;
			dSeta.z=370;

			ponteiro->setPosition(Vector(130,90,390));
			//ponteiro->move(Vector(170,10,10));

			//Definindo posicao/rotacao da tela dos respectivos itens
			guidao->setPosition(mov);
			guidao->setRotationY(this->angulo+40);
			seta->setPosition(dSeta);
			barra->setPosition(Vector(0,0,370));
			barra2->setPosition(Vector(0,0,370));
			pista->setPosition(Vector(-20, 900, 400));
			pista->setRotationX(90);
			pista->setRotationY(180);
			
			
			//Adicionando Texto na parte superior esquerda da tela, que mostra as coordenadas do marcador
			this->infoScene = new GuiText;
			this->infoScene->load(Vector(10, 10), "", "../media/arial.ttf", Vector(1, 1, 1), 20);
			this->guiWindow->addEntity(this->infoScene);
			this->infoScene->setText(msg);

		}
//----------------------------------------------------------------------------------------------------------------
	void adicionaEntidade(){
			this->guiWindow->addEntity(this->imgLogo);//Adiciona Velocimetro
			this->addGlobalEntity(ponteiro); // Adiciona ponteiro Velocimetro
			addGlobalEntity(seta);//Adicionando Entidade Global da miniatura da moto 
			addGlobalEntity(pista);// Adiciona Pista
		}

	
//----------------------------------------------------------------------------------------------------------------
	void removeEntidade(){
		this->removeGlobalEntity(seta->getID());
		this->removeGlobalEntity(this->guidao->getID());
		this->removeGlobalEntity(pista->getID());
		this->removeGlobalEntity(ponteiro->getID());
		imgLogo->setPosition(Vector(900, 900));
	}
//----------------------------------------------------------------------------------------------------------------
	
	void update(int elapsedTime)
		{
			if (contagem==100 && Aplicacao==true){
				if(adicionarTempo==false){
					tmpCalibracao=tempoCalibracao/CLOCKS_PER_SEC;
					adicionarTempo=true;
				}
				tf = clock();
				tempo_gasto = (((( (int) ((tf - t0) ))/ CLOCKS_PER_SEC)-TEMPOMAX)*-1)+tmpCalibracao;
				mov2=this->marcador2->getPosition();
				mov.z=this->marcador2->getRotationZ();
				if (this->marcador2->isVisible()==true){
					if (this->apareceu==false) {
						this->addGlobalEntity(guidao);
						this->apareceu=true;
						marcadorAparece->play(); // Toca o som quando marcador aparece
						controleVelocidade.z=mov2.z;
					}
					movAnterior.z=mov.z;
					msg = "Marcador detectado     Voltas: ";
					msg = msg+ Convert(volta);
					msg=msg + " Tempo: " + Convert(tempo_gasto);
					this->guidao->setRotationZ(mov.z);
					seta->setRotationZ(mov.z-180); //rotaciona a moto para a frente, ja que apenas com a rotacao do marcador ela fica virada para trás
					this->infoScene->setText(msg);
				}else { // Else responsavel por remover o guidao da cena e exibir uma mensagem quando nao tiver marcadores em tela
					std::string msg;
					msg = "Sem marcadores em tela  ";
					msg=msg + " Tempo: " + Convert(tempo_gasto);
					this->infoScene->setText(msg);
					if (this->apareceu==true){
						this->removeGlobalEntity(this->guidao->getID());
						apareceu=false;
						travaAceleracao=false;
						marcadorSome->play();
					}
				}//Else
				velocidade=10;
				velocidade *= .98;
				dist = valorDist;
				if (apareceu){
					dSeta.x = ((sin(marcador2->getRotationZ()*(PI/180))/VELOMOVIMENTO*velocidade))*10;
					seta->move(Vector(dSeta.x-movAnterior.x,dSeta.y-movAnterior.y,0));
					pista->move(Vector(0,velocidadeMoto,0));
					aux=pista->getPosition();
					posPista=aux.y;
					seta->move(Vector(dist,0,0));
					
					//Aumentando a velocidade da moto a partir do movimento da cabeça
					if (this->controleVelocidade.z-mov2.z>40 && travaAceleracao==false && controlePonteiro.z<500){
						velocidadeMoto--;
						travaAceleracao=true; //Apenas permite que seja novamente acelerado se a cabeça voltar para trás
						ponteiro->setRotationZ(controlePonteiro.z+=40);
					}
					if (this->controleVelocidade.z-mov2.z<10 && travaAceleracao==true){
						travaAceleracao=false;
					}

					//diminuindo a velocidade
					if (controlePonteiro.z>=270 && controleVelocidade.z-mov2.z<-30 && travaReducao==false){
						velocidadeMoto++;
						ponteiro->setRotationZ(controlePonteiro.z-=40);
						travaReducao=true;
					}
					if (this->controleVelocidade.z-mov2.z>10 && travaReducao==true){
						travaReducao=false;
					}

					if(posPista<(-970)){
						pista->move(Vector(0,1900,0));
						posPista=0;
						volta++;
						voltaCompleta->play();
					}
				}
				movAnterior=dSeta;
				movAnterior.z=marcador2->getRotationZ();
				posSeta=seta->getPosition();
					
				//Limita o movimento do objeto Moto no espaço da tela
				if(posSeta.x>170){
					seta->setPosition(Vector(170,dSeta.y,dSeta.z));
				}
				if(posSeta.x<-170){
					seta->setPosition(Vector(-170,dSeta.y,dSeta.z));
				}
				rotSeta=seta->getRotation();
				if(rotSeta.z>45){
					valorDist=elapsedTime * -0.02f;
				}
				else valorDist=elapsedTime * 0.02f;
		}
		if (tempo_gasto==0 && Aplicacao==true){
			removeEntidade();
			addGlobalEntity(barra);
			addGlobalEntity(barra2);
			Aplicacao=false;
			msg="Tempo Esgotado. Posicione a cabeca ao centro da tela e tecle Enter";
			infoScene->setText(msg);
			fimCalibracao=false;
			contagem=0;
		}
}
//---------------------------------------------------------------------------------------------

		void onGetFrame(ImageBase videoFrame)
		{			
			if (this->saveImage)
			{
				switch (contagem){
					case 0:
						nomeArquivo="MeioTela.bmp";
						if (Aplicacao==false)
							nomeArquivo="MeioTelaFinal.bmp";
						msg="Mova a cabeca para a esquerda e tecle Enter";
						contagem=1;
						break;
					case 1:
						nomeArquivo="EsquerdaTela.jpg";
						if (Aplicacao==false)
							nomeArquivo="EsquerdaTelaFinal.jpg";
						msg="Mova a cabeca para a direita e tecle Enter";
						contagem=2;
						break;
					case 2:
						nomeArquivo="DireitaTela.jpg";
						if (Aplicacao==false)
							nomeArquivo="DireitaTelaFinal.jpg";
						msg="Mova a cabeca para cima e tecle Enter";
						contagem=3;
						break;
					case 3:
						nomeArquivo="CimaTela.jpg";
						if (Aplicacao==false)
							nomeArquivo="CimaTelaFinal.jpg";
						msg="Mova a cabeca para baixo e tecle Enter";
						contagem=4;
						break;
					case 4:
						nomeArquivo="BaixoTela.jpg";
						if (Aplicacao==false){
							nomeArquivo="BaixoTelaFinal.jpg";
							msg="Aplicacao Finalizada!";
						}
						else msg="PRESSIONE ENTER PARA INICIAR!";
						removeGlobalEntity(barra->getID());
						removeGlobalEntity(barra2->getID());
						contagem=5;
						break;
				}
				IplImage *image = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U, 4);
				image->imageData = (char *) videoFrame.getPixels();
				cvSaveImage(nomeArquivo.c_str(), image);
				this->infoScene->setText(msg);
				this->saveImage = false;
				if (fimCalibracao==true){
					if  (Aplicacao==true)
						adicionaEntidade();
					contagem=100; //Numero que anula o Switch acima
					tempoCalibracao=clock();
				}
				if (contagem==5){
					fimCalibracao=true;
				}
			}
		}

//---------------------------------------------------------------------------------------------
		void onKeyPressed(KeyboardButton key) 
		{
			switch(key)                              // Switch responsavel por aumentar ou diminuir a velocidade da moto conforme a tecla apertada
			{	
				case KEY_UP:             
				case KEY_w:
					velocidadeMoto--;                // Aumenta a velocidade
					break;
				case KEY_DOWN:
				case KEY_s:
					velocidadeMoto++;               // Diminui a velocidade
					break;
				case KEY_RETURN:
					saveImage=true;
			}
		}

//----------------------------------------------------------------------------------------------------------------
		std::string Convert (float number){
			std::ostringstream buff;
			buff<<number;
			return buff.str();
		}
//----------------------------------------------------------------------------------------------------------------
	/*	void PrintForm4(CPage* ps){

			double row=ps->Print(0.0,0.0,TEXT_NORMAL|TEXT_CENTER,24,"Relatório de Imagens do atual paciente");
			row=ps->Print(row,0.0,TEXT_BOLD|TEXT_ITALIC|TEXT_CENTER,24,"Posição da cabeça antes da aplicação");
			//	same bitmap different size bounding rectangle
			ps->PrintBitMap(1.0,1.0,3.2,4.0,"MeioTela.bmp");
			ps->PrintBitMap(1.0,4.5,3.2,7.5,"MeioTela.bmp");

			ps->PrintBitMap(3.6,1.0,5.8,4.0,"MeioTela.bmp");
			ps->PrintBitMap(3.6,4.5,5.8,7.5,"MeioTela.bmp");

			ps->PrintBitMap(6.2,1.0,8.4,4.0,"MeioTela.bmp");
			ps->PrintBitMap(6.2,4.5,8.4,7.5,"MeioTela.bmp");

			ps->PrintBitMap(8.8,1.0,11.0,4.0,"MeioTela.bmp");
			ps->PrintBitMap(8.8,4.5,11.0,7.5,"MeioTela.bmp");

			ps->PrintBitMap(11.8,1.0,14.0,4.0,"MeioTela.bmp");
			ps->PrintBitMap(8.8,4.5,11.0,7.5,"MeioTela.bmp");
			//	place text over bit map. Looks different in preview than in printing
		}

	void PrintForm1(CPage* ps){
			double row=ps->Print(0.0,0.0,TEXT_NORMAL|TEXT_CENTER,24,"Relatório de Imagens do atual paciente");
			row=ps->Print(row,0.0,TEXT_BOLD|TEXT_ITALIC|TEXT_CENTER,24,"Posição da cabeça antes da aplicação");
			//	same bitmap different size bounding rectangle
			ps->PrintBitMap(1.0,1.0,3.2,4.0,"MeioTela.bmp");
			ps->PrintBitMap(1.0,4.5,3.2,7.5,"MeioTela.bmp");
			//	place text over bit map. Looks different in preview than in printing
	}
	*/


//----------------------------------------------------------------------------------------------------------------
};

int main(int argc, char **argv)
{
	VrmlSample().go("Tratamento Torcicolo", "../media/WDM_camera_flipV.xml", "../media/camera_para.dat");
	return (0);
}