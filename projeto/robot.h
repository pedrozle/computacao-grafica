/* angulos para as articulacoes */
int deltaTime = 2;

/* Distância da câmera ao centro do mundo */
float cameraDistanceBase = 500.0f;
float cameraDistance = cameraDistanceBase;

/* Ângulo a ser adicionado na rotação da câmera */
float angleCamera = 5.0f;

/* Ângulo da câmera na coordenada X */
float cameraAngleXBase = 20.0f;
float cameraAngleX = 0.0f;
/* Ângulo da câmera na coordenada Y */
float cameraAngleYBase = 0.0f;
float cameraAngleY = 0.0f;

/* Variáveis para controlar animações */

int animacaoAtual = 1;

/* Cores para os robos */
float redExtremidades = 0, greenExtremidades = 0, blueExtremidades = 0;
float redMembros = 0, greenMembros = 0, blueMembros = 0;
float redCorpo = 0, greenCorpo = 0, blueCorpo = 0;

/* Bracos */
float bracoSx = 0.5f;
float bracoSy = 3.0f;
float bracoSz = 0.5f;

/* Pernas */
float pernaSx = 0.5f;
float pernaSy = 3.0f;
float pernaSz = 0.5f;

/* Junta */
float juntaTam = 4;

float baseAnim = 0.0f;
float deltaTimeBaseAnim = deltaTime;

float angleCorpoX = 0.0f;
float angleCorpoY = 0.0f;
float angleCorpoZ = 0.0f;

float deltaTimeAngleCorpoX = deltaTime;

// Tronco
float deltaTimeTroncoBase = 1.5;
float troncoAngleTopX = 0;
float troncoAngleTopY = 0;
float troncoAngleTopZ = 0;
float troncoAngleBotX = 0;
float troncoAngleBotY = 0;
float troncoAngleBotZ = 0;
float deltaTimeTroncoX = 1.5;
float deltaTimeTroncoZ = 0.5;
float deltaTimeTroncoY = 1.5;

float timerAnim = 0.0f;

/* Ombro */

float angleOmbroIdle = -45.0f;

float angleOmbroDirX = 0.0f;
float angleOmbroDirY = 0.0f;
float angleOmbroDirZ = 0.0f;
float angleOmbroEsqX = 0.0f;
float angleOmbroEsqY = 0.0f;
float angleOmbroEsqZ = 0.0f;

float deltaTimeOmbroBase = 2;
float deltaTimeOmbroX = deltaTimeOmbroBase;
float deltaTimeOmbroY = deltaTimeOmbroBase;
float deltaTimeOmbroZ = 0.5;

/* Cotovelo */
float cotoveloAngleIdle = -110;

float angleCotoveloDirX = 0.0f;
float angleCotoveloDirY = 0.0f;
float angleCotoveloDirZ = 0.0f;
float angleCotoveloEsqX = 0.0f;
float angleCotoveloEsqY = 0.0f;
float angleCotoveloEsqZ = 0.0f;

float deltaTimeCotoveloBase = 1.5;
float deltaTimeCotoveloX = deltaTimeCotoveloBase;
float deltaTimeCotoveloY = deltaTimeCotoveloBase;
float deltaTimeCotoveloZ = deltaTimeCotoveloBase;

// Perna
float anglePernaDirX = 0.0f;
float anglePernaDirY = 0.0f;
float anglePernaDirZ = 0.0f;
float anglePernaEsqX = 0.0f;
float anglePernaEsqY = 0.0f;
float anglePernaEsqZ = 0.0f;

// Coxa
float angleCoxaDirX = 0.0f;
float angleCoxaDirY = 0.0f;
float angleCoxaDirZ = 0.0f;
float angleCoxaEsqX = 0.0f;
float angleCoxaEsqY = 0.0f;
float angleCoxaEsqZ = 0.0f;


float deltaTimeCoxaBase = 1.5;
float deltaTimeCoxaX = deltaTimeCoxaBase;
float deltaTimeCoxaY = deltaTimeCoxaBase;
float deltaTimeCoxaZ = deltaTimeCoxaBase;

// Joelho
float angleJoelhoDirX = 0.0f;
float angleJoelhoDirY = 0.0f;
float angleJoelhoDirZ = 0.0f;
float angleJoelhoEsqX = 0.0f;
float angleJoelhoEsqY = 0.0f;
float angleJoelhoEsqZ = 0.0f;

float deltaTimeJoelho = deltaTime;

float transCorpoX = 0.0f;
float transCorpoY = 0.0f;
float transCorpoZ = 0.0f;